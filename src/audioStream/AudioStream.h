#ifndef CONDUCTOR_AUDIOSTREAM_H
#define CONDUCTOR_AUDIOSTREAM_H

#include <memory>
#include <NonCopyable.h>
#include "packet/RawAudioPacket.h"

namespace conductor {

class AudioStream : impresarioUtils::NonCopyable {
public:
    virtual ~AudioStream() = default;

    virtual bool nextPacketIsReady() const = 0;

    virtual void waitUntilNextPacketIsReady() = 0;

    virtual std::unique_ptr<RawAudioPacket> getNextPacket() = 0;
};

}

#endif //CONDUCTOR_AUDIOSTREAM_H
