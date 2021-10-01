#ifndef CONDUCTOR_MATERIALIZATIONPROCESSOR_H
#define CONDUCTOR_MATERIALIZATIONPROCESSOR_H

#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/MelSignalPacket.h"
#include "packet/Serializable.h"
#include "packet/DisplaySignalPacket.h"

namespace conductor {

class MaterializationProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<MelSignalPacket>> input;
    std::unique_ptr<PacketDispatcher<Serializable>> output;
    std::vector<float> previousSignal;
    std::vector<float> previousPerceptions;
    float perceptionDecay;
    int contributionRadius;

public:
    MaterializationProcessor(std::unique_ptr<PacketReceiver<MelSignalPacket>> input,
                             std::unique_ptr<PacketDispatcher<Serializable>> output);

    void process() override;
};

}

#endif //CONDUCTOR_MATERIALIZATIONPROCESSOR_H
