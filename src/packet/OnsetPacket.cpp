#include "OnsetPacket.h"

namespace conductor {

OnsetPacket::OnsetPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                         uint64_t onsetTimestamp, ImpresarioSerialization::OnsetMethod method, float confidence)
        : AudioPacket{originTimestamp, frequencyBand},
          onsetTimestamp{onsetTimestamp},
          method{method},
          confidence{confidence} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> OnsetPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto serializedOnset = ImpresarioSerialization::CreateOnset(*builder, originTimestamp, frequencyBand, method,
                                                                onsetTimestamp, confidence);
    builder->Finish(serializedOnset);
    return builder;
}

ImpresarioSerialization::Identifier OnsetPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::onset;
}

}
