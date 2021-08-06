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

    // equalizer
    auto equalizerInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
    auto equalizerOutputConduit = std::make_unique<PacketConduit<STFTPacket>>();
    auto equalizerOutput = std::make_unique<PacketDispatcher<STFTPacket>>(*equalizerOutputConduit);
    auto equalizerProcessor = std::make_unique<EqualizerProcessor>(move(equalizerInput), move(equalizerOutput));
    audioProcessors.push_back(move(equalizerProcessor));

    // harmonic transform
    auto harmonicTransformInput = std::make_unique<PacketReceiver<STFTPacket>>(*equalizerOutputConduit);
    auto harmonicTransformOutputConduit = std::make_unique<PacketConduit<HarmonicTransformPacket>>();
    auto harmonicTransformOutput = std::make_unique<PacketDispatcher<HarmonicTransformPacket>>(
            *harmonicTransformOutputConduit
    );
    auto harmonicTransformProcessor = std::make_unique<HarmonicTransformProcessor>(
            move(harmonicTransformInput), move(harmonicTransformOutput)
    );
    audioProcessors.push_back(move(harmonicTransformProcessor));

    // specflux munger processor
    auto mungerInput = std::make_unique<PacketReceiver<HarmonicTransformPacket>>(
            *harmonicTransformOutputConduit
    );
    auto mungerOutputConduit = std::make_unique<PacketConduit<STFTPacket>>();
    auto mungerOutput = std::make_unique<PacketDispatcher<STFTPacket>>(
            *mungerOutputConduit
    );
    auto mungerProcessor = std::make_unique<SpecfluxMunger>(
            move(mungerInput), move(mungerOutput)
    );
    audioProcessors.push_back(move(mungerProcessor));

    // onset processor
    auto onsetInput = std::make_unique<PacketReceiver<STFTPacket>>(*mungerOutputConduit);
    auto onsetOutput = std::make_unique<PacketDispatcher<Serializable>>(*performerSinkInputConduit);
    auto onsetParameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, config.getParameterEndpoint(),
                                                                                 zmq::socket_type::sub, false);
    onsetParameterSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::onsetProcessorParameters);
    auto onsetProcessor = std::make_unique<SpecFluxOnsetProcessor>(move(onsetInput), move(onsetOutput),
                                                                   move(onsetParameterSocket),
                                                                   ImpresarioSerialization::FrequencyBand::all);
    audioProcessors.push_back(move(onsetProcessor));

//    // mel filterbank
//    auto melFilterbankInput = std::make_unique<PacketReceiver<HarmonicTransformPacket>>(
//            *harmonicTransformOutputConduit
//    );
//    auto melFilterbankOutputConduit = std::make_unique<PacketConduit<MelSignalPacket>>();
//    auto melFilterbankOutput = std::make_unique<PacketDispatcher<MelSignalPacket>>(
//            *melFilterbankOutputConduit
//    );
//    auto melFilterbankProcessor = std::make_unique<MelFilterbankProcessor>(
//            move(melFilterbankInput), move(melFilterbankOutput)
//    );
//    audioProcessors.push_back(move(melFilterbankProcessor));
//
//    // display signal processor
//    auto displaySignalInput = std::make_unique<PacketReceiver<MelSignalPacket>>(
//            *melFilterbankOutputConduit
//    );
//    auto displaySignalOutput = std::make_unique<PacketDispatcher<Serializable>>(*performerSinkInputConduit);
//    auto displaySignalProcessor = std::make_unique<DisplaySignalProcessor>(
//            move(displaySignalInput), move(displaySignalOutput)
//    );
//    audioProcessors.push_back(move(displaySignalProcessor));
//
    // spectrogram output
    if (config.getRecordToFiles()) {
        auto spectrogramInput = std::make_unique<PacketReceiver<HarmonicTransformPacket>>(
                *harmonicTransformOutputConduit
        );
        auto spectrogramOutputConduit = std::make_unique<PacketConduit<SpectrogramPacket>>();
        auto spectrogramOutput = std::make_unique<PacketDispatcher<SpectrogramPacket>>(*spectrogramOutputConduit);
        auto spectrogramProcessor = std::make_unique<SpectrogramProcessor>(
                move(spectrogramInput), move(spectrogramOutput)
        );
        audioProcessors.push_back(move(spectrogramProcessor));

        auto fileWriterInput = std::make_unique<PacketReceiver<SpectrogramPacket>>(*spectrogramOutputConduit);
        auto relativePath = std::string{"quickAndCurious.jl"};
        auto fileWriter = std::make_unique<FileWriter<SpectrogramPacket>>(move(fileWriterInput), relativePath);
        audioProcessors.push_back(move(fileWriter));
        packetConduitCurator->addPacketConduit(move(spectrogramOutputConduit));
    }

    // packet conduit curation
    packetConduitCurator->addPacketConduit(move(stftOutputConduit));
    packetConduitCurator->addPacketConduit(move(equalizerOutputConduit));
    packetConduitCurator->addPacketConduit(move(harmonicTransformOutputConduit));
//    packetConduitCurator->addPacketConduit(move(melFilterbankOutputConduit));
    packetConduitCurator->addPacketConduit(move(audioStreamOutputConduit));
    packetConduitCurator->addPacketConduit(move(performerSinkInputConduit));
    packetConduitCurator->addPacketConduit(move(mungerOutputConduit));
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
