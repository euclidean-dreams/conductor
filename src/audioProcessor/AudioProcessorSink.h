#ifndef CONDUCTOR_AUDIOPROCESSORSINK_H
#define CONDUCTOR_AUDIOPROCESSORSINK_H

#include <zmq.hpp>
#include <NetworkSocket.h>
#include <NetworkProxy.h>
#include "audioProcessor/AudioProcessor.h"

namespace conductor {

class AudioProcessorSink : public AudioProcessor {
private:
    std::unique_ptr<impresarioUtils::NetworkProxy> proxy;

public:
    static std::unique_ptr<AudioProcessorSink> create(zmq::context_t &context, const std::string &inputEndpoint,
                                                      const std::string &outputEndpoint);

    explicit AudioProcessorSink(std::unique_ptr<impresarioUtils::NetworkProxy> proxy);

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSINK_H
