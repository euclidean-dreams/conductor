#include "AudioStreamSource.h"

namespace conductor {

AudioStreamSource::AudioStreamSource(AudioStream &audioStream,
                                     std::unique_ptr<PacketConduit> output)
        : audioStream{audioStream},
          output{move(output)} {
}

void AudioStreamSource::process() {
    audioStream.waitUntilNextPacketIsReady();
    output->sendPacket(audioStream.getNextPacket());
}

bool AudioStreamSource::shouldContinue() {
    return true;
}

}
