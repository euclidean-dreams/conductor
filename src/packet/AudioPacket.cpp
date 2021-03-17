#include "AudioPacket.h"

namespace conductor {

const AudioPacket &AudioPacket::from(const Packet &packet) {
    return dynamic_cast<const AudioPacket &>(packet);
}

AudioPacket::AudioPacket(int size)
        : data{},
          maxSize{size},
          addIndex{0},
          finalized{false},
          timestamp{impresarioUtils::getCurrentTime()} {
    data.reserve(maxSize);
}

std::unique_ptr<flatbuffers::FlatBufferBuilder> AudioPacket::serialize() const {
    throw NotSerializableException();
}

ImpresarioSerialization::Identifier AudioPacket::getIdentifier() const {
    throw NotSerializableException();
}

void AudioPacket::addSample(float sample) {
    if (finalized) {
        throw std::runtime_error("attempted to add a sample to a finalized audio packet");
    }
    data[addIndex] = sample;
    addIndex++;
    if (addIndex == maxSize) {
        finalized = true;
    }
}

float AudioPacket::getSample(int index) const {
    if (!finalized) {
        throw std::runtime_error("attempted to use an audio packet that wasn't finalized");
    }
    if (index < 0 || index >= maxSize) {
        throw std::out_of_range("attempted to access an out of range sample");
    }
    return data[index];
}

int AudioPacket::size() const {
    return maxSize;
}

uint64_t AudioPacket::getTimestamp() const {
    return timestamp;
}

}
