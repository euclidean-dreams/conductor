#include "AudioInput.h"

namespace conductor {

AudioInput::AudioInput(float sampleRate, int packetSize)
        : inputStream{},
          outputStream{packetSize} {
    enumerateAudioDevices();
    auto inputDevice = Config::getInstance().getAudioDevice();
    spdlog::get(Config::getInstance().getLoggerName())->info(
            "using audio device: {}", Pa_GetDeviceInfo(inputDevice)->name);
    PaStreamParameters inputParameters;
    inputParameters.device = inputDevice;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputDevice)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;
    throwOnPortaudioError(Pa_OpenStream(
            &inputStream,
            &inputParameters,
            nullptr,
            sampleRate,
            paFramesPerBufferUnspecified,
            paNoFlag,
            streamCallback,
            &outputStream
    ));
    PaAlsa_EnableRealtimeScheduling(inputStream, true);
    throwOnPortaudioError(Pa_StartStream(inputStream));
}

AudioStream &AudioInput::getAudioStream() {
    return outputStream;
}

int AudioInput::streamCallback(const void *input, void *output, unsigned long frameCount,
                               const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                               void *userData) {
    auto samples = static_cast<const float *>(input);
    auto ringBuffer = static_cast<RingBuffer *>(userData);
    ringBuffer->addSamples(samples, frameCount);
    return paContinue;
}

void AudioInput::enumerateAudioDevices() {
    auto logger = spdlog::get(Config::getInstance().getLoggerName());
    logger->info("enumerating audio devices");
    for (int i = 0; i < Pa_GetDeviceCount(); i++) {
        auto defaultDeviceInfo = Pa_GetDeviceInfo(i);
        logger->info("device {}: {}", i, defaultDeviceInfo->name);
    }
}

}
