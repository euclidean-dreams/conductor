#ifndef CONDUCTOR_MELFILTERBANKPROCESSOR_H
#define CONDUCTOR_MELFILTERBANKPROCESSOR_H

#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "packet/HarmonicTransformPacket.h"
#include "packet/MelSignalPacket.h"

namespace conductor {

class MelFilterbankProcessor : public AudioProcessor {
private:
    inline static const int FILTERBANK_COUNT = 74 * 5;
    inline static const int FILTERBANK_MAX_FREQUENCY_DENOMINATOR = 32;

    std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input;
    std::unique_ptr<PacketDispatcher<MelSignalPacket>> output;
    std::vector<int> filterbankCenters;
    int filterbankCount;
    int filterbankMaxFrequencyDenominator;
    double sampleRate;

    void calculateFilterbankCenters(int fftSize);

    static double convertHzToMel(double value);

    static double convertMelToHz(double value);

    static int convertHzToBin(double value, int fftSize, double sampleRate);

public:
    MelFilterbankProcessor(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                           std::unique_ptr<PacketDispatcher<MelSignalPacket>> output);

    void process() override;
};

}

#endif //CONDUCTOR_MELFILTERBANKPROCESSOR_H
