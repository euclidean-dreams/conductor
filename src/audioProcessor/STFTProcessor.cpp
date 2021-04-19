#include "STFTProcessor.h"

namespace conductor {

STFTProcessor::STFTProcessor(std::unique_ptr<PacketReceiver<RawAudioPacket>> input,
                             std::unique_ptr<PacketDispatcher<STFTPacket>> output,
                             WindowFunction windowFunction)
        : input{move(input)},
          output{move(output)},
          windowFunction{windowFunction},
          hopSize{Config::getInstance().getSTFTHopSize()},
          windowSize{Config::getInstance().getSTFTWindowSize()},
          fftSize{windowSize * Config::getInstance().getAudioPacketSize()},
          fftInput{static_cast<ne10_float32_t *>(malloc(fftSize * sizeof(ne10_float32_t)))},
          fftOutput{static_cast<ne10_fft_cpx_float32_t *>(malloc(fftSize * sizeof(ne10_fft_cpx_float32_t)))},
          fftPlan{ne10_fft_alloc_r2c_float32(fftSize)},
          currentPackets{} {
}

STFTProcessor::~STFTProcessor() {
    NE10_FREE(fftInput);
    NE10_FREE(fftOutput);
    ne10_fft_destroy_r2c_float32(fftPlan);
}

void STFTProcessor::process() {
    if (currentPackets.empty()) {
        for (int i = 0; i < windowSize / hopSize; i++) {
            currentPackets.push_front(input->getPackets(hopSize));
        }
    } else {
        currentPackets.pop_back();
        currentPackets.push_front(input->getPackets(hopSize));
    }
    auto fftInputIndex = 0;
    for (auto &packetCollection: currentPackets) {
        for (auto &packet: *packetCollection) {
            for (int i = 0; i < packet->size(); i++) {
                auto sample = packet->getSample(i);
                if (windowFunction == WindowFunction::hann) {
                    fftInput[fftInputIndex] = sample * hannWindow(fftInputIndex);
                } else if (windowFunction == WindowFunction::hamming) {
                    fftInput[fftInputIndex] = sample * hammingWindow(fftInputIndex);
                } else {
                    throw std::logic_error{"invalid window function selected"};
                }
                fftInputIndex++;
            }
        }
    }
    ne10_fft_r2c_1d_float32_neon(fftOutput, fftInput, fftPlan);
    auto &mostRecentPacket = currentPackets.front()->getPacket(0);
    auto outputPacket = std::make_unique<STFTPacket>(mostRecentPacket.getOriginTimestamp(),
                                                     mostRecentPacket.getFrequencyBand(), fftSize / 2 + 1);
    for (int i = 0; i < fftSize / 2 + 1; i++) {
        outputPacket->addSample(fftOutput[i].r, fftOutput[i].i);
    }
    output->sendPacket(move(outputPacket));
}

float STFTProcessor::hammingWindow(int sampleNumber) {
    return static_cast<float>(0.53836 - 0.46164 * std::cos((2 * M_PI * sampleNumber) / (fftSize - 1)));
}

float STFTProcessor::hannWindow(int sampleNumber) {
    return static_cast<float>(0.5 * (1 - std::cos((2 * M_PI * sampleNumber) / (fftSize - 1))));
}

}
