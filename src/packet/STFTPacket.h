#ifndef CONDUCTOR_STFTPACKET_H
#define CONDUCTOR_STFTPACKET_H

#include <cmath>
#include <vector>
#include "packet/AudioPacket.h"
#include "packet/NotSerializableException.h"

namespace conductor {

class STFTPacket : public AudioPacket {
private:
    std::vector<float> real;
    std::vector<float> imaginary;
    int maxSize;
    int addIndex;
    bool finalized;

    void validateRetrieve(int index) const;

public:
    static const STFTPacket &from(const Packet &packet);

    STFTPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;

    void addSample(float realPart, float imaginaryPart);

    float getRealPart(int index) const;

    float getImaginaryPart(int index) const;

    float getMagnitude(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_STFTPACKET_H
