#include "AudioProcessorSuite.h"

namespace conductor {

AudioProcessorSuite::AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream)
        : audioProcessors{} {
    // general
    auto config = Config::getInstance();
    packetConduitCurator = std::make_unique<PacketConduitCurator>();

    // source
    auto audioStreamOutputConduit = std::make_unique<PacketConduit<RawAudioPacket>>();
    auto audioStreamOutput = std::make_unique<PacketDispatcher<RawAudioPacket>>(*audioStreamOutputConduit);
    auto audioStreamSource = std::make_unique<AudioStreamSource>(audioStream, move(audioStreamOutput));
    audioProcessors.push_back(move(audioStreamSource));

    // performer sink
    auto performerSinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
    auto performerSinkInput = std::make_unique<PacketReceiver<Serializable>>(*performerSinkInputConduit);
    auto performerSinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context,
                                                                                config.getPerformerOutputEndpoint(),
                                                                                zmq::socket_type::pub, true);
    auto performerSink = std::make_unique<AudioProcessorSink>(move(performerSinkInput), move(performerSinkOutput));
    audioProcessors.push_back(move(performerSink));

    // data sink
    std::unique_ptr<PacketConduit<Serializable>> dataSinkInputConduit;
    if (config.getSendData()) {
        dataSinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
        auto dataSinkInput = std::make_unique<PacketReceiver<Serializable>>(*dataSinkInputConduit);
        auto dataSinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context, config.getDataOutputEndpoint(),
                                                                               zmq::socket_type::pub, true);
        auto dataSink = std::make_unique<AudioProcessorSink>(move(dataSinkInput), move(dataSinkOutput));
        audioProcessors.push_back(move(dataSink));
    }

    // stft
    auto stftInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*audioStreamOutputConduit);
    auto stftOutputConduit = std::make_unique<PacketConduit<STFTPacket>>();
    auto stftOutput = std::make_unique<PacketDispatcher<STFTPacket>>(*stftOutputConduit);
    auto stftProcessor = std::make_unique<STFTProcessor>(move(stftInput), move(stftOutput),
                                                         WindowFunction::hamming);
    audioProcessors.push_back(move(stftProcessor));

    // curious onset processor
    auto onsetInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
    auto onsetOutputConduit = std::make_unique<PacketConduit<SpectrogramPacket>>();
    auto onsetOutput = std::make_unique<PacketDispatcher<SpectrogramPacket>>(*onsetOutputConduit);
    auto onsetParameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, config.getParameterEndpoint(),
                                                                                 zmq::socket_type::sub, false);
    onsetParameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::onsetProcessorParameters);
    auto onsetProcessor = std::make_unique<CuriousOnsetDetector>(move(onsetInput), move(onsetOutput),
                                                                 move(onsetParameterSocket));
    audioProcessors.push_back(move(onsetProcessor));

    // spectrogram file writer
    if (config.getRecordToFiles()) {
        auto fileWriterInput = std::make_unique<PacketReceiver<SpectrogramPacket>>(*onsetOutputConduit);
        auto relativePath = std::string{"spectrogram.jl"};
        auto fileWriter = std::make_unique<FileWriter<SpectrogramPacket>>(move(fileWriterInput), relativePath);
        audioProcessors.push_back(move(fileWriter));
    }

    packetConduitCurator->addPacketConduit(move(onsetOutputConduit));
    packetConduitCurator->addPacketConduit(move(stftOutputConduit));

//    // pipeline
//    for (const auto &item : SUITE_CONFIGURATION) {
//        auto frequencyBand = item.first;
//        auto onsetMethods = std::get<0>(item.second);
//        auto pitchMethods = std::get<1>(item.second);
//
//        // bandpass filter
//        auto bandpassInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*audioStreamOutputConduit);
//        auto bandpassOutputConduit = std::make_unique<PacketConduit<RawAudioPacket>>();
//        auto bandpassOutput = std::make_unique<PacketDispatcher<RawAudioPacket>>(*bandpassOutputConduit);
//        auto bandpassProcessor = std::make_unique<BandpassProcessor>(move(bandpassInput), move(bandpassOutput),
//                                                                     frequencyBand);
//        audioProcessors.push_back(move(bandpassProcessor));
//
//        // audio file writer
//        if (config.getRecordToFiles()) {
//            auto fileWriterInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*bandpassOutputConduit);
//            auto relativePath = std::string{"raw_audio/"};
//            relativePath.append(ImpresarioSerialization::EnumNameFrequencyBand(frequencyBand));
//            relativePath.append(".wav");
//            auto fileWriter = std::make_unique<FileWriter<RawAudioPacket>>(move(fileWriterInput), relativePath);
//            audioProcessors.push_back(move(fileWriter));
//        }
//
//        // stft
//        auto stftInput = std::make_unique<PacketReceiver<RawAudioPacket>>(*bandpassOutputConduit);
//        auto stftOutputConduit = std::make_unique<PacketConduit<STFTPacket>>();
//        auto stftOutput = std::make_unique<PacketDispatcher<STFTPacket>>(*stftOutputConduit);
//        auto stftProcessor = std::make_unique<STFTProcessor>(move(stftInput), move(stftOutput),
//                                                             WindowFunction::hamming);
//        audioProcessors.push_back(move(stftProcessor));
//
//        // onset processors
//        for (auto method : onsetMethods) {
//            auto onsetInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
//            auto onsetOutput = std::make_unique<PacketDispatcher<Serializable>>(*performerSinkInputConduit);
//            auto onsetParameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, config.getParameterEndpoint(),
//                                                                                         zmq::socket_type::sub, false);
//            onsetParameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::onsetProcessorParameters);
//            auto onsetProcessor = std::make_unique<SpecFluxOnsetProcessor>(move(onsetInput), move(onsetOutput),
//                                                                           move(onsetParameterSocket), frequencyBand);
//            audioProcessors.push_back(move(onsetProcessor));
//        }
//        packetConduitCurator->addPacketConduit(move(bandpassOutputConduit));
//        packetConduitCurator->addPacketConduit(move(stftOutputConduit));
//    }
    packetConduitCurator->addPacketConduit(move(audioStreamOutputConduit));
    packetConduitCurator->addPacketConduit(move(performerSinkInputConduit));
    if (config.getSendData()) {
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
