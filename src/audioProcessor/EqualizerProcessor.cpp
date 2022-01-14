#include "EqualizerProcessor.h"

namespace conductor {

EqualizerProcessor::EqualizerProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                       std::unique_ptr<PacketDispatcher<STFTPacket>> output,
                                       std::unique_ptr<impresarioUtils::NetworkSocket> morselSocket)
        : input{move(input)},
          output{move(output)},
          morselSocket{move(morselSocket)},
          defaultScalingFactors{Config::getInstance().getEqualizerScalingValues()},
          scalingFactors{Config::getInstance().getEqualizerScalingValues()},
          sampleRate{Config::getInstance().getSampleRate()} {

}

void EqualizerProcessor::process() {
    auto parcel = morselSocket->receiveParcel(zmq::recv_flags::dontwait);
    if (parcel != nullptr) {
        if (parcel->getIdentifier() == ImpresarioSerialization::Identifier::axiomology) {
            auto axiomology = impresarioUtils::Unwrap::Axiomology(*parcel);
            auto gain = axiomology->axioms()->Get(1);
            scalingFactors[100] = gain * defaultScalingFactors[100];
            scalingFactors[200] = gain * defaultScalingFactors[200];
            scalingFactors[400] = gain * defaultScalingFactors[400];
            scalingFactors[1000] = gain * defaultScalingFactors[1000];
            scalingFactors[2000] = gain * defaultScalingFactors[2000];
            scalingFactors[20000] = gain * defaultScalingFactors[20000];
        }
    }
    auto currentPacket = input->getPacket();
    auto &stftInput = *currentPacket;
    auto outputPacket = std::make_unique<STFTPacket>(
            stftInput.getOriginTimestamp(), stftInput.size(), stftInput.getFFTSize()
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
