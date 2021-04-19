#ifndef CONDUCTOR_SPECTROGRAMPACKET_H
#define CONDUCTOR_SPECTROGRAMPACKET_H

#include <iostream>
#include <vector>
#include <ImpresarioUtils.h>
#include "packet/AudioPacket.h"
#include "packet/Serializable.h"
#include "packet/FileWritable.h"

namespace conductor {

class SpectrogramPacket : public AudioPacket, public Serializable, public FileWritable {
private:
    std::vector<float> magnitudes;
    std::vector<float> spectralFluxes;
    std::vector<float> fluxyFluxes;
    std::vector<float> peaks;
    std::vector<float> firedCatalog;

public:
    SpectrogramPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;

    void writeToFile(std::ofstream &fileStream) const override;

    void addSample(float magnitude, float spectralFlux, float fluxyFlux, float peak, float fired);
};

}

#endif //CONDUCTOR_SPECTROGRAMPACKET_H
