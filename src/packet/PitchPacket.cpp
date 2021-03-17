#include "PitchPacket.h"

namespace conductor {

const PitchPacket &PitchPacket::from(const Packet &packet) {
    return dynamic_cast<const PitchPacket &>(packet);
}

PitchPacket::PitchPacket(ImpresarioSerialization::PitchMethod method, uint8_t pitch, float confidence,
                         uint64_t sampleTimestamp)
        : pitch{pitch},
          confidence{confidence},
          method{method},
          sampleTimestamp{sampleTimestamp} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> PitchPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto serializedPitch = ImpresarioSerialization::CreatePitch(*builder, method, pitch, confidence, sampleTimestamp);
    builder->Finish(serializedPitch);
    return builder;
}

ImpresarioSerialization::Identifier PitchPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::pitch;
}

uint64_t PitchPacket::getSampleTimestamp() const {
    return sampleTimestamp;
}

}
