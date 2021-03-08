#ifndef CONDUCTOR_AUDIOPROCESSORSINK_H
#define CONDUCTOR_AUDIOPROCESSORSINK_H

#include <zmq.hpp>
#include "NetworkSocket.h"
#include "NetworkProxy.h"
#include "audioProcessor/AudioProcessor.h"

using namespace zmq;

class AudioProcessorSink : public AudioProcessor {
private:
    std::unique_ptr<NetworkProxy> proxy;

public:
    static std::unique_ptr<AudioProcessorSink> create(context_t &context, const string &inputEndpoint,
                                                      const string &outputEndpoint);

    explicit AudioProcessorSink(std::unique_ptr<NetworkProxy> proxy);

    void setup() override;

    void process() override;

    bool shouldContinue() override;

};

#endif //CONDUCTOR_AUDIOPROCESSORSINK_H
