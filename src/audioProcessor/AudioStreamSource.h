#ifndef CONDUCTOR_AUDIOMANAGER_H
#define CONDUCTOR_AUDIOMANAGER_H

#include <thread>
#include <memory>
#include "Config.h"
#include "Utils.h"
#include "AudioProcessor.h"
#include "NetworkSocket.h"
#include "audioStream/AudioStream.h"
#include "AudioPacket_generated.h"

using namespace ImpresarioSerialization;
using namespace flatbuffers;
using namespace std::chrono;
using namespace std::this_thread;

class AudioStreamSource : public AudioProcessor {
private:
    AudioStream &audioStream;
    std::unique_ptr<NetworkSocket> outputSocket;

public:
    static std::unique_ptr<AudioProcessor> create(context_t &context, AudioStream &audioStream,
                                                  const string &outputEndpoint);

    AudioStreamSource(AudioStream &audioStream, std::unique_ptr<NetworkSocket> outputSocket);

    void setup() override {};

    void process() override;

    bool shouldContinue() override;

};

#endif //PERFORMER_AUDIOMANAGER_H
