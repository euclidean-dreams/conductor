#ifndef CONDUCTOR_HARMONICTRANSFORMPROCESSOR_H
#define CONDUCTOR_HARMONICTRANSFORMPROCESSOR_H

#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/STFTPacket.h"
#include "packet/HarmonicTransformPacket.h"

namespace conductor {

class HarmonicTransformProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<STFTPacket>> input;
    std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output;
    std::vector<std::vector<double>> harmonicSignalSnapshots;
    std::vector<std::vector<double>> lowPhiCorrelationSignals;
    double squishFactor;
    int maxPartials;
    int considerationOffset;
    int initialPhi;
    int maxPhiDivisor;
    double harmonicSignalMagnitudeMultiplier;
    double harmonicSignalPeakDecay;
    std::shared_ptr<const STFTPacket> lastPacket;

    void initializeHarmonicSignalSnapshots();

    void initializeLowPhiHarmonicSignals();

public:
    HarmonicTransformProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                               std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output);

    void process() override;
};

}

#endif //CONDUCTOR_HARMONICTRANSFORMPROCESSOR_H
