#ifndef CONDUCTOR_STREAMMANAGER_H
#define CONDUCTOR_STREAMMANAGER_H

#include <memory>
#include <portaudio.h>
#include <pa_linux_alsa.h>
#include <NonCopyable.h>
#include "RingBuffer.h"
#include "audioStream/AudioUtils.h"

namespace conductor {

class AudioInput : impresarioUtils::NonCopyable {
private:
    PaStream *inputStream;
    RingBuffer outputStream;

public:
    AudioInput(float sampleRate, int packetSize);

    AudioStream &getAudioStream();

    static int streamCallback(const void *input, void *output, unsigned long frameCount,
                              const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                              void *userData);
};

}

#endif //PERFORMER_STREAMMANAGER_H
