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

    std::vector<double> signalDerivative{};
    signalDerivative.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        if (index == 0) {
            signalDerivative.push_back(0);
        } else {
            auto derivative = stft.getMagnitude(index) - stft.getMagnitude(index - 1);
            signalDerivative.push_back(derivative);
        }
    }

    std::vector<double> horizontalHarmonicDerivative{};
    horizontalHarmonicDerivative.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        auto derivative = harmonicTransform.getSample(index) - (*lastPacket).getHarmonicTransformPacket().getSample(index);
        horizontalHarmonicDerivative.push_back(derivative);
    }

    std::vector<double> verticalHarmonicDerivative{};
    verticalHarmonicDerivative.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        if (index == 0) {
            verticalHarmonicDerivative.push_back(0);
        } else {
            auto derivative = harmonicTransform.getSample(index) - harmonicTransform.getSample(index - 1);
            verticalHarmonicDerivative.push_back(derivative);
        }
    }

    std::vector<double> harmonicallyScaledSignal{};
    harmonicallyScaledSignal.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        harmonicallyScaledSignal.push_back(stft.getMagnitude(index) * harmonicTransform.getSample(index));
    }

    std::vector<double> horizontalScaledDerivative{};
    horizontalScaledDerivative.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        auto derivative = harmonicallyScaledSignal[index]
                          - ((*lastPacket).getHarmonicTransformPacket().getSample(index)
                             * (*lastPacket).getHarmonicTransformPacket().getSTFTPacket().getMagnitude(index));
        horizontalScaledDerivative.push_back(derivative);
    }

    std::vector<double> verticalScaledDerivative{};
    verticalScaledDerivative.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        if (index == 0) {
            verticalScaledDerivative.push_back(0);
        } else {
            auto derivative = harmonicallyScaledSignal[index] - harmonicallyScaledSignal[index - 1];
            verticalScaledDerivative.push_back(derivative);
        }
    }

    auto layerCount = 3;
    auto outputSpectrogramPacket = std::make_unique<SpectrogramPacket>(
            harmonicTransform.getOriginTimestamp(), layerCount
    );
    std::vector<double> outputData{};
    outputData.resize(layerCount);
    for (int index = 0; index < harmonicTransform.size(); index++) {
        outputData[0] = stft.getMagnitude(index);
        outputData[1] = harmonicTransform.getSample(index);
        if (outputData[1] < 0) {
            outputData[1] = 0;
        }
        outputData[2] = harmonicallyScaledSignal[index];
//        outputData[3] = verticalScaledDerivative[index];
//
////        // playground
//        auto fireCount = 0;
//        if (index < harmonicTransform.size() - 1) {
//            if (verticalScaledDerivative[index + 1] <= 0
//                && verticalScaledDerivative[index] > 0
//                && harmonicallyScaledSignal[index] > 50) {
//                outputData[4] = harmonicallyScaledSignal[index];
//                fireCount++;
//            } else {
//                outputData[4] = 0;
//            }
//        } else {
//            outputData[4] = 0;
//        }
//        if (index < melSignal.size()) {
//            outputData[5] = melSignal.getSample(index);
//        } else {
//            outputData[5] = 0;
//        }
        outputSpectrogramPacket->addData(outputData);
    }
    output->sendPacket(move(outputSpectrogramPacket));
    lastPacket = move(packet);
}

}
