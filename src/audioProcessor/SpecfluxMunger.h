#ifndef CONDUCTOR_SPECFLUXMUNGER_H
#define CONDUCTOR_SPECFLUXMUNGER_H

#include <packet/HarmonicTransformPacket.h>
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketReceiver.h"
#include "packet/PacketDispatcher.h"

namespace conductor {

class SpecfluxMunger : public AudioProcessor {
    std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input;
    std::unique_ptr<PacketDispatcher<STFTPacket>> output;

public:
    SpecfluxMunger(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                   std::unique_ptr<PacketDispatcher<STFTPacket>> output);

    void process() override;
};

}

#endif //CONDUCTOR_SPECFLUXMUNGER_H
