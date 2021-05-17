#include "MelFilterbankProcessor.h"

namespace conductor {

MelFilterbankProcessor::MelFilterbankProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                                               std::unique_ptr<PacketDispatcher<MelSignalPacket>> output)
        : input{move(input)},
          output{move(output)},
          filterbankCenters{},
          filterbankCount{FILTERBANK_COUNT},
          filterbankMaxFrequencyDenominator{FILTERBANK_MAX_FREQUENCY_DENOMINATOR},
          sampleRate{Config::getInstance().getSampleRate()} {

}

void MelFilterbankProcessor::calculateFilterbankCenters(int fftSize) {
    auto maxMelFrequency = convertHzToMel(sampleRate / filterbankMaxFrequencyDenominator);
    auto increment = maxMelFrequency / static_cast<double>(filterbankCount + 1);
    filterbankCenters.reserve(filterbankCount);
    for (int index = 1; index <= filterbankCount; index++) {
        auto melCenter = increment * static_cast<double>(index);
        auto hzCenter = convertMelToHz(melCenter);
        auto binCenter = convertHzToBin(hzCenter, fftSize, sampleRate);
        filterbankCenters.push_back(binCenter);
    }
}

double MelFilterbankProcessor::convertHzToMel(double value) {
    return 1127 * std::log(1 + value / 700);
}

double MelFilterbankProcessor::convertMelToHz(double value) {
    return 700 * (std::exp(value / 1127) - 1);
}

int MelFilterbankProcessor::convertHzToBin(double value, int fftSize, double sampleRate) {
    return static_cast<int>(std::floor(value * static_cast<double>(fftSize) / sampleRate));
}

void MelFilterbankProcessor::process() {
    auto packet = input->getPacket();
    auto &harmonicTransform = *packet;
    auto &stft = harmonicTransform.getSTFTPacket();
    if (filterbankCenters.empty()) {
        calculateFilterbankCenters(stft.getFFTSize());
    }

    auto outputPacket = std::make_unique<MelSignalPacket>(
            harmonicTransform.getOriginTimestamp(), harmonicTransform.getFrequencyBand(), filterbankCount, packet
    );

    for (int index = 0; index < filterbankCenters.size(); index++) {
        auto result = 0.0;
        int firstBinToConsider;
        if (index == 0) {
            firstBinToConsider = 0;
        } else {
            firstBinToConsider = filterbankCenters[index - 1];
        }
        auto centerBin = filterbankCenters[index];
        auto positiveBinSlope = 1 / static_cast<double>(centerBin - firstBinToConsider);
        for (int bin = firstBinToConsider + 1; bin < centerBin; bin++) {
            auto factor = positiveBinSlope * static_cast<double>(bin - firstBinToConsider);
            auto value = harmonicTransform.getSample(bin) * stft.getMagnitude(bin) * factor;
            result += value;
        }

        int lastBinToConsider;
        if (index == filterbankCenters.size() - 1) {
            lastBinToConsider = harmonicTransform.size();
        } else {
            lastBinToConsider = filterbankCenters[index + 1];
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
