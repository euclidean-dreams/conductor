#include "RawAudioPacket.h"

namespace conductor {

RawAudioPacket::RawAudioPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                               int size)
        : AudioPacket{originTimestamp, frequencyBand},
          signal{size} {
}

void RawAudioPacket::writeToFile(std::ofstream &fileStream) const {
    auto size = signal.size();
    auto outputBuffer = std::make_unique<float[]>(size);
    for (int index = 0; index < size; index++) {
        outputBuffer[index] = getSample(index);
    }
    fileStream.write(reinterpret_cast<char *>(outputBuffer.get()), static_cast<std::streamsize>(size * sizeof(float)));
}

void RawAudioPacket::addSample(float sample) {
    signal.addSample(sample);
}

float RawAudioPacket::getSample(int index) const {
    return signal.getSample(index);
}

int RawAudioPacket::size() const {
    return signal.size();
}

}
