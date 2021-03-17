#include "AudioProcessorSuite.h"

namespace conductor {

AudioProcessorSuite::AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream)
        : audioProcessors{} {
    // general
    auto parameterEndpoint = static_cast<std::string>(PARAMETER_ENDPOINT);

    // source
    auto audioStreamSource = std::make_unique<AudioStreamSource>(audioStream, std::make_unique<PacketConduit>());

    // sink
    auto conductorOutputConduit = std::make_shared<PacketConduit>();
    auto sinkInput = std::make_unique<PacketSpout>(*conductorOutputConduit);
    auto sinkOutputEndpoint = static_cast<std::string>(CONDUCTOR_OUTPUT_ENDPOINT);
    auto sinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context, sinkOutputEndpoint,
                                                                       zmq::socket_type::pub,
                                                                       true);
    auto audioProcessorSink = std::make_unique<AudioProcessorSink>(move(sinkInput), move(sinkOutput));

    // onset
    for (auto method : ONSET_PROCESSORS) {
        auto input = std::make_unique<PacketSpout>(audioStreamSource->getOutput());
        auto parameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                                zmq::socket_type::sub, false);
        parameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::onsetProcessorParameters);
        auto processor = std::make_unique<OnsetProcessor>(move(input), conductorOutputConduit, move(parameterSocket),
                                                          method);
        audioProcessors.push_back(move(processor));
    }

    // pitch
    for (auto method : PITCH_PROCESSORS) {
        auto input = std::make_unique<PacketSpout>(audioStreamSource->getOutput());
        auto parameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                                zmq::socket_type::sub, false);
        parameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::pitchProcessorParameters);
        auto processor = std::make_unique<PitchProcessor>(move(input), conductorOutputConduit, move(parameterSocket),
                                                          method);
        audioProcessors.push_back(move(processor));
    }

    // finalization
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
