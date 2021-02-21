#include "AudioStreamManager.h"

AudioStreamManager::AudioStreamManager(float sampleRate, int packetSize, int bufferSizeMultiplier)
        : stream{},
          ringBuffer{packetSize, bufferSizeMultiplier} {
    logIfError(Pa_Initialize());
    logIfError(Pa_OpenDefaultStream(
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
    logIfError(Pa_StartStream(stream));
}

AudioStreamManager::~AudioStreamManager() {
    logIfError(Pa_Terminate());
}

void AudioStreamManager::logIfError(PaError portAudioReturnCode) {
    if (portAudioReturnCode != paNoError) {
        spdlog::get(LOGGER_NAME)->error(
                "portaudio encountered an error: {}", Pa_GetErrorText(portAudioReturnCode)
        );
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
