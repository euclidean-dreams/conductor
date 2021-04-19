#ifndef CONDUCTOR_AUDIOFILESTREAM_H
#define CONDUCTOR_AUDIOFILESTREAM_H

#include <thread>
#include <iostream>
#include <ImpresarioUtils.h>
#include "audioStream/AudioStream.h"
#include "packet/RawAudioPacket.h"

namespace conductor {

class AudioFileStream : public AudioStream {
private:
    int index;
    std::vector<std::unique_ptr<RawAudioPacket>> packets;

public:
    AudioFileStream();

    void addPacket(std::unique_ptr<RawAudioPacket> packet);

    bool nextPacketIsReady() const override;

    void waitUntilNextPacketIsReady() override;

    std::unique_ptr<RawAudioPacket> getNextPacket() override;
};

}

#endif //CONDUCTOR_AUDIOFILESTREAM_H
