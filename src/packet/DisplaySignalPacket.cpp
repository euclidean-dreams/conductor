#include "DisplaySignalPacket.h"

namespace conductor {

DisplaySignalPacket::DisplaySignalPacket(uint64_t originTimestamp, int size,
                                         std::shared_ptr<const MelSignalPacket> melSignalPacket,
                                         std::vector<float> lagFlux)
        : AudioPacket{originTimestamp},
          signal{size},
          melSignalPacket{move(melSignalPacket)},
          lagFlux{move(lagFlux)} {

}

std::unique_ptr<flatbuffers::FlatBufferBuilder> DisplaySignalPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    std::vector<float> melSignal;
    for (int index = 0; index < melSignalPacket->size(); index++) {
        melSignal.push_back(melSignalPacket->getSample(index));
    }
    auto mels = builder->CreateVector(melSignal);
    auto radixes = builder->CreateVector(signal.getVectorReference());
    auto lagFluxOffset = builder->CreateVector(lagFlux);
    auto serializedPacket = ImpresarioSerialization::CreateEssentia(*builder, mels, radixes, lagFluxOffset);
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
