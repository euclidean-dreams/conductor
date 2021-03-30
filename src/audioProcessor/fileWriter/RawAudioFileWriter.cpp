#include "RawAudioFileWriter.h"

namespace conductor {

RawAudioFileWriter::RawAudioFileWriter(std::unique_ptr<PacketSpout> input, const std::string &fileName)
        : FileWriter{move(input), fileName} {

}

void RawAudioFileWriter::process() {
    auto packets = input->getPackets(1);
    auto &packet = RawAudioPacket::from(packets->getPacket(0));
    auto outputBuffer = std::make_unique<float[]>(packet.size());
    for (int index = 0; index < packet.size(); index++) {
        outputBuffer[index] = packet.getSample(index);
    }
    fileStream.write(reinterpret_cast<char *>(outputBuffer.get()), packet.size() * sizeof(float));
}

}
