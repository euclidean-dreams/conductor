#include "STFTProcessor.h"

namespace conductor {

STFTProcessor::STFTProcessor(std::unique_ptr<PacketSpout> input, std::unique_ptr<PacketConduit> output)
        : input{move(input)},
          output{move(output)},
          fftInput{static_cast<float *>(mufft_alloc(FFT_SIZE * sizeof(float)))},
          fftOutput{static_cast<cfloat *>(mufft_alloc(FFT_SIZE * sizeof(cfloat)))},
          fftPlan{mufft_create_plan_1d_r2c(FFT_SIZE, 0)},
          currentPackets{} {
}

STFTProcessor::~STFTProcessor() {
    mufft_free(fftInput);
    mufft_free(fftOutput);
    mufft_free(fftPlan);
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
                fftInput[fftInputIndex] = sample * hannWindow(fftInputIndex);
                fftInputIndex++;
            }
        }
    }
    mufft_execute_plan_1d(fftPlan, fftOutput, fftInput);
    auto earliestTimestamp = RawAudioPacket::from(currentPackets.front()->getPacket(0)).getSampleTimestamp();
    auto outputPacket = std::make_unique<STFTPacket>(earliestTimestamp, ImpresarioSerialization::FrequencyBand::all,
                                                     FFT_SIZE / 2);
    for (int i = 0; i < FFT_SIZE / 2; i++) {
        auto magnitude = std::sqrt(std::pow(fftOutput[i].real, 2) + std::pow(fftOutput[i].imag, 2));
        outputPacket->addSample(magnitude);
    }
    output->sendPacket(move(outputPacket));
}

float STFTProcessor::hannWindow(int sampleNumber) {
    return static_cast<float>(0.5 * (1 - cos((2 * M_PI * sampleNumber) / (FFT_SIZE - 1))));
}

bool STFTProcessor::shouldContinue() {
    return true;
}

}
