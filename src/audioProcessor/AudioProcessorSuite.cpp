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

    // harmonic transform
    auto harmonicTransformInput = std::make_unique<PacketReceiver<STFTPacket>>(*stftOutputConduit);
    auto harmonicTransformOutputConduit = std::make_unique<PacketConduit<HarmonicTransformPacket>>();
    auto harmonicTransformOutput = std::make_unique<PacketDispatcher<HarmonicTransformPacket>>(
            *harmonicTransformOutputConduit
    );
    auto harmonicTransformProcessor = std::make_unique<HarmonicTransformProcessor>(
            move(harmonicTransformInput), move(harmonicTransformOutput)
    );
    audioProcessors.push_back(move(harmonicTransformProcessor));

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
    packetConduitCurator->addPacketConduit(move(harmonicTransformOutputConduit));
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
