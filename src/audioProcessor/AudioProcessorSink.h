#ifndef CONDUCTOR_AUDIOPROCESSORSINK_H
#define CONDUCTOR_AUDIOPROCESSORSINK_H

#include <thread>
#include <zmq.hpp>
#include <NetworkSocket.h>
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketSpout.h"

namespace conductor {

class AudioProcessorSink : public AudioProcessor {
private:
    std::unique_ptr<PacketSpout> input;
    std::unique_ptr<impresarioUtils::NetworkSocket> output;

public:
    explicit AudioProcessorSink(std::unique_ptr<PacketSpout> input,
                                std::unique_ptr<impresarioUtils::NetworkSocket> output);

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSINK_H
