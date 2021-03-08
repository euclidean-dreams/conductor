#ifndef CONDUCTOR_AUDIOPROCESSORSUITE_H
#define CONDUCTOR_AUDIOPROCESSORSUITE_H

#include <memory>
#include <thread>
#include <aubio/aubio.h>
#include <zmq.hpp>
#include "Config.h"
#include "NonCopyable.h"
#include "NetworkSocket.h"
#include "AudioStreamSource.h"
#include "AudioProcessorSink.h"
#include "audioStream/AudioStream.h"
#include "OnsetProcessor.h"

using namespace std;
using namespace zmq;

class AudioProcessorSuite : NonCopyable {
private:
    vector<std::unique_ptr<AudioProcessor>> audioProcessors;
    int endpointCounter;

    inline string generateInprocEndpoint();

public:
    AudioProcessorSuite(context_t &context, AudioStream &audioStream);

    ~AudioProcessorSuite();

    void activate();

    static void activateAudioProcessor(AudioProcessor &audioProcessor);
};

#endif //CONDUCTOR_AUDIOPROCESSORSUITE_H
