#ifndef CONDUCTOR_PITCHPACKET_H
#define CONDUCTOR_PITCHPACKET_H

#include <cstdint>
#include <Pitch_generated.h>
#include "Config.h"
#include "packet/Packet.h"

namespace conductor {

class PitchPacket : public Packet {
private:
    ImpresarioSerialization::PitchMethod method;
    uint8_t pitch;
    float confidence;
    uint64_t sampleTimestamp;

public:
    static const PitchPacket &from(const Packet &packet);

    PitchPacket(ImpresarioSerialization::PitchMethod method, uint8_t pitch, float confidence, uint64_t sampleTimestamp);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;

    uint64_t getSampleTimestamp() const;
};

}

#endif //CONDUCTOR_PITCHPACKET_H
