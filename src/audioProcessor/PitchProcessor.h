#ifndef CONDUCTOR_PITCHPROCESSOR_H
#define CONDUCTOR_PITCHPROCESSOR_H

#include <memory>
#include <aubio/aubio.h>
#include <NetworkSocket.h>
#include <Pitch_generated.h>
#include <PitchProcessorParameters_generated.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketConduit.h"
#include "packet/Packet.h"
#include "packet/PitchPacket.h"
#include "packet/AudioPacket.h"
#include "packet/PacketSpout.h"

namespace conductor {

class PitchProcessor : public AudioProcessor {
private:
    inline static const smpl_t DEFAULT_THRESHOLD = 0.3;
    inline static const smpl_t DEFAULT_SILENCE = -60.0;
    inline static const std::string_view PITCH_UNIT = "midi";

    std::unique_ptr<PacketSpout> input;
    std::shared_ptr<PacketConduit> output;
    std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket;
    ImpresarioSerialization::PitchMethod method;
    aubio_pitch_t *pitchAlgorithm;
    fvec_t *pitchInput;
    fvec_t *pitchResultWrapper;
    float pitchThreshold;

    void updateAlgorithmParameters();

public:
    PitchProcessor(std::unique_ptr<PacketSpout> input, std::shared_ptr<PacketConduit> output,
                   std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                   ImpresarioSerialization::PitchMethod method);

    ~PitchProcessor() override;

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_PITCHPROCESSOR_H
