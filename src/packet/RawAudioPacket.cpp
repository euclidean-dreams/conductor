#include "RawAudioPacket.h"

namespace conductor {

RawAudioPacket::RawAudioPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                               int size)
        : AudioPacket{originTimestamp, frequencyBand},
          data{},
          maxSize{size},
          addIndex{0},
          finalized{false} {
    data.reserve(maxSize);
}

void RawAudioPacket::addSample(float sample) {
    if (finalized) {
        throw std::runtime_error("attempted to add a sample to a finalized audio packet");
    }
    data[addIndex] = sample;
    addIndex++;
    if (addIndex == maxSize) {
        finalized = true;
    }
}

float RawAudioPacket::getSample(int index) const {
    if (!finalized) {
        throw std::runtime_error("attempted to use an audio packet that wasn't finalized");
    }
    if (index < 0 || index >= maxSize) {
        throw std::out_of_range("attempted to access an out of range sample");
    }
    return data[index];
}

int RawAudioPacket::size() const {
    return maxSize;
}

void RawAudioPacket::writeToFile(std::ofstream &fileStream) const {
    auto outputBuffer = std::make_unique<float[]>(maxSize);
    for (int index = 0; index < maxSize; index++) {
        outputBuffer[index] = getSample(index);
    }
    fileStream.write(reinterpret_cast<char *>(outputBuffer.get()), maxSize * sizeof(float));
}

}
