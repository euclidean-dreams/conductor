#ifndef CONDUCTOR_RINGBUFFER_H
#define CONDUCTOR_RINGBUFFER_H

#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include "audioStream/AudioStream.h"

namespace conductor {

class RingBuffer : public AudioStream {
private:
    inline static const int BUFFER_SIZE_MULTIPLIER = 16;

    std::mutex mutex;
    std::condition_variable packetAddedExpectant;
    int packetSize;
    int bufferSize;
    std::vector<float> internalBuffer;
    std::vector<float>::const_iterator readIterator;
    std::vector<float>::iterator writeIterator;

public:
    explicit RingBuffer(int packetSize);

    void addSamples(const float *samples, unsigned long count);

    bool nextPacketIsReady() const override;

    void waitUntilNextPacketIsReady() override;

    std::unique_ptr<AudioPacket> getNextPacket() override;
};

}

#endif //PERFORMER_RINGBUFFER_H
