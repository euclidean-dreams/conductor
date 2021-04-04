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

    // performer sink
    auto performerSinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
    auto performerSinkInput = std::make_unique<PacketReceiver<Serializable>>(*performerSinkInputConduit);
    auto performerSinkOutputEndpoint = static_cast<std::string>(PERFORMER_OUTPUT_ENDPOINT);
    auto performerSinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context, performerSinkOutputEndpoint,
                                                                                zmq::socket_type::pub, true);
    auto performerSink = std::make_unique<AudioProcessorSink>(move(performerSinkInput), move(performerSinkOutput));
    audioProcessors.push_back(move(performerSink));

    // data sink
    std::unique_ptr<PacketConduit<Serializable>> dataSinkInputConduit;
    if (SEND_DATA) {
        dataSinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
        auto dataSinkInput = std::make_unique<PacketReceiver<Serializable>>(*dataSinkInputConduit);
        auto dataSinkOutputEndpoint = static_cast<std::string>(DATA_OUTPUT_ENDPOINT);
        auto dataSinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context, dataSinkOutputEndpoint,
                                                                               zmq::socket_type::pub, true);
        auto dataSink = std::make_unique<AudioProcessorSink>(move(dataSinkInput), move(dataSinkOutput));
        audioProcessors.push_back(move(dataSink));
    }

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
        auto stftProcessor = std::make_unique<STFTProcessor>(move(stftInput), move(stftOutput),
                                                             WindowFunction::hamming);
        audioProcessors.push_back(move(stftProcessor));

        // stft output
        if (SEND_DATA) {
            auto packetTypeConverterInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
            auto packetTypeConverterOutput = std::make_unique<PacketDispatcher<Serializable>>(
                    *dataSinkInputConduit
            );
            auto packetTypeConverter = std::make_unique<PacketTypeConverter<STFTPacket, Serializable>>(
                    move(packetTypeConverterInput), move(packetTypeConverterOutput)
            );
            audioProcessors.push_back(move(packetTypeConverter));
        }

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
            auto onsetOutput = std::make_unique<PacketDispatcher<Serializable>>(*performerSinkInputConduit);
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
    packetConduitCurator->addPacketConduit(move(performerSinkInputConduit));
    if (SEND_DATA) {
        packetConduitCurator->addPacketConduit(move(dataSinkInputConduit));
    }
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
