#include "AudioStreamSource.h"

namespace conductor {

AudioStreamSource::AudioStreamSource(AudioStream &audioStream,
                                     std::unique_ptr<PacketPublisher> output)
        : audioStream{audioStream},
          output{move(output)} {
}

void AudioStreamSource::process() {
    if (audioStream.nextPacketIsReady()) {
        output->addPacket(audioStream.getNextPacket());
    } else {
        std::this_thread::sleep_for(std::chrono::microseconds(AUDIO_STREAM_QUERY_INTERVAL));
    }
}

bool AudioStreamSource::shouldContinue() {
    return true;
}

}
