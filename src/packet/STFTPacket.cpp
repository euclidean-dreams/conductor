#include "STFTPacket.h"

namespace conductor {

const STFTPacket &STFTPacket::from(const Packet &packet) {
    return dynamic_cast<const STFTPacket &>(packet);
}

STFTPacket::STFTPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size)
        : AudioPacket{sampleTimestamp, frequencyBand},
          data{},
          maxSize{size},
          addIndex{0},
          finalized{false} {
    data.reserve(maxSize);
}

std::unique_ptr<flatbuffers::FlatBufferBuilder> STFTPacket::serialize() const {
    throw NotSerializableException();
}

ImpresarioSerialization::Identifier STFTPacket::getIdentifier() const {
    throw NotSerializableException();
}

void STFTPacket::addSample(float sample) {
    if (finalized) {
        throw std::runtime_error("attempted to add a sample to a finalized audio packet");
    }
    data[addIndex] = sample;
    addIndex++;
    if (addIndex == maxSize) {
        finalized = true;
    }
}

float STFTPacket::getSample(int index) const {
    if (!finalized) {
        throw std::runtime_error("attempted to use an audio packet that wasn't finalized");
    }
    if (index < 0 || index >= maxSize) {
        throw std::out_of_range("attempted to access an out of range sample");
    }
    return data[index];
}

int STFTPacket::size() const {
    return maxSize;
}
}
