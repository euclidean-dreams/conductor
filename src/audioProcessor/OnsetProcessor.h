#ifndef CONDUCTOR_ONSETPROCESSOR_H
#define CONDUCTOR_ONSETPROCESSOR_H

#include <aubio/aubio.h>
#include <Time.h>
#include <NetworkSocket.h>
#include <Onset_generated.h>
#include <OnsetProcessorParameters_generated.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/RawAudioPacket.h"
#include "packet/OnsetPacket.h"
#include "packet/PacketConduit.h"
#include "packet/PacketSpout.h"

namespace conductor {

class OnsetProcessor : public AudioProcessor {
private:
    inline static const smpl_t DEFAULT_THRESHOLD = 0.3;
    inline static const smpl_t DEFAULT_SILENCE = -60.0;
    inline static const smpl_t DEFAULT_MINIOI_MS = 1;
    inline static const bool DEFAULT_ADAPTIVE_WHITENING = true;

    std::unique_ptr<PacketSpout> input;
    std::shared_ptr<PacketConduit> output;
    std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket;
    ImpresarioSerialization::OnsetMethod method;
    aubio_onset_t *onsetAlgorithm;
    fvec_t *onsetInput;
    fvec_t *onsetResultWrapper;

    void updateAlgorithmParameters();

    uint64_t determineOnsetDelay(PacketCollection &packets);

    static uint64_t determineOnsetTimestamp(uint64_t onsetDelay, uint64_t rawAudioPacketTimestamp);

public:
    OnsetProcessor(std::unique_ptr<PacketSpout> input, std::shared_ptr<PacketConduit> output,
                   std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                   ImpresarioSerialization::OnsetMethod method);

    ~OnsetProcessor() override;

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //PERFORMER_ONSETPROCESSOR_H
