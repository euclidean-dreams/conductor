#include "RingBuffer.h"

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

std::unique_ptr<vector<float>> RingBuffer::getNextPacket() {
    auto packet = make_unique<vector<float>>(readIterator, readIterator + packetSize);
    readIterator += packetSize;
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
