#include "AudioProcessorSuite.h"

namespace conductor {

AudioProcessorSuite::AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream)
        : audioProcessors{} {
    // general
    auto parameterEndpoint = static_cast<std::string>(PARAMETER_ENDPOINT);

    // source
    auto audioStreamOutput = std::make_unique<PacketConduit>();
    auto &audioStreamOutputRef = *audioStreamOutput;
    auto audioStreamSource = std::make_unique<AudioStreamSource>(audioStream, move(audioStreamOutput));
    audioProcessors.push_back(move(audioStreamSource));

    // sink
    auto suiteOutput = std::make_shared<PacketConduit>();
    auto sinkInput = std::make_unique<PacketSpout>(*suiteOutput);
    auto sinkOutputEndpoint = static_cast<std::string>(CONDUCTOR_OUTPUT_ENDPOINT);
    auto sinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context, sinkOutputEndpoint,
                                                                       zmq::socket_type::pub,
                                                                       true);
    auto audioProcessorSink = std::make_unique<AudioProcessorSink>(move(sinkInput), move(sinkOutput));
    audioProcessors.push_back(move(audioProcessorSink));

    // pipeline
    for (const auto &item : SUITE_CONFIGURATION) {
        auto frequencyBand = item.first;
        auto onsetMethods = std::get<0>(item.second);
        auto pitchMethods = std::get<1>(item.second);

        // bandpass filters
        auto bandpassInput = std::make_unique<PacketSpout>(audioStreamOutputRef);
        auto bandpassOutput = std::make_unique<PacketConduit>();
        auto &bandpassOutputRef = *bandpassOutput;
        auto bandpassProcessor = std::make_unique<BandpassProcessor>(move(bandpassInput), move(bandpassOutput),
                                                                     frequencyBand);
        audioProcessors.push_back(move(bandpassProcessor));

        // file writers
        if (RECORD_AUDIO_TO_FILES) {
            auto audioFileWriterInput = std::make_unique<PacketSpout>(bandpassOutputRef);
            auto audioFileWriter = std::make_unique<AudioFileWriter>(move(audioFileWriterInput), frequencyBand);
            audioProcessors.push_back(move(audioFileWriter));
        }

        // onset processors
        for (auto method : onsetMethods) {
            auto onsetInput = std::make_unique<PacketSpout>(bandpassOutputRef);
            auto onsetParameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                                         zmq::socket_type::sub, false);
            onsetParameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::onsetProcessorParameters);
            auto onsetProcessor = std::make_unique<OnsetProcessor>(move(onsetInput), suiteOutput,
                                                                   move(onsetParameterSocket), method);
            audioProcessors.push_back(move(onsetProcessor));
        }

        // pitch processors
        for (auto method : pitchMethods) {
            auto pitchInput = std::make_unique<PacketSpout>(bandpassOutputRef);
            auto pitchParameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                                         zmq::socket_type::sub, false);
            pitchParameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::pitchProcessorParameters);
            auto pitchProcessor = std::make_unique<PitchProcessor>(move(pitchInput), suiteOutput,
                                                                   move(pitchParameterSocket), method);
            audioProcessors.push_back(move(pitchProcessor));
        }
    }
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
