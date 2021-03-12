#ifndef CONDUCTOR_ONSETPACKET_H
#define CONDUCTOR_ONSETPACKET_H

#include <cstdint>
#include <Onset_generated.h>
#include "packet/Packet.h"

namespace conductor {

class OnsetPacket : public Packet {
private:
    uint64_t timestamp;
    ImpresarioSerialization::OnsetMethod method;

public:
    static const OnsetPacket &from(const Packet &packet);

    OnsetPacket(uint64_t timestamp, ImpresarioSerialization::OnsetMethod method);

    std::unique_ptr<zmq::multipart_t> serialize() const override;
};

}

#endif //CONDUCTOR_ONSETPACKET_H
