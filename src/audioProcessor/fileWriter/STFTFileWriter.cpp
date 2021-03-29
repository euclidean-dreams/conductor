#include "STFTFileWriter.h"

namespace conductor {

STFTFileWriter::STFTFileWriter(std::unique_ptr<PacketSpout> input, const std::string &fileName)
        : FileWriter(move(input), fileName) {

}

void STFTFileWriter::process() {
    auto packets = input->getPackets(1);
    auto &packet = STFTPacket::from(packets->getPacket(0));
    auto outputBuffer = std::make_unique<float[]>(packet.size());
    for (int index = 0; index < packet.size(); index++) {
        outputBuffer[index] = packet.getMagnitude(index);
    }
    fileStream.write(reinterpret_cast<char *>(outputBuffer.get()), packet.size() * sizeof(float));
}

}
