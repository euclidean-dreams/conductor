#include "STFTProcessor.h"

namespace conductor {

STFTProcessor::STFTProcessor(std::unique_ptr<PacketSpout> input, std::unique_ptr<PacketConduit> output)
        : input{move(input)},
          output{move(output)},
          fftInput{static_cast<ne10_float32_t *>(malloc(FFT_SIZE * sizeof(ne10_float32_t)))},
          fftOutput{static_cast<ne10_fft_cpx_float32_t *>(malloc(FFT_SIZE * sizeof(ne10_fft_cpx_float32_t)))},
          fftPlan{ne10_fft_alloc_r2c_float32(FFT_SIZE)},
          currentPackets{} {
}

STFTProcessor::~STFTProcessor() {
    NE10_FREE(fftInput);
    NE10_FREE(fftOutput);
    ne10_fft_destroy_r2c_float32(fftPlan);
}

void STFTProcessor::setup() {

}

void STFTProcessor::process() {
    if (currentPackets.empty()) {
        for (int i = 0; i < WINDOW_SIZE / HOP_SIZE; i++) {
            currentPackets.push_back(input->getPackets(HOP_SIZE));
        }
    } else {
        currentPackets.pop_front();
        currentPackets.push_back(input->getPackets(HOP_SIZE));
    }
    auto fftInputIndex = 0;
    for (auto &packetCollection: currentPackets) {
        for (auto &packetPointer: *packetCollection) {
            auto &packet = RawAudioPacket::from(*packetPointer);
            for (int i = 0; i < packet.size(); i++) {
                auto sample = packet.getSample(i);
                fftInput[fftInputIndex] = sample * hammingWindow(fftInputIndex);
                fftInputIndex++;
            }
        }
    }
    ne10_fft_r2c_1d_float32_neon(fftOutput, fftInput, fftPlan);
    auto earliestTimestamp = RawAudioPacket::from(currentPackets.front()->getPacket(0)).getSampleTimestamp();
    auto outputPacket = std::make_unique<STFTPacket>(earliestTimestamp, ImpresarioSerialization::FrequencyBand::all,
                                                     FFT_SIZE / 2 + 1);
    for (int i = 0; i < FFT_SIZE / 2 + 1; i++) {
        outputPacket->addSample(fftOutput[i].r, fftOutput[i].i);
    }
    output->sendPacket(move(outputPacket));
}

float STFTProcessor::hammingWindow(int sampleNumber) {
    return static_cast<float>(0.53836 - 0.46164 * std::cos((2 * M_PI * sampleNumber) / (FFT_SIZE - 1)));
}

bool STFTProcessor::shouldContinue() {
    return true;
}

}
