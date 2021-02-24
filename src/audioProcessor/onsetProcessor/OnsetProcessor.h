#ifndef CONDUCTOR_ONSETPROCESSOR_H
#define CONDUCTOR_ONSETPROCESSOR_H

#include <map>
#include <aubio/aubio.h>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include "Config.h"
#include "Utils.h"
#include "NetworkSocket.h"
#include "audioProcessor/AudioProcessor.h"
#include "AudioPacket_generated.h"
#include "Onset_generated.h"
#include "OnsetProcessorParameters_generated.h"

using namespace ImpresarioSerialization;
using namespace flatbuffers;

class OnsetProcessor : public AudioProcessor {
private:
    std::unique_ptr<NetworkSocket> inputSocket;
    std::unique_ptr<NetworkSocket> outputSocket;
    std::unique_ptr<NetworkSocket> parameterSocket;
    OnsetMethod method;
    aubio_onset_t *onsetAlgorithm;
    fvec_t *onsetInput;
    fvec_t *onsetResultWrapper;

    inline void updateAlgorithmParameters();

    inline void registerWithAggregator();

    inline void waitForReadySignalFromOnsetAggregator();

    inline uint64_t determineOnsetDelay(const Vector<float> *samples);

    static inline uint64_t determineOnsetTimestamp(uint64_t onsetDelay, uint64_t audioPacketTimestamp);

    inline void sendOnset(uint64_t onsetTimestamp);

public:
    static std::unique_ptr<OnsetProcessor> create(context_t &context, const string &parameterEndpoint,
                                                  const string &inputEndpoint, const string &outputEndpoint,
                                                  OnsetMethod method);

    OnsetProcessor(std::unique_ptr<NetworkSocket> inputSocket, std::unique_ptr<NetworkSocket> outputSocket,
                   std::unique_ptr<NetworkSocket> parameterSocket, OnsetMethod method);

    ~OnsetProcessor() override;

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

#endif //PERFORMER_ONSETPROCESSOR_H
