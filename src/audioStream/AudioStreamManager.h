#ifndef CONDUCTOR_STREAMMANAGER_H
#define CONDUCTOR_STREAMMANAGER_H

#include <memory>
#include <portaudio.h>
#include <pa_linux_alsa.h>
#include <NonCopyable.h>
#include "RingBuffer.h"

namespace conductor {

class AudioStreamManager : impresarioUtils::NonCopyable {
private:
    PaStream *stream;
    RingBuffer ringBuffer;

    static void throwOnError(PaError portAudioReturnCode);

public:
    AudioStreamManager(float sampleRate, int packetSize);

    ~AudioStreamManager();

    AudioStream &getAudioStream();

    static int streamCallback(const void *input,
                              void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData);
};

}

#endif //PERFORMER_STREAMMANAGER_H
