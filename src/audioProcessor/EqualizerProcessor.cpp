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
    auto morselWrapper = morselSocket->receiveSerializedData(zmq::recv_flags::dontwait);
    if (morselWrapper != nullptr) {
        if (morselWrapper->getIdentifier() == ImpresarioSerialization::Identifier::floatArrayMorsel) {
            auto morsel = ImpresarioSerialization::GetFloatArrayMorsel(morselWrapper->getBuffer());
            if (morsel->field() == 0) {
                scalingFactors[100] = defaultScalingFactors[100] * morsel->value()->Get(0);
                scalingFactors[200] = defaultScalingFactors[200] * morsel->value()->Get(0);
                scalingFactors[400] = defaultScalingFactors[400] * morsel->value()->Get(1);
                scalingFactors[1000] = defaultScalingFactors[1000] * morsel->value()->Get(1);
                scalingFactors[2000] = defaultScalingFactors[2000] * morsel->value()->Get(2);
                scalingFactors[20000] = defaultScalingFactors[20000] * morsel->value()->Get(2);
            }
        }
    }
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
