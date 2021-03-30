#include "OnsetPacket.h"

namespace conductor {

const OnsetPacket &OnsetPacket::from(const Packet &packet) {
    return dynamic_cast<const OnsetPacket &>(packet);
}

OnsetPacket::OnsetPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                         uint64_t onsetTimestamp, ImpresarioSerialization::OnsetMethod method, float confidence)
        : AudioPacket{sampleTimestamp, frequencyBand},
          onsetTimestamp{onsetTimestamp},
          method{method},
          confidence{confidence} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> OnsetPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto serializedOnset = ImpresarioSerialization::CreateOnset(*builder, sampleTimestamp, frequencyBand, method,
                                                                onsetTimestamp, confidence);
    builder->Finish(serializedOnset);
    return builder;
}

ImpresarioSerialization::Identifier OnsetPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::onset;
}

}
