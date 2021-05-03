#include "SpectrogramProcessor.h"

namespace conductor {

SpectrogramProcessor::SpectrogramProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                                           std::unique_ptr<PacketDispatcher<SpectrogramPacket>> output)
        : input{move(input)},
          output{move(output)} {

}

void SpectrogramProcessor::process() {
    auto packet = input->getPacket();
    auto &harmonicTransform = *packet;;
    auto &stft = harmonicTransform.getSTFTPacket();

    auto layerCount = 2;
    auto outputSpectrogramPacket = std::make_unique<SpectrogramPacket>(
            harmonicTransform.getOriginTimestamp(), harmonicTransform.getFrequencyBand(), layerCount
    );
    std::vector<double> outputData{};
    outputData.resize(layerCount);
    for (int index = 0; index < harmonicTransform.size(); index++) {
        outputData[0] = stft.getMagnitude(index);
        outputData[1] = harmonicTransform.getSample(index);
        outputSpectrogramPacket->addData(outputData);
    }
    output->sendPacket(move(outputSpectrogramPacket));
}

}
