#include "AudioStreamManager.h"

namespace conductor {

AudioStreamManager::AudioStreamManager(float sampleRate, int packetSize, int bufferSizeMultiplier)
        : stream{},
          ringBuffer{packetSize, bufferSizeMultiplier} {
    throwOnError(Pa_Initialize());
    throwOnError(Pa_OpenDefaultStream(
            &stream,
            1,
            0,
            paFloat32,
            sampleRate,
            paFramesPerBufferUnspecified,
            streamCallback,
            &ringBuffer
    ));
    PaAlsa_EnableRealtimeScheduling(stream, true);
    throwOnError(Pa_StartStream(stream));
}

AudioStreamManager::~AudioStreamManager() {
    throwOnError(Pa_Terminate());
}

void AudioStreamManager::throwOnError(PaError portAudioReturnCode) {
    if (portAudioReturnCode != paNoError) {
        throw std::runtime_error(Pa_GetErrorText(portAudioReturnCode));
    }
}

AudioStream &AudioStreamManager::getAudioStream() {
    return ringBuffer;
}

int AudioStreamManager::streamCallback(const void *input,
                                       void *output,
                                       unsigned long frameCount,
                                       const PaStreamCallbackTimeInfo *timeInfo,
                                       PaStreamCallbackFlags statusFlags,
                                       void *userData) {
    auto samples = (float *) input;
    auto ringBuffer = (RingBuffer *) userData;
    ringBuffer->addSamples(samples, frameCount);
    return paContinue;
}

}
