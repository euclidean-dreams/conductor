#include "RingBuffer.h"

namespace conductor {

RingBuffer::RingBuffer(int packetSize, int bufferMultiplier)
        : mutex{},
          packetAddedExpectant{},
          packetSize{packetSize},
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
    if (nextPacketIsReady()) {
        packetAddedExpectant.notify_all();
    }
}

bool RingBuffer::nextPacketIsReady() const {
    if (readIterator + packetSize <= writeIterator
        || (writeIterator < readIterator && writeIterator >= internalBuffer.begin() + packetSize)) {
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<AudioPacket> RingBuffer::getNextPacket() {
    auto packet = std::make_unique<AudioPacket>(packetSize);
    for (int i = 0; i < packetSize; i++) {
        packet->addSample(*readIterator);
        readIterator++;
    }
    if (readIterator >= internalBuffer.end()) {
        readIterator = internalBuffer.cbegin();
    }
    return packet;
}

void RingBuffer::waitUntilNextPacketIsReady() {
    std::unique_lock<std::mutex> lock{mutex};
    while (!nextPacketIsReady()) {
        packetAddedExpectant.wait(lock);
    }
}

}
