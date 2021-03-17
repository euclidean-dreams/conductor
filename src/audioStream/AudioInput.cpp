#include "AudioInput.h"

namespace conductor {

AudioInput::AudioInput(float sampleRate, int packetSize)
        : inputStream{},
          outputStream{packetSize} {
    throwOnPortaudioError(Pa_OpenDefaultStream(
            &inputStream,
            1,
            0,
            paFloat32,
            sampleRate,
            paFramesPerBufferUnspecified,
            streamCallback,
            &outputStream
    ));
    PaAlsa_EnableRealtimeScheduling(inputStream, true);
    throwOnPortaudioError(Pa_StartStream(inputStream));
}

AudioStream &AudioInput::getAudioStream() {
    return outputStream;
}

int AudioInput::streamCallback(const void *input,
                               void *output,
                               unsigned long frameCount,
                               const PaStreamCallbackTimeInfo *timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData) {
    auto samples = static_cast<const float *>(input);
    auto ringBuffer = (RingBuffer *) userData;
    ringBuffer->addSamples(samples, frameCount);
    return paContinue;
}

}
