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
#ifdef USE_MUFFT
          fftInput{static_cast<float *>(mufft_alloc(fftSize * sizeof(float)))},
          fftOutput{static_cast<cfloat *>(mufft_alloc(fftSize * sizeof(cfloat)))},
          fftPlan{mufft_create_plan_1d_r2c(fftSize, 0)},
#else
        fftInput{static_cast<ne10_float32_t *>(malloc(fftSize * sizeof(ne10_float32_t)))},
        fftOutput{static_cast<ne10_fft_cpx_float32_t *>(malloc(fftSize * sizeof(ne10_fft_cpx_float32_t)))},
        fftPlan{ne10_fft_alloc_r2c_float32(fftSize)},
#endif // USE_MUFFT
          currentPackets{} {
    for (int i = 0; i < fftSize; i++) {
        fftInput[i] = 0;
    }
}

STFTProcessor::~STFTProcessor() {
#ifdef USE_MUFFT
    mufft_free(fftInput);
    mufft_free(fftOutput);
    mufft_free(fftPlan);
#else
    NE10_FREE(fftInput);
    NE10_FREE(fftOutput);
    ne10_fft_destroy_r2c_float32(fftPlan);
#endif // USE_MUFFT
}

void STFTProcessor::process() {
    if (currentPackets.empty()) {
        for (int i = 0; i < windowSize / hopSize; i++) {
            currentPackets.push_back(input->getPackets(hopSize));
        }
    } else {
        currentPackets.pop_front();
        currentPackets.push_back(input->getPackets(hopSize));
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

#ifdef USE_MUFFT
    mufft_execute_plan_1d(fftPlan, fftOutput, fftInput);
#else
    ne10_fft_r2c_1d_float32_neon(fftOutput, fftInput, fftPlan);
#endif // USE_MUFFT

    auto &mostRecentPacket = currentPackets.front()->getPacket(0);
    auto outputPacket = std::make_unique<STFTPacket>(mostRecentPacket.getOriginTimestamp(), fftSize / 2 + 1, fftSize);
    for (int i = 0; i < fftSize / 2 + 1; i++) {
#ifdef USE_MUFFT
        auto sample = std::complex<float>(fftOutput[i].real, fftOutput[i].imag);
#else
        auto sample = std::complex<float>(fftOutput[i].r, fftOutput[i].i);
#endif // USE_MUFFT
        outputPacket->addSample(sample);
    }
    output->sendPacket(move(outputPacket));
}

float STFTProcessor::hammingWindow(int sampleNumber) const {
    return static_cast<float>(0.53836 - 0.46164 * std::cos((2 * M_PI * sampleNumber) / (fftSize - 1)));
}

float STFTProcessor::hannWindow(int sampleNumber) const {
    return static_cast<float>(0.5 * (1 - std::cos((2 * M_PI * sampleNumber) / (fftSize - 1))));
}

}
