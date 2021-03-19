#ifndef CONDUCTOR_RAWAUDIOPACKET_H
#define CONDUCTOR_RAWAUDIOPACKET_H

#include <stdexcept>
#include <vector>
#include "packet/AudioPacket.h"
#include "packet/NotSerializableException.h"

namespace conductor {

class RawAudioPacket : public AudioPacket {
private:
    std::vector<float> data;
    int maxSize;
    int addIndex;
    bool finalized;

public:
    static const RawAudioPacket &from(const Packet &packet);

    RawAudioPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_RAWAUDIOPACKET_H
