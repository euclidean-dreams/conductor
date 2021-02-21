#ifndef CONDUCTOR_STREAMMANAGER_H
#define CONDUCTOR_STREAMMANAGER_H

#include <memory>
#include <portaudio.h>
#include <iostream>
#include <pa_linux_alsa.h>
#include <spdlog/spdlog.h>
#include "RingBuffer.h"
#include "NonCopyable.h"

using namespace std;

class AudioStreamManager : NonCopyable {
private:
    PaStream *stream;
    RingBuffer ringBuffer;

    inline static void logIfError(PaError portAudioReturnCode);

public:
    AudioStreamManager(float sampleRate, int packetSize, int bufferSizeMultiplier);

    ~AudioStreamManager();

    AudioStream &getAudioStream();

    static int streamCallback(const void *input,
                              void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData);
};

#endif //PERFORMER_STREAMMANAGER_H
