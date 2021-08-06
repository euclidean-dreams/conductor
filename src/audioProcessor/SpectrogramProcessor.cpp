#include "SpectrogramProcessor.h"

namespace conductor {

SpectrogramProcessor::SpectrogramProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
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
    auto &harmonicTransform = *packet;
    auto &stft = harmonicTransform.getSTFTPacket();

    std::vector<double> harmonicallyScaledSignal{};
    harmonicallyScaledSignal.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        auto mid = harmonicTransform.getSample(index);
        if (mid < 0) {
            mid = 0;
        }
        harmonicallyScaledSignal.push_back(stft.getMagnitude(index) * mid);
    }

    std::vector<double> drummyTwoSignal{};
    for (int index = 0; index < harmonicTransform.size(); index++) {
        auto sample = stft.getMagnitude(index) - harmonicallyScaledSignal[index] * 10;
        if (sample < 0) {
            sample = 0;
        }
        drummyTwoSignal.push_back(sample);
    }

    auto layerCount = 4;
    auto outputSpectrogramPacket = std::make_unique<SpectrogramPacket>(
            harmonicTransform.getOriginTimestamp(), harmonicTransform.getFrequencyBand(), layerCount
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
        outputData[3] = drummyTwoSignal[index];
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
    std::cout << "w" << std::endl;
    lastPacket = move(packet);
}

}
