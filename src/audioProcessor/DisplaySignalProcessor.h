#ifndef CONDUCTOR_DISPLAYSIGNALPROCESSOR_H
#define CONDUCTOR_DISPLAYSIGNALPROCESSOR_H

#include "audioProcessor/AudioProcessor.h"
#include "packet/Serializable.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/HarmonicTransformPacket.h"
#include "packet/HarmonicTransformPacket.h"

namespace conductor {

class DisplaySignalProcessor: public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<STFTPacket>> input;
    std::unique_ptr<PacketDispatcher<Serializable>> output;
    std::vector<std::vector<double>> harmonicSignalSnapshots;
    std::vector<std::vector<double>> lowPhiCorrelationSignals;

};

}

#endif //CONDUCTOR_DISPLAYSIGNALPROCESSOR_H
