#ifndef CONDUCTOR_AUDIOPACKET_H
#define CONDUCTOR_AUDIOPACKET_H

#include <ImpresarioSerialization.h>
#include "packet/Packet.h"

namespace conductor {

class AudioPacket : public Packet {
protected:
    uint64_t originTimestamp;

public:
    AudioPacket(uint64_t originTimestamp);

    uint64_t getOriginTimestamp() const;
};

}

#endif //CONDUCTOR_AUDIOPACKET_H
