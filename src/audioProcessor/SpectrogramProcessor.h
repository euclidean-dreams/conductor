#ifndef CONDUCTOR_SPECTROGRAMPROCESSOR_H
#define CONDUCTOR_SPECTROGRAMPROCESSOR_H

#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/HarmonicTransformPacket.h"
#include "packet/SpectrogramPacket.h"

namespace conductor {

class SpectrogramProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input;
    std::unique_ptr<PacketDispatcher<SpectrogramPacket>> output;

public:
    SpectrogramProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                         std::unique_ptr<PacketDispatcher<SpectrogramPacket>> output);

    void process() override;
};

}

#endif //CONDUCTOR_SPECTROGRAMPROCESSOR_H
