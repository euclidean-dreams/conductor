#include "CuriousPerceptron.h"

namespace conductor {

CuriousPerceptron::CuriousPerceptron()
        : memoryLength{DEFAULT_MEMORY_LENGTH},
          magnitudes{},
          spectralFluxes{},
          fluxyFluxes{},
          threshold{DEFAULT_THRESHOLD} {
    lastActivationSpectralPeak = spectralFluxes.cend();
    lastActivationMagnitudePeak = magnitudes.cend();
}

void CuriousPerceptron::addSample(float magnitude) {
    if (magnitudes.empty()) {
        magnitudes.push_front(magnitude);
        spectralFluxes.push_front(0);
        fluxyFluxes.push_front(0);
    } else {
        while (magnitudes.size() >= memoryLength) {
            if (lastActivationMagnitudePeak == --magnitudes.cend()) {
                lastActivationMagnitudePeak = magnitudes.cend();
            }
            magnitudes.pop_back();
            if (lastActivationSpectralPeak == --spectralFluxes.cend()) {
                lastActivationSpectralPeak = spectralFluxes.cend();
            }
            spectralFluxes.pop_back();
            fluxyFluxes.pop_back();
        }
        auto spectralFlux = magnitude - magnitudes.front();
        auto fluxyFlux = spectralFlux - spectralFluxes.front();
        magnitudes.push_front(magnitude);
        spectralFluxes.push_front(spectralFlux);
        fluxyFluxes.push_front(fluxyFlux);
    }
}

float CuriousPerceptron::getPeaks() {
    if (peakOccurring()) {
        auto lastActivationValley = findLastActivationValley();
        auto onset = findOnset();
        auto doldrumSize = 0;
        auto doldrumSum = 0.0f;
        auto onsetCopy{onset};
        while (onsetCopy != lastActivationValley) {
            doldrumSum += *onset;
            onsetCopy++;
            doldrumSize++;
        }
        auto doldrumMean = doldrumSum / static_cast<float>(doldrumSize);
        auto deviationSum = 0.0f;
        while (onset != lastActivationValley) {
            deviationSum += static_cast<float>(std::pow(*onset - doldrumMean, 2));
            onset++;
        }
        auto standardDeviation = std::sqrt(deviationSum / (static_cast<float>(doldrumSize) - 1));
        return standardDeviation;
    }
    return 0;
}

bool CuriousPerceptron::fired() {
    if (peakOccurring()) {
//        auto onset = findOnset();
//        auto totalMagnitudeChange = magnitudes.front() - *onset;
//        if (totalMagnitudeChange > threshold) {
//            lastActivationMagnitudePeak = magnitudes.cbegin();
//            lastActivationSpectralPeak = spectralFluxes.cbegin();
//            return true;
//        }

        auto lastActivationValley = findLastActivationValley();
        auto onset = findOnset();
        auto doldrumSize = 0;
        auto doldrumSum = 0.0f;
        auto onsetCopy{onset};
        while (onsetCopy != lastActivationValley) {
            doldrumSum += *onset;
            onsetCopy++;
            doldrumSize++;
        }
        auto doldrumMean = doldrumSum / static_cast<float>(doldrumSize);
        auto deviationSum = 0.0f;
        while (onset != lastActivationValley) {
            deviationSum += static_cast<float>(std::pow(*onset - doldrumMean, 2));
            onset++;
        }
        auto standardDeviation = std::sqrt(deviationSum / (static_cast<float>(doldrumSize) - 1));
        if (magnitudes.front() > doldrumMean + standardDeviation * threshold + 5) {
            lastActivationMagnitudePeak = magnitudes.cbegin();
            lastActivationSpectralPeak = spectralFluxes.cbegin();
            return true;
        }
    }
    return false;
}

bool CuriousPerceptron::peakOccurring() const {
    if (spectralFluxes.front() <= 0 && *(++spectralFluxes.cbegin()) > 0 && fluxyFluxes.front() < 0) {
        return true;
    }
    return false;
}

std::_List_const_iterator<float> CuriousPerceptron::findLastActivationValley() const {
    if (lastActivationMagnitudePeak == magnitudes.cend()) {
        return magnitudes.cend();
    } else {
        auto lastActivationSpectralValley{lastActivationSpectralPeak};
        auto lastActivationMagnitudeValley{lastActivationMagnitudePeak};
        while (*lastActivationSpectralValley < 0) {
            lastActivationSpectralValley--;
            lastActivationMagnitudeValley--;
        }
        return lastActivationMagnitudeValley;
    }
}

std::_List_const_iterator<float> CuriousPerceptron::findOnset() const {
    auto spectralOnset = spectralFluxes.cbegin();
    auto onset = magnitudes.cbegin();
    spectralOnset++;
    while (*spectralOnset > 0) {
        spectralOnset++;
        onset++;
    }
    return onset;
}

}

