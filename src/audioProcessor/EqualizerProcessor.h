#ifndef CONDUCTOR_EQUALIZERPROCESSOR_H
#define CONDUCTOR_EQUALIZERPROCESSOR_H

#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/STFTPacket.h"

namespace conductor {

class EqualizerProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<STFTPacket>> input;
    std::unique_ptr<PacketDispatcher<STFTPacket>> output;
    std::unique_ptr<impresarioUtils::NetworkSocket> morselSocket;
    std::map<int, float> defaultScalingFactors;
    std::map<int, float> scalingFactors;
    double sampleRate;

public:
    EqualizerProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                       std::unique_ptr<PacketDispatcher<STFTPacket>> output,
                       std::unique_ptr<impresarioUtils::NetworkSocket> morselSocket);

    void process() override;
};

}

#endif //CONDUCTOR_EQUALIZERPROCESSOR_H
