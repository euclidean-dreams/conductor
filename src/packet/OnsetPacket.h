#ifndef CONDUCTOR_ONSETPACKET_H
#define CONDUCTOR_ONSETPACKET_H

#include <cstdint>
#include <Onset_generated.h>
#include "Config.h"
#include "packet/AudioPacket.h"

namespace conductor {

class OnsetPacket : public AudioPacket {
private:
    uint64_t onsetTimestamp;
    ImpresarioSerialization::OnsetMethod method;
    float confidence;

public:
    static const OnsetPacket &from(const Packet &packet);

    OnsetPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, uint64_t onsetTimestamp,
                ImpresarioSerialization::OnsetMethod method, float confidence);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;
};

}

#endif //CONDUCTOR_ONSETPACKET_H
