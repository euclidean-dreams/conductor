#ifndef CONDUCTOR_AUDIOMANAGER_H
#define CONDUCTOR_AUDIOMANAGER_H

#include <thread>
#include <ImpresarioUtils.h>
#include "packet/PacketDispatcher.h"
#include "packet/RawAudioPacket.h"
#include "audioStream/AudioStream.h"
#include "audioProcessor/AudioProcessor.h"

namespace conductor {

class AudioStreamSource : public AudioProcessor {
private:
    AudioStream &audioStream;
    std::unique_ptr<PacketDispatcher<RawAudioPacket>> output;

public:
    AudioStreamSource(AudioStream &audioStream, std::unique_ptr<PacketDispatcher<RawAudioPacket>> output);

    void process() override;
};

}

#endif //PERFORMER_AUDIOMANAGER_H
