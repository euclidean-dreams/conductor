#include "AudioProcessorSuite.h"

AudioProcessorSuite::AudioProcessorSuite(context_t &context, AudioStream &audioStream)
        : audioProcessors{},
          endpointCounter{0} {
    auto sourceOutputEndpoint = generateInprocEndpoint();
    auto audioStreamSource = AudioStreamSource::create(context, audioStream, sourceOutputEndpoint);
    audioProcessors.push_back(move(audioStreamSource));

    auto onsetAggregatorInputEndpoint = generateInprocEndpoint();
    auto onsetAggregator = OnsetAggregator::create(context, onsetAggregatorInputEndpoint,
                                                   ONSET_AGGREGATOR_OUTPUT_ENDPOINT, size(ONSET_PROCESSORS));
    audioProcessors.push_back(move(onsetAggregator));

    for (auto method : ONSET_PROCESSORS) {
        auto processor = OnsetProcessor::create(context, PARAMETER_RECEIVER_ENDPOINT, sourceOutputEndpoint,
                                                onsetAggregatorInputEndpoint, method);
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
