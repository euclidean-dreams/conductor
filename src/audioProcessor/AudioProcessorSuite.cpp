#include "AudioProcessorSuite.h"

namespace conductor {

AudioProcessorSuite::AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream)
        : audioProcessors{} {
    auto audioStreamSource = std::make_unique<AudioStreamSource>(audioStream, std::make_unique<PacketConduit>());

    // sink
    auto outputConduit = std::make_shared<PacketConduit>();
    auto sinkInput = std::make_unique<PacketSpout>(*outputConduit);
    auto sinkOutputEndpoint = static_cast<std::string>(CONDUCTOR_OUTPUT_ENDPOINT);
    auto sinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context, sinkOutputEndpoint,
                                                                       zmq::socket_type::pub,
                                                                       true);
    auto audioProcessorSink = std::make_unique<AudioProcessorSink>(move(sinkInput), move(sinkOutput));

    // onset
    auto parameterEndpoint = static_cast<std::string>(PARAMETER_ENDPOINT);
    for (auto method : ONSET_PROCESSORS) {
        auto onsetInput = std::make_unique<PacketSpout>(audioStreamSource->getOutput());
        auto parameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                                zmq::socket_type::sub, false);
        parameterSocket->setSubscriptionFilter("");
        auto processor = std::make_unique<OnsetProcessor>(move(onsetInput), outputConduit, move(parameterSocket),
                                                          method);
        audioProcessors.push_back(move(processor));
    }
    audioProcessors.push_back(move(audioStreamSource));
    audioProcessors.push_back(move(audioProcessorSink));
}

AudioProcessorSuite::~AudioProcessorSuite() {
    aubio_cleanup();
}

void AudioProcessorSuite::activate() {
    std::vector<std::thread> threads;
    for (auto &audioProcessor: audioProcessors) {
        threads.emplace_back(activateAudioProcessor, std::ref(*audioProcessor));
    }
    for (auto &currentThread: threads) {
        currentThread.join();
    }
}

void AudioProcessorSuite::activateAudioProcessor(AudioProcessor &audioProcessor) {
    audioProcessor.setup();
    while (audioProcessor.shouldContinue()) {
        audioProcessor.process();
    }
}

}
