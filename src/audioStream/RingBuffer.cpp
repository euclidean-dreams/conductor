#include "RingBuffer.h"

namespace conductor {

RingBuffer::RingBuffer(int packetSize)
        : mutex{},
          packetAddedExpectant{},
          packetSize{packetSize},
          bufferSize{packetSize * BUFFER_SIZE_MULTIPLIER},
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

std::unique_ptr<RawAudioPacket> RingBuffer::getNextPacket() {
    auto packet = std::make_unique<RawAudioPacket>(impresarioUtils::getCurrentTime(),
                                                   ImpresarioSerialization::FrequencyBand::all, packetSize);
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
