#include "AudioProcessorSuite.h"

AudioProcessorSuite::AudioProcessorSuite(context_t &context, AudioStream &audioStream)
        : audioProcessors{},
          endpointCounter{0} {
    auto sourceOutputEndpoint = generateInprocEndpoint();
    auto audioStreamSource = AudioStreamSource::create(context, audioStream, sourceOutputEndpoint);
    audioProcessors.push_back(move(audioStreamSource));

    auto sinkInputEndpoint = generateInprocEndpoint();
    auto sinkOutputEndpoint = static_cast<string>(CONDUCTOR_OUTPUT_ENDPOINT);
    auto audioProcessorSink = AudioProcessorSink::create(context, sinkInputEndpoint, sinkOutputEndpoint);
    audioProcessors.push_back(move(audioProcessorSink));

    auto parameterEndpoint = static_cast<string>(PARAMETER_ENDPOINT);
    for (auto method : ONSET_PROCESSORS) {
        auto processor = OnsetProcessor::create(context, parameterEndpoint, sourceOutputEndpoint, sinkInputEndpoint,
                                                method);
        audioProcessors.push_back(move(processor));
    }
}

AudioProcessorSuite::~AudioProcessorSuite() {
    aubio_cleanup();
}

string AudioProcessorSuite::generateInprocEndpoint() {
    string endpoint{"inproc://AudioProcessorSuite:"};
    endpoint.append(to_string(endpointCounter));
    endpointCounter++;
    return endpoint;
}

void AudioProcessorSuite::activate() {
    vector<thread> threads;
    for (auto &audioProcessor: audioProcessors) {
        threads.emplace_back(activateAudioProcessor, ref(*audioProcessor));
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
