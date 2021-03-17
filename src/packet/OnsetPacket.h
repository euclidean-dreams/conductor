#ifndef CONDUCTOR_ONSETPACKET_H
#define CONDUCTOR_ONSETPACKET_H

#include <cstdint>
#include <Onset_generated.h>
#include "Config.h"
#include "packet/Packet.h"

namespace conductor {

class OnsetPacket : public Packet {
private:
    uint64_t timestamp;
    ImpresarioSerialization::OnsetMethod method;
    uint64_t sampleTimestamp;

public:
    static const OnsetPacket &from(const Packet &packet);

    OnsetPacket(uint64_t timestamp, ImpresarioSerialization::OnsetMethod method, uint64_t sampleTimestamp);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;

    uint64_t getSampleTimestamp() const;
};

}

#endif //CONDUCTOR_ONSETPACKET_H
