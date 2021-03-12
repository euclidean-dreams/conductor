#ifndef CONDUCTOR_AUDIOPROCESSORSUITE_H
#define CONDUCTOR_AUDIOPROCESSORSUITE_H

#include <thread>
#include <memory>
#include <vector>
#include <zmq.hpp>
#include <NonCopyable.h>
#include "audioProcessor/AudioStreamSource.h"
#include "audioProcessor/AudioProcessorSink.h"
#include "audioProcessor/OnsetProcessor.h"
#include "audioStream/AudioStream.h"
#include "packet/PacketConduit.h"
#include "packet/PacketConduitManager.h"

namespace conductor {

class AudioProcessorSuite : impresarioUtils::NonCopyable {
private:
    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;

public:
    AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream);

    ~AudioProcessorSuite();

    void activate();

    static void activateAudioProcessor(AudioProcessor &audioProcessor);
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSUITE_H
