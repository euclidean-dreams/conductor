#include "AudioFileReader.h"

namespace conductor {

AudioFileReader::AudioFileReader(int packetSize)
        : outputStream{} {
    std::ifstream fileStream;
    fileStream.open(Config::getInstance().getInputAudioFile(), std::ios::in | std::ios::binary | std::ios::ate);
    auto size = static_cast<int>(fileStream.tellg());
    auto readBlock = new char[size];
    fileStream.seekg(0, std::ios::beg);
    fileStream.read(readBlock, size);
    fileStream.close();

    auto floats = reinterpret_cast<float *>(readBlock);
    for (int offset = 0; offset < size / sizeof(float); offset += packetSize) {
        auto packet = std::make_unique<RawAudioPacket>(impresarioUtils::getCurrentTime(),
                                                       ImpresarioSerialization::FrequencyBand::all, packetSize);
        for (int i = 0; i < packetSize; i++) {
            auto sample = floats[offset + i];
            packet->addSample(sample);
        }
        outputStream.addPacket(move(packet));
    }
    delete[] readBlock;
}

AudioStream &AudioFileReader::getAudioStream() {
    return outputStream;
}

}
