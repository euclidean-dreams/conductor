#include "AudioProcessorSink.h"

namespace conductor {

std::unique_ptr<AudioProcessorSink> AudioProcessorSink::create(zmq::context_t &context,
                                                               const std::string &inputEndpoint,
                                                               const std::string &outputEndpoint) {
    auto input = std::make_unique<impresarioUtils::NetworkSocket>(context, inputEndpoint, zmq::socket_type::pull, true);
    auto output = std::make_unique<impresarioUtils::NetworkSocket>(context, outputEndpoint, zmq::socket_type::pub,
                                                                   true);
    auto proxy = std::make_unique<impresarioUtils::NetworkProxy>(move(input), move(output));
    return std::make_unique<AudioProcessorSink>(move(proxy));
}

AudioProcessorSink::AudioProcessorSink(std::unique_ptr<impresarioUtils::NetworkProxy> proxy)
        : proxy{move(proxy)} {

}

void AudioProcessorSink::setup() {

}

void AudioProcessorSink::process() {
    proxy->start();
}

bool AudioProcessorSink::shouldContinue() {
    return true;
}

}
