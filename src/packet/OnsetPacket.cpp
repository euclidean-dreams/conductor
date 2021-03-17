#include "OnsetPacket.h"

namespace conductor {

const OnsetPacket &OnsetPacket::from(const Packet &packet) {
    return dynamic_cast<const OnsetPacket &>(packet);
}

OnsetPacket::OnsetPacket(uint64_t timestamp, ImpresarioSerialization::OnsetMethod method, uint64_t sampleTimestamp)
        : timestamp{timestamp},
          method{method},
          sampleTimestamp{sampleTimestamp} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> OnsetPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto serializedOnset = ImpresarioSerialization::CreateOnset(*builder, timestamp, method, sampleTimestamp);
    builder->Finish(serializedOnset);
    return builder;
}

ImpresarioSerialization::Identifier OnsetPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::onset;
}

uint64_t OnsetPacket::getSampleTimestamp() const {
    return sampleTimestamp;
}
}
