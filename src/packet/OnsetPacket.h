#ifndef CONDUCTOR_ONSETPACKET_H
#define CONDUCTOR_ONSETPACKET_H

#include <cstdint>
#include <ImpresarioSerialization.h>
#include "Config.h"
#include "packet/AudioPacket.h"
#include "packet/Serializable.h"

namespace conductor {

class OnsetPacket : public AudioPacket, public Serializable {
private:
    uint64_t onsetTimestamp;
    ImpresarioSerialization::OnsetMethod method;
    float confidence;

public:
    OnsetPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, uint64_t onsetTimestamp,
                ImpresarioSerialization::OnsetMethod method, float confidence);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;
};

}

#endif //CONDUCTOR_ONSETPACKET_H
