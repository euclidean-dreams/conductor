#ifndef CONDUCTOR_MELFILTERBANKPROCESSOR_H
#define CONDUCTOR_MELFILTERBANKPROCESSOR_H

#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/HarmonicTransformPacket.h"
#include "packet/MelSignalPacket.h"

namespace conductor {

class MelFilterBankProcessor : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input;
    std::unique_ptr<PacketDispatcher<MelSignalPacket>> output;
    std::vector<int> filterBankCenters;
    int filterCount;
    int maxFrequency;
    double sampleRate;

    void calculateFilterBankCenters(int fftSize);

    static double convertHzToMel(double value);

    static double convertMelToHz(double value);

    static int convertHzToBin(double value, int fftSize, double sampleRate);

public:
    MelFilterBankProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                           std::unique_ptr<PacketDispatcher<MelSignalPacket>> output,
                           int filterCount);

    void process() override;
};

}

#endif //CONDUCTOR_MELFILTERBANKPROCESSOR_H
