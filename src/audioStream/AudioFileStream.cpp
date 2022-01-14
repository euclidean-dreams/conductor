#include <iostream>
#include "AudioFileStream.h"

namespace conductor {

AudioFileStream::AudioFileStream()
        : index{0},
          packets{} {

}

void AudioFileStream::addPacket(std::unique_ptr<RawAudioPacket> packet) {
    packets.push_back(move(packet));
}

bool AudioFileStream::nextPacketIsReady() const {
    if (index < packets.size()) {
        return true;
    } else {
        return false;
    }
}

void AudioFileStream::waitUntilNextPacketIsReady() {
    if (index >= packets.size()) {
        std::cout << "finished reading file" << std::endl;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10000));
        }
    }
}

std::unique_ptr<RawAudioPacket> AudioFileStream::getNextPacket() {
    auto packet = move(packets[index]);
    index++;
    return packet;
}

}
