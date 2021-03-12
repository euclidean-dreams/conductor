#ifndef CONDUCTOR_RINGBUFFER_H
#define CONDUCTOR_RINGBUFFER_H

#include <memory>
#include <vector>
#include "audioStream/AudioStream.h"

namespace conductor {

class RingBuffer : public AudioStream {
private:
    int packetSize;
    int bufferSize;
    std::vector<float> internalBuffer;
    std::vector<float>::const_iterator readIterator;
    std::vector<float>::iterator writeIterator;

public:
    explicit RingBuffer(int packetSize, int bufferMultiplier);

    void addSamples(const float *samples, unsigned long count);

    int getPacketSize() const override;

    bool nextPacketIsReady() const override;

    std::unique_ptr<AudioPacket> getNextPacket() override;
};

}

#endif //PERFORMER_RINGBUFFER_H
