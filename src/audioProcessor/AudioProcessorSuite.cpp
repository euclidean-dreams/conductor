#include "AudioProcessorSuite.h"

namespace conductor {

AudioProcessorSuite::AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream)
        : audioProcessors{},
          endpointCounter{0} {
    // source
    auto sourceOutputEndpoint = generateInprocEndpoint();
    auto audioStreamSource = AudioStreamSource::create(context, audioStream, sourceOutputEndpoint);
    audioProcessors.push_back(move(audioStreamSource));

    // sink
    auto sinkInputEndpoint = generateInprocEndpoint();
    auto sinkOutputEndpoint = static_cast<std::string>(CONDUCTOR_OUTPUT_ENDPOINT);
    auto audioProcessorSink = AudioProcessorSink::create(context, sinkInputEndpoint, sinkOutputEndpoint);
    audioProcessors.push_back(move(audioProcessorSink));

    // onset
    auto parameterEndpoint = static_cast<std::string>(PARAMETER_ENDPOINT);
    for (auto method : ONSET_PROCESSORS) {
        auto processor = OnsetProcessor::create(context, parameterEndpoint, sourceOutputEndpoint, sinkInputEndpoint,
                                                method);
        audioProcessors.push_back(move(processor));
    }
}

AudioProcessorSuite::~AudioProcessorSuite() {
    aubio_cleanup();
}

std::string AudioProcessorSuite::generateInprocEndpoint() {
    std::string endpoint{"inproc://AudioProcessorSuite:"};
    endpoint.append(std::to_string(endpointCounter));
    endpointCounter++;
    return endpoint;
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
