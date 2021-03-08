#include "AudioProcessorSink.h"

std::unique_ptr<AudioProcessorSink> AudioProcessorSink::create(context_t &context, const string &inputEndpoint,
                                                               const string &outputEndpoint) {
    auto input = make_unique<NetworkSocket>(context, inputEndpoint, socket_type::pull, true);
    auto output = make_unique<NetworkSocket>(context, outputEndpoint, socket_type::pub, true);
    auto proxy = make_unique<NetworkProxy>(move(input), move(output));
    return make_unique<AudioProcessorSink>(move(proxy));
}

AudioProcessorSink::AudioProcessorSink(std::unique_ptr<NetworkProxy> proxy)
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
