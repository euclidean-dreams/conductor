#ifndef CONDUCTOR_ONSETAGGREGATOR_H
#define CONDUCTOR_ONSETAGGREGATOR_H

#include <spdlog/spdlog.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "NetworkSocket.h"
#include "Onset_generated.h"

using namespace flatbuffers;

class OnsetAggregator : public AudioProcessor {
private:
    std::unique_ptr<NetworkSocket> inputSocket;
    std::unique_ptr<NetworkSocket> outputSocket;
    int onsetProcessorCount;
    vector<string> identities;

    inline void collectIdentities();

    inline void sendReadySignal();

public:
    static std::unique_ptr<OnsetAggregator> create(context_t &context, const string &inputEndpoint,
                                                   const string &outputEndpoint, int onsetProcessorCount);

    OnsetAggregator(std::unique_ptr<NetworkSocket> inputSocket, std::unique_ptr<NetworkSocket> outputSocket,
                    int onsetProcessorCount);

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

#endif //CONDUCTOR_ONSETAGGREGATOR_H
