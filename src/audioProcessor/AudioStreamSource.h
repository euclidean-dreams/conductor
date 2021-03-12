#ifndef CONDUCTOR_AUDIOMANAGER_H
#define CONDUCTOR_AUDIOMANAGER_H

#include <thread>
#include <Time.h>
#include "Config.h"
#include "packet/PacketPublisher.h"
#include "audioStream/AudioStream.h"
#include "audioProcessor/AudioProcessor.h"

namespace conductor {

class AudioStreamSource : public AudioProcessor {
private:
    AudioStream &audioStream;
    std::unique_ptr<PacketPublisher> output;

public:
    AudioStreamSource(AudioStream &audioStream, std::unique_ptr<PacketPublisher> output);

    void setup() override {};

    void process() override;

    bool shouldContinue() override;
};

}

#endif //PERFORMER_AUDIOMANAGER_H
