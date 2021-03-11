#include "AudioStreamSource.h"

namespace conductor {

std::unique_ptr<AudioProcessor> AudioStreamSource::create(zmq::context_t &context, AudioStream &audioStream,
                                                          const std::string &outputEndpoint) {
    auto output = std::make_unique<impresarioUtils::NetworkSocket>(context, outputEndpoint, zmq::socket_type::pub,
                                                                   true);
    return std::make_unique<AudioStreamSource>(audioStream, move(output));
}

AudioStreamSource::AudioStreamSource(AudioStream &audioStream,
                                     std::unique_ptr<impresarioUtils::NetworkSocket> outputSocket)
        : audioStream{audioStream},
          outputSocket{move(outputSocket)} {
}

void AudioStreamSource::process() {
    if (audioStream.nextPacketIsReady()) {
        auto timestamp = impresarioUtils::getCurrentTime();
        flatbuffers::FlatBufferBuilder builder{};
        auto packet = audioStream.getNextPacket();
        auto samplesOffset = builder.CreateVector(*packet);
        auto audioPacket = ImpresarioSerialization::CreateAudioPacket(builder, timestamp, samplesOffset);
        builder.Finish(audioPacket);
        zmq::multipart_t message{builder.GetBufferPointer(), builder.GetSize()};
        auto preSend = impresarioUtils::getElapsedTime(timestamp);
        outputSocket->send(message);
        auto postSend = impresarioUtils::getElapsedTime(timestamp);
        spdlog::get((std::string) LOGGER_NAME)->info("pre send: {}, post send: {}", preSend, postSend);

    } else {
        std::this_thread::sleep_for(std::chrono::microseconds(AUDIO_STREAM_QUERY_INTERVAL));
    }
}

bool AudioStreamSource::shouldContinue() {
    return true;
}

}
