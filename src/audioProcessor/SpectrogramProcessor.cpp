#include "SpectrogramProcessor.h"

namespace conductor {

SpectrogramProcessor::SpectrogramProcessor(std::unique_ptr<PacketReceiver<MelSignalPacket>> input,
                                           std::unique_ptr<PacketDispatcher<SpectrogramPacket>> output)
        : input{move(input)},
          output{move(output)},
          lastPacket{} {

}

void SpectrogramProcessor::process() {
    if (lastPacket == nullptr) {
        lastPacket = input->getPacket();
    }
    auto packet = input->getPacket();
    auto &melSignal = *packet;
    auto &harmonicTransform = melSignal.getHarmonicTransformPacket();
    auto &stft = harmonicTransform.getSTFTPacket();

    // psi hardcoded here
    auto psi = 6;

    std::vector<double> normalizedTimbralSignals{};
    for (int index = 0; index < stft.size(); index++) {
        float largestSample = 0;
        for (int psindex = 0; psindex < psi; psindex++) {
            auto sample = harmonicTransform.getSample(index * psi + psindex);
            if (sample > largestSample) {
                largestSample = sample;
            }
        }
        for (int psindex = 0; psindex < psi; psindex++) {
            auto sample = harmonicTransform.getSample(index * psi + psindex);
            normalizedTimbralSignals.push_back(sample / largestSample);
        }
    }

    auto layerCount = 4;
    for (auto iteration = 0; iteration < psi; iteration++) {
        auto outputSpectrogramPacket = std::make_unique<SpectrogramPacket>(
                harmonicTransform.getOriginTimestamp(), harmonicTransform.getFrequencyBand(), layerCount
        );
        std::vector<double> outputData{};
        outputData.resize(layerCount);
        for (int index = 0; index < stft.size(); index++) {
            if (iteration == 0) {
                outputData[0] = stft.getMagnitude(index);
            } else {
                outputData[0] = 0;
            }
            outputData[1] = stft.getMagnitude(index);
            auto timbralIndex = index * psi + iteration;
            outputData[2] = harmonicTransform.getSample(timbralIndex);
            outputData[3] = normalizedTimbralSignals[timbralIndex];
            outputSpectrogramPacket->addData(outputData);
        }
        output->sendPacket(move(outputSpectrogramPacket));
    }
    lastPacket = move(packet);
}

}
