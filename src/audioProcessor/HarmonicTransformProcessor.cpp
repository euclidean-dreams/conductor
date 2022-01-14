#include "HarmonicTransformProcessor.h"

namespace conductor {

HarmonicTransformProcessor::HarmonicTransformProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                                       std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output)
        : input{move(input)},
          output{move(output)},
          harmonicSignalSnapshots{},
          squishFactor{2.0},
          maxPartials{5},
          considerationOffset{3 * static_cast<int>(std::floor(squishFactor))},
          initialPhi{3},
          maxPhiDivisor{2},
          harmonicSignalMagnitudeMultiplier{100},
          harmonicSignalPeakDecay{0.1},
          lastPacket{} {
    initializeHarmonicSignalSnapshots();
    initializeLowPhiHarmonicSignals();
}

void HarmonicTransformProcessor::initializeHarmonicSignalSnapshots() {
    harmonicSignalSnapshots.reserve(maxPartials);
    for (int n = 1; n <= maxPartials; n++) {
        harmonicSignalSnapshots.emplace_back();
        auto &result = harmonicSignalSnapshots[n - 1];
        result.reserve(2 * considerationOffset + 1);
        for (int x = 0; x < 2 * considerationOffset + 1; x++) {
            auto component = static_cast<double>(x - considerationOffset) / squishFactor;
            auto value = -harmonicSignalMagnitudeMultiplier * component / std::pow(n, harmonicSignalPeakDecay)
                         * std::exp(-std::pow(component, 2));
            result.push_back(value);
        }
    }
}

void HarmonicTransformProcessor::initializeLowPhiHarmonicSignals() {
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
                result += -component / std::pow(n, harmonicSignalPeakDecay) * std::exp(-std::pow(component, 2));
            }
            signal.push_back(result * harmonicSignalMagnitudeMultiplier);
        }
    }
}

void HarmonicTransformProcessor::process() {
    if (lastPacket == nullptr) {
        lastPacket = input->getPacket();
    }

    auto currentPacket = input->getPacket();
    auto &currentSTFT = *currentPacket;
    auto &lastSTFT = *lastPacket;


    // calculate signal derivative
    std::vector<double> signalDerivative;
    signalDerivative.reserve(currentSTFT.size());
    for (int index = 0; index < currentSTFT.size(); index++) {
        if (index == 0) {
            signalDerivative.push_back(0);
        } else {
//            auto horizontalDerivative = currentSTFT.getMagnitude(index) - lastSTFT.getMagnitude(index);
//            auto previousHorizontalDerivative = currentSTFT.getMagnitude(index - 1) - lastSTFT.getMagnitude(index - 1);
//            auto verticalDerivative = horizontalDerivative - previousHorizontalDerivative;
            auto verticalDerivative = currentSTFT.getMagnitude(index) - currentSTFT.getMagnitude(index - 1);
            auto sample = verticalDerivative + (currentSTFT.getMagnitude(index) - lastSTFT.getMagnitude(index));
            signalDerivative.push_back(verticalDerivative);
        }
    }

    // create output packet
    auto maxPhi = currentSTFT.size() / maxPhiDivisor;
    auto outputPacket = std::make_unique<HarmonicTransformPacket>(
            currentSTFT.getOriginTimestamp(), maxPhi, currentPacket
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
            auto lastIndex = phi * maxPartials + considerationOffset;
            for (int index = 0; index <= lastIndex; index++) {
                harmonicIntegral += correlationSignal[index] * signalDerivative[index];
            }
        } else {
            // no overlap in consideration windows, optimize
            auto lastAllowedPeakIndex = static_cast<double>(signalDerivative.size()) - considerationOffset - 1;
            for (int peakIndex = phi; peakIndex / phi <= maxPartials && peakIndex <= lastAllowedPeakIndex;
                 peakIndex += phi) {
                int firstIndex = peakIndex - considerationOffset;
                int lastIndex = peakIndex + considerationOffset;
                auto &correlationSignal = harmonicSignalSnapshots[peakIndex / phi - 1];
                for (int index = firstIndex; index <= lastIndex; index++) {
                    harmonicIntegral += signalDerivative[index] * correlationSignal[index - firstIndex];
                }
            }
        }
        auto harmonicOutput = harmonicIntegral / static_cast<float>(signalDerivative.size());
        outputPacket->addSample(harmonicOutput);
    }
    output->sendPacket(move(outputPacket));
    lastPacket = move(currentPacket);
}

}
