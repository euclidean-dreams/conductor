#ifndef CONDUCTOR_TIMBRALTRANSFORMPROCESSOR_H
#define CONDUCTOR_TIMBRALTRANSFORMPROCESSOR_H

#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/STFTPacket.h"
#include "packet/HarmonicTransformPacket.h"

namespace conductor {

class TimbralTransformProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<STFTPacket>> input;
    std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output;
    int psi;

public:
    TimbralTransformProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                              std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output);

    void process() override;
};

}

#endif //CONDUCTOR_TIMBRALTRANSFORMPROCESSOR_H
