#include "SpectrogramPacket.h"

namespace conductor {

SpectrogramPacket::SpectrogramPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand)
        : AudioPacket{originTimestamp, frequencyBand},
          magnitudes{},
          spectralFluxes{},
          fluxyFluxes{},
          peaks{},
          firedCatalog{} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> SpectrogramPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto magnitudeOffset = builder->CreateVector(magnitudes);
    auto spectralFluxOffset = builder->CreateVector(spectralFluxes);
    auto fluxyFluxOffset = builder->CreateVector(fluxyFluxes);
    auto peakOffset = builder->CreateVector(peaks);
    auto firedOffset = builder->CreateVector(firedCatalog);
    auto serializedPacket = ImpresarioSerialization::CreateSpectrogram(*builder, originTimestamp, magnitudeOffset,
                                                                       spectralFluxOffset, fluxyFluxOffset, peakOffset,
                                                                       firedOffset);
    builder->Finish(serializedPacket);
    return builder;
}

ImpresarioSerialization::Identifier SpectrogramPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::spectrogram;
}

void SpectrogramPacket::addSample(float magnitude, float spectralFlux, float fluxyFlux, float peak, float fired) {
    magnitudes.push_back(magnitude);
    spectralFluxes.push_back(spectralFlux);
    fluxyFluxes.push_back(fluxyFlux);
    peaks.push_back(peak);
    firedCatalog.push_back(fired);
}

}
