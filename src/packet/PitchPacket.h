#ifndef CONDUCTOR_PITCHPACKET_H
#define CONDUCTOR_PITCHPACKET_H

#include <cstdint>
#include <Pitch_generated.h>
#include "Config.h"
#include "packet/AudioPacket.h"

namespace conductor {

class PitchPacket : public AudioPacket {
private:
    ImpresarioSerialization::PitchMethod method;
    uint8_t pitch;
    float confidence;

public:
    static const PitchPacket &from(const Packet &packet);

    PitchPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                ImpresarioSerialization::PitchMethod method, uint8_t pitch, float confidence);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;
};

}

#endif //CONDUCTOR_PITCHPACKET_H
