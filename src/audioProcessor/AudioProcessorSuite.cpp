#include "AudioProcessorSuite.h"

namespace conductor {

AudioProcessorSuite::AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream)
        : audioProcessors{} {
    // general
    auto parameterEndpoint = static_cast<std::string>(PARAMETER_ENDPOINT);
    packetConduitCurator = std::make_unique<PacketConduitCurator>();

    // source
    auto audioStreamOutputConduit = std::make_unique<PacketConduit<RawAudioPacket>>();
    auto audioStreamOutput = std::make_unique<PacketDispatcher<RawAudioPacket>>(*audioStreamOutputConduit);
    auto audioStreamSource = std::make_unique<AudioStreamSource>(audioStream, move(audioStreamOutput));
    audioProcessors.push_back(move(audioStreamSource));

    // sink
    auto sinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
    auto sinkInput = std::make_unique<PacketReceiver<Serializable>>(*sinkInputConduit);
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

        // bandpass filter
        auto bandpassInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*audioStreamOutputConduit);
        auto bandpassOutputConduit = std::make_unique<PacketConduit<RawAudioPacket>>();
        auto bandpassOutput = std::make_unique<PacketDispatcher<RawAudioPacket>>(*bandpassOutputConduit);
        auto bandpassProcessor = std::make_unique<BandpassProcessor>(move(bandpassInput), move(bandpassOutput),
                                                                     frequencyBand);
        audioProcessors.push_back(move(bandpassProcessor));

        // audio file writer
        if (RECORD_TO_FILES) {
            auto fileWriterInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*bandpassOutputConduit);
            auto relativePath = std::string{"raw_audio/"};
            relativePath.append(ImpresarioSerialization::EnumNameFrequencyBand(frequencyBand));
            relativePath.append(".wav");
            auto fileWriter = std::make_unique<FileWriter<RawAudioPacket>>(move(fileWriterInput), relativePath);
            audioProcessors.push_back(move(fileWriter));
        }

        // stft
        auto stftInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*bandpassOutputConduit);
        auto stftOutputConduit = std::make_unique<PacketConduit<STFTPacket>>();
        auto stftOutput = std::make_unique<PacketDispatcher<STFTPacket>>(*stftOutputConduit);
        auto stftProcessor = std::make_unique<STFTProcessor>(move(stftInput), move(stftOutput));
        audioProcessors.push_back(move(stftProcessor));

        // stft file writer
        if (RECORD_TO_FILES) {
            auto fileWriterInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
            auto relativePath = std::string{"stft/"};
            relativePath.append(ImpresarioSerialization::EnumNameFrequencyBand(frequencyBand));
            auto fileWriter = std::make_unique<FileWriter<STFTPacket>>(move(fileWriterInput), relativePath);
            audioProcessors.push_back(move(fileWriter));
        }

        // onset processors
        for (auto method : onsetMethods) {
            auto onsetInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
            auto onsetOutput = std::make_unique<PacketDispatcher<Serializable>>(*sinkInputConduit);
            auto onsetParameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                                         zmq::socket_type::sub, false);
            onsetParameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::onsetProcessorParameters);
            auto onsetProcessor = std::make_unique<SpecFluxOnsetProcessor>(move(onsetInput), move(onsetOutput),
                                                                           move(onsetParameterSocket), frequencyBand);
            audioProcessors.push_back(move(onsetProcessor));
        }
        packetConduitCurator->addPacketConduit(move(bandpassOutputConduit));
        packetConduitCurator->addPacketConduit(move(stftOutputConduit));
    }
    packetConduitCurator->addPacketConduit(move(audioStreamOutputConduit));
    packetConduitCurator->addPacketConduit(move(sinkInputConduit));
}

void AudioProcessorSuite::activate() {
    std::vector<std::thread> threads;
    threads.emplace_back(PacketConduitCurator::startCleaningLoop, move(packetConduitCurator));
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
