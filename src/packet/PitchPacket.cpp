#include "PitchPacket.h"

namespace conductor {

const PitchPacket &PitchPacket::from(const Packet &packet) {
    return dynamic_cast<const PitchPacket &>(packet);
}

PitchPacket::PitchPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                         ImpresarioSerialization::PitchMethod method, uint8_t pitch, float confidence)
        : AudioPacket{sampleTimestamp, frequencyBand},
          method{method},
          pitch{pitch},
          confidence{confidence} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> PitchPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto serializedPitch = ImpresarioSerialization::CreatePitch(*builder, sampleTimestamp, frequencyBand, method, pitch,
                                                                confidence);
    builder->Finish(serializedPitch);
    return builder;
}

ImpresarioSerialization::Identifier PitchPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::pitch;
}

}
