#include "AudioStreamSource.h"

std::unique_ptr<AudioProcessor> AudioStreamSource::create(context_t &context, AudioStream &audioStream,
                                                          const string &outputEndpoint) {
    auto output = make_unique<NetworkSocket>(context, outputEndpoint, socket_type::pub, true);
    return make_unique<AudioStreamSource>(audioStream, move(output));
}

AudioStreamSource::AudioStreamSource(AudioStream &audioStream, std::unique_ptr<NetworkSocket> outputSocket)
        : audioStream{audioStream},
          outputSocket{move(outputSocket)} {
}

void AudioStreamSource::process() {
    if (audioStream.nextPacketIsReady()) {
        auto timestamp = getCurrentTime();
        FlatBufferBuilder builder{AUDIO_PACKET_SIZE};
        auto packet = audioStream.getNextPacket();
        auto samplesOffset = builder.CreateVector(*packet);
        auto audioPacket = CreateAudioPacket(builder, timestamp, samplesOffset);
        builder.Finish(audioPacket);
        multipart_t message{builder.GetBufferPointer(), builder.GetSize()};
        outputSocket->send(message);
    } else {
        sleep_for(microseconds(AUDIO_STREAM_QUERY_INTERVAL));
    }
}

bool AudioStreamSource::shouldContinue() {
    return true;
}
