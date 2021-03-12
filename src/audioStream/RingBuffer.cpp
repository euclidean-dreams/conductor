#include "RingBuffer.h"

namespace conductor {

RingBuffer::RingBuffer(int packetSize, int bufferMultiplier)
        : packetSize{packetSize},
          bufferSize{packetSize * bufferMultiplier},
          internalBuffer(bufferSize),
          readIterator{internalBuffer.cbegin()},
          writeIterator{internalBuffer.begin()} {
}

void RingBuffer::addSamples(const float *samples, unsigned long count) {
    for (int i = 0; i < count; i++) {
        if (writeIterator >= internalBuffer.end()) {
            writeIterator = internalBuffer.begin();
        }
        *writeIterator = samples[i];
        writeIterator++;
    }
}

std::unique_ptr<AudioPacket> RingBuffer::getNextPacket() {
    auto packet = std::make_unique<AudioPacket>();
    for (int i = 0; i < packetSize; i++) {
        packet->addSample(*readIterator);
        readIterator++;
    }
    if (readIterator >= internalBuffer.end()) {
        readIterator = internalBuffer.cbegin();
    }
    return packet;
}

bool RingBuffer::nextPacketIsReady() const {
    if (readIterator + packetSize <= writeIterator
        || (writeIterator < readIterator && writeIterator >= internalBuffer.begin() + packetSize)) {
        return true;
    } else {
        return false;
    }
}

int RingBuffer::getPacketSize() const {
    return packetSize;
}

}
