#ifndef CONDUCTOR_DISPLAYSIGNALPROCESSOR_H
#define CONDUCTOR_DISPLAYSIGNALPROCESSOR_H

#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/Serializable.h"
#include "packet/MelSignalPacket.h"
#include "packet/DisplaySignalPacket.h"

namespace conductor {

class DisplaySignalProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<MelSignalPacket>> input;
    std::unique_ptr<PacketDispatcher<Serializable>> output;

public:
    DisplaySignalProcessor(std::unique_ptr<PacketReceiver<MelSignalPacket>> input,
                           std::unique_ptr<PacketDispatcher<Serializable>> output);

    void process() override;
};

}

#endif //CONDUCTOR_DISPLAYSIGNALPROCESSOR_H
