#ifndef CONDUCTOR_ONSETPROCESSOR_H
#define CONDUCTOR_ONSETPROCESSOR_H

//#include <map>
#include <aubio/aubio.h>
//#include <zmq.hpp>
//#include <spdlog/spdlog.h>
#include <Time.h>
#include <NetworkSocket.h>
#include <Onset_generated.h>
#include <OnsetProcessorParameters_generated.h>
#include <AudioPacket_generated.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"

namespace conductor {

class OnsetProcessor : public AudioProcessor {
private:
    std::unique_ptr<impresarioUtils::NetworkSocket> inputSocket;
    std::unique_ptr<impresarioUtils::NetworkSocket> outputSocket;
    std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket;
    ImpresarioSerialization::OnsetMethod method;
    aubio_onset_t *onsetAlgorithm;
    fvec_t *onsetInput;
    fvec_t *onsetResultWrapper;

    void updateAlgorithmParameters();

    uint64_t determineOnsetDelay(const flatbuffers::Vector<float> *samples);

    static uint64_t determineOnsetTimestamp(uint64_t onsetDelay, uint64_t audioPacketTimestamp);

    void sendOnset(uint64_t onsetTimestamp);

public:
    static std::unique_ptr<OnsetProcessor> create(zmq::context_t &context, const std::string &parameterEndpoint,
                                                  const std::string &inputEndpoint, const std::string &outputEndpoint,
                                                  ImpresarioSerialization::OnsetMethod method);

    OnsetProcessor(std::unique_ptr<impresarioUtils::NetworkSocket> inputSocket,
                   std::unique_ptr<impresarioUtils::NetworkSocket> outputSocket,
                   std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                   ImpresarioSerialization::OnsetMethod method);

    ~OnsetProcessor() override;

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //PERFORMER_ONSETPROCESSOR_H
