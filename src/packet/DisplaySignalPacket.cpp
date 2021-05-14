#include "DisplaySignalPacket.h"

namespace conductor {

DisplaySignalPacket::DisplaySignalPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                                         int size)
        : AudioPacket{originTimestamp, frequencyBand},
          signal{size} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> DisplaySignalPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    auto samples = builder->CreateVector(signal.getVectorReference());
    auto serializedPacket = ImpresarioSerialization::CreateDisplaySignal(*builder, originTimestamp, samples);
    builder->Finish(serializedPacket);
    return builder;
}

ImpresarioSerialization::Identifier DisplaySignalPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::displaySignal;
}

void DisplaySignalPacket::addSample(float sample) {
    signal.addSample(sample);
}

}
