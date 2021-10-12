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

    // performer sinks
    auto matrixPerformerSinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
    auto matrixPerformerSinkInput = std::make_unique<PacketReceiver<Serializable>>(*matrixPerformerSinkInputConduit);
    auto matrixPerformerSinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context,
                                                                                      config.getMatrixPerformerOutputEndpoint(),
                                                                                      zmq::socket_type::pub, true);
    auto matrixPerformerSink = std::make_unique<AudioProcessorSink>(move(matrixPerformerSinkInput),
                                                                    move(matrixPerformerSinkOutput));
    audioProcessors.push_back(move(matrixPerformerSink));
    auto bannerPerformerSinkInputConduit = std::make_unique<PacketConduit<Serializable>>();
    auto bannerPerformerSinkInput = std::make_unique<PacketReceiver<Serializable>>(*bannerPerformerSinkInputConduit);
    auto bannerPerformerSinkOutput = std::make_unique<impresarioUtils::NetworkSocket>(context,
                                                                                      config.getBannerPerformerOutputEndpoint(),
                                                                                      zmq::socket_type::pub, true);
    auto bannerPerformerSink = std::make_unique<AudioProcessorSink>(move(bannerPerformerSinkInput),
                                                                    move(bannerPerformerSinkOutput));
    audioProcessors.push_back(move(bannerPerformerSink));

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
    auto morselSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, config.getMorselEndpoint(),
                                                                         zmq::socket_type::sub, false);
    morselSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::floatMorsel);
    morselSocket->setSubscriptionFilter(ImpresarioSerialization::Identifier::floatArrayMorsel);
    auto equalizerInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
    auto equalizerOutputConduit = std::make_unique<PacketConduit<STFTPacket>>();
    auto equalizerOutput = std::make_unique<PacketDispatcher<STFTPacket>>(*equalizerOutputConduit);
    auto equalizerProcessor = std::make_unique<EqualizerProcessor>(
            move(equalizerInput), move(equalizerOutput), move(morselSocket)
    );
    audioProcessors.push_back(move(equalizerProcessor));

    // harmonic transform
    auto harmonicTransformInput = std::make_unique<PacketReceiver<STFTPacket>>(*equalizerOutputConduit);
    auto harmonicTransformOutputConduit = std::make_unique<PacketConduit<HarmonicTransformPacket>>();
    auto harmonicTransformOutput = std::make_unique<PacketDispatcher<HarmonicTransformPacket>>(
            *harmonicTransformOutputConduit
    );
    auto harmonicTransformProcessor = std::make_unique<TimbralTransformProcessor>(
            move(harmonicTransformInput), move(harmonicTransformOutput)
    );
    audioProcessors.push_back(move(harmonicTransformProcessor));

    // mel filterbank
    auto matrixMelFilterbankInput = std::make_unique<PacketReceiver<HarmonicTransformPacket>>(
            *harmonicTransformOutputConduit
    );
    auto matrixMelFilterbankOutputConduit = std::make_unique<PacketConduit<MelSignalPacket>>();
    auto matrixMelFilterbankOutput = std::make_unique<PacketDispatcher<MelSignalPacket>>(
            *matrixMelFilterbankOutputConduit
    );
    auto matrixMelFilterbankProcessor = std::make_unique<MelFilterBankProcessor>(
            move(matrixMelFilterbankInput), move(matrixMelFilterbankOutput), config.getMatrixLedCount()
    );
    audioProcessors.push_back(move(matrixMelFilterbankProcessor));
    auto bannerMelFilterbankInput = std::make_unique<PacketReceiver<HarmonicTransformPacket>>(
            *harmonicTransformOutputConduit
    );
    auto bannerMelFilterbankOutputConduit = std::make_unique<PacketConduit<MelSignalPacket>>();
    auto bannerMelFilterbankOutput = std::make_unique<PacketDispatcher<MelSignalPacket>>(
            *bannerMelFilterbankOutputConduit
    );
    auto bannerMelFilterbankProcessor = std::make_unique<MelFilterBankProcessor>(
            move(bannerMelFilterbankInput), move(bannerMelFilterbankOutput), config.getBannerLedCount()
    );
    audioProcessors.push_back(move(bannerMelFilterbankProcessor));

    // display signal processors
    auto matrixDisplaySignalInput = std::make_unique<PacketReceiver<MelSignalPacket>>(
            *matrixMelFilterbankOutputConduit
    );
    auto matrixDisplaySignalOutput = std::make_unique<PacketDispatcher<Serializable>>(*matrixPerformerSinkInputConduit);
    auto matrixSignalProcessor = std::make_unique<MaterializationProcessor>(
            move(matrixDisplaySignalInput), move(matrixDisplaySignalOutput)
    );
    audioProcessors.push_back(move(matrixSignalProcessor));
    auto bannerDisplaySignalInput = std::make_unique<PacketReceiver<MelSignalPacket>>(
            *matrixMelFilterbankOutputConduit
    );
    auto bannerDisplaySignalOutput = std::make_unique<PacketDispatcher<Serializable>>(*bannerPerformerSinkInputConduit);
    auto bannerSignalProcessor = std::make_unique<MaterializationProcessor>(
            move(bannerDisplaySignalInput), move(bannerDisplaySignalOutput)
    );
    audioProcessors.push_back(move(bannerSignalProcessor));

    // spectrogram output
    if (config.getRecordToFiles()) {
        auto spectrogramInput = std::make_unique<PacketReceiver<MelSignalPacket>>(
                *matrixMelFilterbankOutputConduit
        );
        auto spectrogramOutputConduit = std::make_unique<PacketConduit<SpectrogramPacket>>();
        auto spectrogramOutput = std::make_unique<PacketDispatcher<SpectrogramPacket>>(*spectrogramOutputConduit);
        auto spectrogramProcessor = std::make_unique<SpectrogramProcessor>(
                move(spectrogramInput), move(spectrogramOutput)
        );
        audioProcessors.push_back(move(spectrogramProcessor));

        auto fileWriterInput = std::make_unique<PacketReceiver<SpectrogramPacket>>(*spectrogramOutputConduit);
        auto relativePath = std::string{"output.condout"};
        auto fileWriter = std::make_unique<FileWriter<SpectrogramPacket>>(move(fileWriterInput), relativePath);
        audioProcessors.push_back(move(fileWriter));
        packetConduitCurator->addPacketConduit(move(spectrogramOutputConduit));
    }

    // packet conduit curation
    packetConduitCurator->addPacketConduit(move(stftOutputConduit));
    packetConduitCurator->addPacketConduit(move(equalizerOutputConduit));
    packetConduitCurator->addPacketConduit(move(harmonicTransformOutputConduit));
    packetConduitCurator->addPacketConduit(move(matrixMelFilterbankOutputConduit));
    packetConduitCurator->addPacketConduit(move(bannerMelFilterbankOutputConduit));
    packetConduitCurator->addPacketConduit(move(audioStreamOutputConduit));
    packetConduitCurator->addPacketConduit(move(matrixPerformerSinkInputConduit));
    packetConduitCurator->addPacketConduit(move(bannerPerformerSinkInputConduit));
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
