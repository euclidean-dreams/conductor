#ifndef CONDUCTOR_AUDIOSTREAM_H
#define CONDUCTOR_AUDIOSTREAM_H

#include <memory>
#include <vector>
#include "NonCopyable.h"

using namespace std;

class AudioStream : NonCopyable {
public:
    virtual ~AudioStream() = default;

    virtual int getPacketSize() const = 0;

    virtual bool nextPacketIsReady() const = 0;

    virtual std::unique_ptr<vector<float>> getNextPacket() = 0;
};

#endif //CONDUCTOR_AUDIOSTREAM_H
