#include "HarmonicTransformProcessor.h"

namespace conductor {

HarmonicTransformProcessor::HarmonicTransformProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                                       std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output)
        : input{move(input)},
          output{move(output)},
          correlationSignalSnapshots{},
          squishFactor{2},
          maxPartials{10},
          considerationOffset{3 * static_cast<int>(std::floor(squishFactor))},
          initialPhi{10},
          maxPhiDivisor{2},
          correlationSignalMagnitudeMultiplier{100},
          correlationSignalPeakDecay{0.1} {
    initializeCorrelationSignalSnapshots();
    initializeLowPhiCorrelationSignals();
}

void HarmonicTransformProcessor::initializeCorrelationSignalSnapshots() {
    correlationSignalSnapshots.reserve(maxPartials);
    for (int n = 1; n <= maxPartials; n++) {
        correlationSignalSnapshots.emplace_back();
        auto &result = correlationSignalSnapshots[n - 1];
        result.reserve(2 * considerationOffset + 1);
        for (int x = 0; x < 2 * considerationOffset + 1; x++) {
            auto component = static_cast<double>(x - considerationOffset) / squishFactor;
            auto value = -correlationSignalMagnitudeMultiplier * component / std::pow(n, correlationSignalPeakDecay)
                         * std::exp(-std::pow(component, 2));
            result.push_back(value);
        }
    }
}

void HarmonicTransformProcessor::initializeLowPhiCorrelationSignals() {
    lowPhiCorrelationSignals.reserve(2 * considerationOffset);
    for (int i = 0; i < initialPhi; i++) {
        lowPhiCorrelationSignals.emplace_back();
    }
    for (int phi = initialPhi; phi <= 2 * considerationOffset; phi++) {
        lowPhiCorrelationSignals.emplace_back();
        auto &signal = lowPhiCorrelationSignals[phi];
        auto maxIndex = phi * maxPartials + considerationOffset;
        signal.reserve(maxIndex + 1);
        for (int x = 0; x <= maxIndex; x++) {
            auto result = 0.0;
            for (int n = 1; n <= maxPartials; n++) {
                auto component = static_cast<double>(x - n * phi) / squishFactor;
                result += -component / std::pow(n, correlationSignalPeakDecay) * std::exp(-std::pow(component, 2));
            }
            signal.push_back(result * correlationSignalMagnitudeMultiplier);
        }
    }
}

void HarmonicTransformProcessor::process() {
    auto currentPacket = input->getPacket();
    auto &currentSTFT = *currentPacket;
    auto startTime = impresarioUtils::getCurrentTime();

    // calculate signal derivative
    std::vector<double> signalDerivative;
    signalDerivative.reserve(currentSTFT.size());
    for (int index = 0; index < currentSTFT.size(); index++) {
        if (index == 0) {
            signalDerivative.push_back(0);
        } else {
            signalDerivative.push_back(currentSTFT.getMagnitude(index) - currentSTFT.getMagnitude(index - 1));
        }
    }

    // create output packet
    auto maxPhi = currentSTFT.size() / maxPhiDivisor;
    auto outputPacket = std::make_unique<HarmonicTransformPacket>(
            currentSTFT.getOriginTimestamp(), currentSTFT.getFrequencyBand(), maxPhi, currentPacket
    );

    // populate packet with low phi skipped values
    for (int phi = 0; phi < initialPhi; phi++) {
        outputPacket->addSample(0);
    }

    // calculate harmonic transform
    for (int phi = initialPhi; phi < maxPhi; phi++) {
        auto harmonicIntegral = 0.0;
        if (phi <= 2 * considerationOffset) {
            // consideration windows overlap, cannot optimize
            auto &correlationSignal = lowPhiCorrelationSignals[phi];
            for (int index = 0; index < correlationSignal.size(); index++) {
                harmonicIntegral += correlationSignal[index] * signalDerivative[index];
            }
        } else {
            // no overlap in consideration windows, optimize
            auto lastAllowedPeakIndex = static_cast<double>(signalDerivative.size()) - considerationOffset - 1;
            for (int peakIndex = phi; peakIndex / phi <= maxPartials && peakIndex <= lastAllowedPeakIndex;
                 peakIndex += phi) {
                int firstIndex = peakIndex - considerationOffset;
                int lastIndex = peakIndex + considerationOffset;
                auto &correlationSignal = correlationSignalSnapshots[peakIndex / phi - 1];
                for (int index = firstIndex; index <= lastIndex; index++) {
                    harmonicIntegral += signalDerivative[index] * correlationSignal[index - firstIndex];
                }
            }
        }
        auto harmonicOutput = harmonicIntegral / static_cast<float>(signalDerivative.size());
        outputPacket->addSample(harmonicOutput);
    }
    auto endTime = impresarioUtils::getElapsedTime(startTime);
    std::cout << endTime << std::endl;
    output->sendPacket(move(outputPacket));
}

}
