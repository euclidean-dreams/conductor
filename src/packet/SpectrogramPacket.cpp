#include "SpectrogramPacket.h"

namespace conductor {

SpectrogramPacket::SpectrogramPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                                     int layerCount)
        : AudioPacket{originTimestamp, frequencyBand},
          layers{} {
    layers.resize(layerCount);
}

void SpectrogramPacket::writeToFile(std::ofstream &fileStream) const {
    auto layerCount = static_cast<int>(layers.size());
    fileStream.write(reinterpret_cast<char *>(&layerCount), sizeof(int));
    auto bufferSize = static_cast<int>(layers[0].size());
    fileStream.write(reinterpret_cast<char *>(&bufferSize), sizeof(int));
    auto outputBuffer = std::make_unique<float[]>(bufferSize);
    for (auto &layer: layers) {
        for (int index = 0; index < bufferSize; index++) {
            outputBuffer[index] = static_cast<float>(layer[index]);
        }
        fileStream.write(
                reinterpret_cast<char *>(outputBuffer.get()), static_cast<std::streamsize >(bufferSize * sizeof(float))
        );
    }
}

void SpectrogramPacket::addData(std::vector<double> &data) {
    for (int index = 0; index < layers.size(); index++) {
        layers[index].push_back(data[index]);
    }
}

}
