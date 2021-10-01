#include "MaterializationProcessor.h"

namespace conductor {

MaterializationProcessor::MaterializationProcessor(std::unique_ptr<PacketReceiver<MelSignalPacket>> input,
                                                   std::unique_ptr<PacketDispatcher<Serializable>> output)
        : input{move(input)},
          output{move(output)},
          previousSignal{},
          previousPerceptions{},
          perceptionDecay{2},
          contributionRadius{5} {

}

void MaterializationProcessor::process() {
    auto currentPacket = input->getPacket();

    if (previousSignal.empty()) {
        previousSignal.resize(currentPacket->size());
    }
    if (previousPerceptions.empty()) {
        previousPerceptions.resize(currentPacket->size());
    }

    // calculate signal derivative
    std::vector<float> magnitudeDerivative{};
    for (int index = 0; index < currentPacket->size(); index++) {
        auto currentSample = currentPacket->getSample(index);
        auto previousSample = previousSignal[index];
        auto derivative = currentSample - previousSample;
        if (derivative < 0) {
            derivative = 0;
        }
        magnitudeDerivative.push_back(derivative);
    }

    // record signal
    for (int index = 0; index < currentPacket->size(); index++) {
        previousSignal[index] = currentPacket->getSample(index);
    }

    // calculate perceptions
    std::vector<float> currentPerceptions{};
    for (int index = 0; index < magnitudeDerivative.size(); index++) {
        auto firstPreviousPerceptionIndex = index - contributionRadius;
        if (firstPreviousPerceptionIndex < 0) {
            firstPreviousPerceptionIndex = 0;
        }
        auto lastPreviousPerceptionIndex = index + contributionRadius;
        if (lastPreviousPerceptionIndex >= magnitudeDerivative.size()) {
            lastPreviousPerceptionIndex = magnitudeDerivative.size() - 1;
        }
        auto previousPerceptionSum = 0;
        auto contributionCount = 0;
        for (int jindex = firstPreviousPerceptionIndex; jindex <= lastPreviousPerceptionIndex; jindex++) {
            previousPerceptionSum += previousPerceptions[jindex];
            contributionCount++;
        }
        auto previousPerceptionContribution = previousPerceptionSum / contributionCount / perceptionDecay;
        auto perception = magnitudeDerivative[index] + previousPerceptionContribution;
        currentPerceptions.push_back(perception);
    }

    // record perceptions
    for (int index = 0; index < currentPerceptions.size(); index++) {
        previousPerceptions[index] = currentPerceptions[index];
    }

    // clarify peaks
    std::vector<float> clarifiedPerceptions{};
    clarifiedPerceptions.resize(currentPerceptions.size());
    std::vector<float> previousIterationClarifiedPerceptions{};
    previousIterationClarifiedPerceptions.reserve(currentPerceptions.size());
    for (int index = 0; index < currentPerceptions.size(); index++) {
        previousIterationClarifiedPerceptions.push_back(currentPerceptions[index]);
    }
    for (int iteration = 0; iteration < 2; iteration++) {
        for (int index = 0; index < currentPerceptions.size(); index++) {
            auto firstPerceptionIndex = index - contributionRadius;
            if (firstPerceptionIndex < 0) {
                firstPerceptionIndex = 0;
            }
            auto lastPerceptionIndex = index + contributionRadius;
            if (lastPerceptionIndex >= currentPerceptions.size()) {
                lastPerceptionIndex = currentPerceptions.size() - 1;
            }
            auto result = 0;
            auto contributionCount = 0;
            for (int jindex = firstPerceptionIndex; jindex <= lastPerceptionIndex; jindex++) {
                result += previousIterationClarifiedPerceptions[jindex];
                contributionCount++;
            }
            clarifiedPerceptions[index] = result / contributionCount;
        }
        for (int index = 0; index < currentPerceptions.size(); index++) {
            previousIterationClarifiedPerceptions[index] = clarifiedPerceptions[index];
        }
    }

    // find maxima
    std::vector<float> horizontalDerivative{};
    horizontalDerivative.resize(clarifiedPerceptions.size());
    for (int index = 0; index < clarifiedPerceptions.size(); index++) {
        if (index == 0) {
            horizontalDerivative[index] = 0;
        } else {
            horizontalDerivative[index] = clarifiedPerceptions[index] - clarifiedPerceptions[index - 1];
        }
    }

    std::vector<float> perceptionMaxima{};
    perceptionMaxima.resize(clarifiedPerceptions.size());
    for (int index = 0; index < clarifiedPerceptions.size(); index++) {
        if (index < clarifiedPerceptions.size() - 1) {
            if (horizontalDerivative[index] > 0 && horizontalDerivative[index + 1] <= 0) {
                perceptionMaxima[index] = clarifiedPerceptions[index];
            }
        }
    }

    // send output
    auto outputPacket = std::make_unique<DisplaySignalPacket>(
            currentPacket->getOriginTimestamp(), currentPacket->getFrequencyBand(), currentPacket->size()
    );
    for (int index = 0; index < currentPacket->size(); index++) {
        outputPacket->addSample(perceptionMaxima[index]);
    }
    output->sendPacket(move(outputPacket));
}

}
