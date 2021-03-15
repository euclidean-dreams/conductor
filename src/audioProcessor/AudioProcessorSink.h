#ifndef CONDUCTOR_AUDIOPROCESSORSINK_H
#define CONDUCTOR_AUDIOPROCESSORSINK_H

#include <thread>
#include <zmq.hpp>
#include <NetworkSocket.h>
#include <spdlog/spdlog.h>
#include <Time.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketSpout.h"
#include "packet/OnsetPacket.h"

namespace conductor {

class AudioProcessorSink : public AudioProcessor {
private:
    std::unique_ptr<PacketSpout> input;
    std::unique_ptr<impresarioUtils::NetworkSocket> output;
    std::shared_ptr<spdlog::logger> logger;

public:
    explicit AudioProcessorSink(std::unique_ptr<PacketSpout> input,
                                std::unique_ptr<impresarioUtils::NetworkSocket> output);

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSINK_H
