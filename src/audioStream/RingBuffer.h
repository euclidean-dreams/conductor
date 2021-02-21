#ifndef CONDUCTOR_RINGBUFFER_H
#define CONDUCTOR_RINGBUFFER_H

#include <memory>
#include <vector>
#include "Config.h"
#include "AudioStream.h"

using namespace std;

class RingBuffer : public AudioStream {
private:
    int packetSize;
    int bufferSize;
    vector<float> internalBuffer;
    vector<float>::const_iterator readIterator;
    vector<float>::iterator writeIterator;

public:
    explicit RingBuffer(int packetSize, int bufferMultiplier);

    void addSamples(const float *samples, unsigned long count);

    int getPacketSize() const override;

    bool nextPacketIsReady() const override;

    std::unique_ptr<vector<float>> getNextPacket() override;
};

#endif //PERFORMER_RINGBUFFER_H
