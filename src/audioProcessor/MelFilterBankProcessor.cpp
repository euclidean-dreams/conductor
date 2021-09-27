#include "MelFilterBankProcessor.h"

namespace conductor {

MelFilterBankProcessor::MelFilterBankProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                                               std::unique_ptr<PacketDispatcher<MelSignalPacket>> output,
                                               int filterCount)
        : input{move(input)},
          output{move(output)},
          filterCount{filterCount},
          filterBankCenters{},
          maxFrequency{Config::getInstance().getMelFilterBankMaxFrequency()},
          sampleRate{Config::getInstance().getSampleRate()} {

}

void MelFilterBankProcessor::calculateFilterBankCenters(int fftSize) {
    auto maxMelFrequency = convertHzToMel(maxFrequency);
    auto increment = maxMelFrequency / static_cast<double>(filterCount + 1);
    filterBankCenters.reserve(filterCount);
    for (int index = 1; index <= filterCount; index++) {
        auto melCenter = increment * static_cast<double>(index);
        auto hzCenter = convertMelToHz(melCenter);
        auto binCenter = convertHzToBin(hzCenter, fftSize, sampleRate);
        filterBankCenters.push_back(binCenter);
    }
}

double MelFilterBankProcessor::convertHzToMel(double value) {
    return 1127 * std::log(1 + value / 700);
}

double MelFilterBankProcessor::convertMelToHz(double value) {
    return 700 * (std::exp(value / 1127) - 1);
}

int MelFilterBankProcessor::convertHzToBin(double value, int fftSize, double sampleRate) {
    return static_cast<int>(std::floor(value * static_cast<double>(fftSize) / sampleRate));
}

void MelFilterBankProcessor::process() {
    auto packet = input->getPacket();
    auto &harmonicTransform = *packet;
    auto &stft = harmonicTransform.getSTFTPacket();
    if (filterBankCenters.empty()) {
        calculateFilterBankCenters(stft.getFFTSize());
    }

    auto outputPacket = std::make_unique<MelSignalPacket>(
            harmonicTransform.getOriginTimestamp(), harmonicTransform.getFrequencyBand(), filterCount, packet
    );

    for (int index = 0; index < filterBankCenters.size(); index++) {
        auto result = 0.0;
        int firstBinToConsider;
        if (index == 0) {
            firstBinToConsider = 0;
        } else {
            firstBinToConsider = filterBankCenters[index - 1];
        }
        auto centerBin = filterBankCenters[index];
        auto positiveBinSlope = 1 / static_cast<double>(centerBin - firstBinToConsider);
        for (int bin = firstBinToConsider + 1; bin < centerBin; bin++) {
            auto factor = positiveBinSlope * static_cast<double>(bin - firstBinToConsider);
            auto value = harmonicTransform.getSample(bin) * stft.getMagnitude(bin) * factor;
            result += value;
        }

        int lastBinToConsider;
        if (index == filterBankCenters.size() - 1) {
            lastBinToConsider = harmonicTransform.size();
        } else {
            lastBinToConsider = filterBankCenters[index + 1];
        }
        auto negativeBinSlope = 1 / static_cast<double>(lastBinToConsider - centerBin);
        for (int bin = centerBin; bin < lastBinToConsider; bin++) {
            auto factor = 1 - negativeBinSlope * static_cast<double>(bin - centerBin);
            auto value = harmonicTransform.getSample(bin) * stft.getMagnitude(bin) * factor;
            result += value;
        }
        outputPacket->addSample(result);
    }
    output->sendPacket(move(outputPacket));
}

}
