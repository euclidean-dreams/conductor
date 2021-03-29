#include "AudioFileWriter.h"

namespace conductor {

AudioFileWriter::AudioFileWriter(std::unique_ptr<PacketSpout> input,
                                 ImpresarioSerialization::FrequencyBand frequencyBand)
        : input{move(input)},
          frequencyBand{frequencyBand},
          fileStream{} {
    std::ostringstream fileName;
    fileName << FILE_PATH << ImpresarioSerialization::EnumNameFrequencyBand(frequencyBand) << ".raw";
    auto path = fileName.str();
    fileStream.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
}

AudioFileWriter::~AudioFileWriter() {
    fileStream.close();
}

void AudioFileWriter::process() {
    auto packets = input->getPackets(1);
    auto &packet = RawAudioPacket::from(packets->getPacket(0));
    auto outputBuffer = std::make_unique<float[]>(packet.size());
    for (int index = 0; index < packet.size(); index++) {
        outputBuffer[index] = packet.getSample(index);
    }
    fileStream.write(reinterpret_cast<char *>(outputBuffer.get()), packet.size() * sizeof(float));
}

bool AudioFileWriter::shouldContinue() {
    return true;
}

}
