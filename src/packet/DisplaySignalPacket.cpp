#include "DisplaySignalPacket.h"

namespace conductor {

DisplaySignalPacket::DisplaySignalPacket(uint64_t originTimestamp, int size)
        : AudioPacket{originTimestamp},
          signal{size} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> DisplaySignalPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    std::vector<float> rawStft;
    auto stft = builder->CreateVector(rawStft);
    auto radixes = builder->CreateVector(signal.getVectorReference());
    auto serializedPacket = ImpresarioSerialization::CreateEssentia(*builder, stft, radixes);
    builder->Finish(serializedPacket);
    return builder;
}

ImpresarioSerialization::Identifier DisplaySignalPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::essentia;
}

void DisplaySignalPacket::addSample(float sample) {
    signal.addSample(sample);
}

float DisplaySignalPacket::getSample(int index) const {
    return signal.getSample(index);
}

int DisplaySignalPacket::size() const {
    return signal.size();
}

}
