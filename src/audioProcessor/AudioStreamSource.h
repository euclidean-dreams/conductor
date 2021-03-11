#ifndef CONDUCTOR_AUDIOMANAGER_H
#define CONDUCTOR_AUDIOMANAGER_H

#include <chrono>
#include <thread>
#include <zmq.hpp>
#include <NetworkSocket.h>
#include <Time.h>
#include <AudioPacket_generated.h>
#include "Config.h"
#include "audioStream/AudioStream.h"
#include "audioProcessor/AudioProcessor.h"


#include <spdlog/spdlog.h>
namespace conductor {

class AudioStreamSource : public AudioProcessor {
private:
    AudioStream &audioStream;
    std::unique_ptr<impresarioUtils::NetworkSocket> outputSocket;

public:
    static std::unique_ptr<AudioProcessor> create(zmq::context_t &context, AudioStream &audioStream,
                                                  const std::string &outputEndpoint);

    AudioStreamSource(AudioStream &audioStream, std::unique_ptr<impresarioUtils::NetworkSocket> outputSocket);

    void setup() override {};

    void process() override;

    bool shouldContinue() override;
};

}

#endif //PERFORMER_AUDIOMANAGER_H
