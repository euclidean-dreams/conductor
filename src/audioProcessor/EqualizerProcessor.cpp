#include "EqualizerProcessor.h"

namespace conductor {

EqualizerProcessor::EqualizerProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                       std::unique_ptr<PacketDispatcher<STFTPacket>> output)
        : input{move(input)},
          output{move(output)},
          scalingFactors{Config::getInstance().getEqualizerScalingValues()},
          sampleRate{Config::getInstance().getSampleRate()} {

}

void EqualizerProcessor::process() {
    auto currentPacket = input->getPacket();
    auto &stftInput = *currentPacket;
    auto outputPacket = std::make_unique<STFTPacket>(
            stftInput.getOriginTimestamp(), stftInput.getFrequencyBand(), stftInput.size(), stftInput.getFFTSize()
    );
    auto frequencyIncrement = sampleRate / stftInput.getFFTSize();
    auto currentScalingFactorIterator = begin(scalingFactors);
    for (int index = 0; index < stftInput.size(); index++) {
        if (index * frequencyIncrement > currentScalingFactorIterator->first) {
            currentScalingFactorIterator++;
        }
        auto inputSample = stftInput.getSample(index);
        auto outputSample = inputSample * currentScalingFactorIterator->second;
        outputPacket->addSample(outputSample);
    }
    output->sendPacket(move(outputPacket));
}

}
