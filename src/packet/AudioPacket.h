#ifndef CONDUCTOR_AUDIOPACKET_H
#define CONDUCTOR_AUDIOPACKET_H

#include <FrequencyBand_generated.h>
#include "packet/Packet.h"

namespace conductor {

class AudioPacket : public Packet {
protected:
    uint64_t sampleTimestamp;
    ImpresarioSerialization::FrequencyBand frequencyBand;

public:
    AudioPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand);

    uint64_t getSampleTimestamp() const;

    ImpresarioSerialization::FrequencyBand getFrequencyBand() const;
};

}

#endif //CONDUCTOR_AUDIOPACKET_H
