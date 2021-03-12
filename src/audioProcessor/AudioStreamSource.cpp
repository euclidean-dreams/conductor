#include "AudioStreamSource.h"

namespace conductor {

AudioStreamSource::AudioStreamSource(AudioStream &audioStream,
                                     std::unique_ptr<PacketConduit> output)
        : audioStream{audioStream},
          output{move(output)} {
}

void AudioStreamSource::process() {
    if (audioStream.nextPacketIsReady()) {
        output->sendPacket(audioStream.getNextPacket());
    } else {
        std::this_thread::sleep_for(std::chrono::microseconds(AUDIO_STREAM_SOURCE_WAKE_INTERVAL));
    }
}

bool AudioStreamSource::shouldContinue() {
    return true;
}

PacketConduit &AudioStreamSource::getOutput() {
    return *output;
}

}
