#include "SpecFluxOnsetProcessor.h"

namespace conductor {

SpecFluxOnsetProcessor::SpecFluxOnsetProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                               std::shared_ptr<PacketDispatcher<Serializable>> output,
                                               std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                                               ImpresarioSerialization::FrequencyBand frequencyBand)
        : input{move(input)},
          output{move(output)},
          parameterSocket{move(parameterSocket)},
          frequencyBand{frequencyBand},
          previousPacket{nullptr},
          spectralFluxCatalog{},
          targetSpectralFluxIterator{},
          threshold{DEFAULT_THRESHOLD},
          peakPickingWindowSize{DEFAULT_PEAK_PICKING_WINDOW_SIZE},
          peakPickingWindowTailMultiplier{DEFAULT_PEAK_PICKING_WINDOW_TAIL_MULTIPLIER} {
    spectralFluxCatalog.resize(expectedSpectralFluxCatalogSize(), 0);
    adjustTargetSpectralFluxIterator();
    spectralFluxTimestamps.resize(expectedSpectralFluxTimestampSize(), 0);
}

int SpecFluxOnsetProcessor::expectedSpectralFluxCatalogSize() const {
    return peakPickingWindowSize + 1 + peakPickingWindowTailMultiplier * peakPickingWindowSize;
}

int SpecFluxOnsetProcessor::expectedSpectralFluxTimestampSize() const {
    return peakPickingWindowSize + 1;
}

void SpecFluxOnsetProcessor::adjustTargetSpectralFluxIterator() {
    targetSpectralFluxIterator = spectralFluxCatalog.cbegin();
    for (int i = 0; i < peakPickingWindowSize; i++) {
        targetSpectralFluxIterator++;
    }
}

void SpecFluxOnsetProcessor::updateAlgorithmParameters() {
    auto parameters = parameterSocket->receiveSerializedData(zmq::recv_flags::dontwait);
    if (parameters != nullptr) {
        auto onsetParameters = ImpresarioSerialization::GetOnsetProcessorParameters(parameters->getBuffer());
        if (onsetParameters->method() == ImpresarioSerialization::OnsetMethod::specflux) {
            threshold = onsetParameters->threshold();
            auto desiredPeakPickingWindowSize = onsetParameters->peakPickingWindowSize();
            if (desiredPeakPickingWindowSize >= 1) {
                peakPickingWindowSize = desiredPeakPickingWindowSize;
                adjustTargetSpectralFluxIterator();
            }
            auto desiredPeakPickingWindowTailMultiplier = onsetParameters->peakPickingWindowTailMultiplier();
            if (desiredPeakPickingWindowTailMultiplier >= 1) {
                peakPickingWindowTailMultiplier = desiredPeakPickingWindowTailMultiplier;
            }
        }
    }
}

void SpecFluxOnsetProcessor::process() {
    updateAlgorithmParameters();
    if (previousPacket == nullptr) {
        previousPacket = input->getPacket();
    }
    auto currentPacket = input->getPacket();
    catalogNextSpectralFlux(*currentPacket);
    if (spectralFluxCatalog.size() == expectedSpectralFluxCatalogSize()
        && spectralFluxTimestamps.size() == expectedSpectralFluxTimestampSize()) {
        if (onsetDetected()) {
            auto onsetTimestamp = spectralFluxTimestamps.back();
            auto onsetConfidence = *targetSpectralFluxIterator;
            auto resultPacket = std::make_unique<OnsetPacket>(onsetTimestamp, frequencyBand, onsetTimestamp,
                                                              ImpresarioSerialization::OnsetMethod::specflux,
                                                              onsetConfidence);
            output->sendPacket(move(resultPacket));
        }
    }
    previousPacket = move(input->getPacket());
}

void SpecFluxOnsetProcessor::catalogNextSpectralFlux(const STFTPacket &currentSTFT) {
    auto spectralFlux = calculateSpectralFlux(currentSTFT, *previousPacket);
    while (spectralFluxCatalog.size() >= expectedSpectralFluxCatalogSize()) {
        spectralFluxCatalog.pop_back();
    }
    spectralFluxCatalog.push_front(spectralFlux);
    targetSpectralFluxIterator--;

    while (spectralFluxTimestamps.size() >= expectedSpectralFluxTimestampSize()) {
        spectralFluxTimestamps.pop_back();
    }
    spectralFluxTimestamps.push_front(currentSTFT.getOriginTimestamp());
}

float SpecFluxOnsetProcessor::calculateSpectralFlux(const STFTPacket &currentSTFT, const STFTPacket &previousSTFT) {
    auto spectralFlux = 0.0f;
    for (int i = 0; i < currentSTFT.size(); i++) {
        auto magnitudeDifference = currentSTFT.getMagnitude(i) - previousSTFT.getMagnitude(i);
        auto rectifiedMagnitudeDifference = halfWaveRectifier(magnitudeDifference);
        spectralFlux += rectifiedMagnitudeDifference;
    }
    return spectralFlux;
}

float SpecFluxOnsetProcessor::halfWaveRectifier(float x) {
    return (x + std::abs(x)) / 2;
}

bool SpecFluxOnsetProcessor::onsetDetected() {
    float maxSurroundingSpectralFlux = 0;
    int index = 0;
    auto spectralFluxCatalogIterator = spectralFluxCatalog.cbegin();
    for (int i = 0; i < peakPickingWindowSize * 2 + 1; i++) {
        if (*spectralFluxCatalogIterator > maxSurroundingSpectralFlux
            && spectralFluxCatalogIterator != targetSpectralFluxIterator) {
            maxSurroundingSpectralFlux = *spectralFluxCatalogIterator;
        }
        spectralFluxCatalogIterator++;
    }
    float spectralFluxSum = 0;
    for (auto currentSpectralFlux : spectralFluxCatalog) {
        spectralFluxSum += currentSpectralFlux;
    }
    auto surroundingSpectralFluxMean = spectralFluxSum / spectralFluxCatalog.size();
    float targetSpectralFlux = *targetSpectralFluxIterator;
    if (targetSpectralFlux >= maxSurroundingSpectralFlux
        && targetSpectralFlux >= surroundingSpectralFluxMean + threshold) {
        return true;
    } else {
        return false;
    }
}

}
