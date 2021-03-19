#ifndef CONDUCTOR_BANDPASSPROCESSOR_H
#define CONDUCTOR_BANDPASSPROCESSOR_H

#include <map>
#include <Iir.h>
#include <FrequencyBand_generated.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/RawAudioPacket.h"
#include "packet/PacketConduit.h"
#include "packet/PacketSpout.h"

namespace conductor {

class BandpassProcessor : public AudioProcessor {
private:
    inline static const int ORDER = 10;
    inline static const std::map<ImpresarioSerialization::FrequencyBand, std::pair<float, float>> FREQUENCY_BAND_VALUES = {
            {ImpresarioSerialization::FrequencyBand::subBass,       {20,   60}},
            {ImpresarioSerialization::FrequencyBand::bass,          {60,   250}},
            {ImpresarioSerialization::FrequencyBand::lowMidrange,   {250,  500}},
            {ImpresarioSerialization::FrequencyBand::midrange,      {500,  2000}},
            {ImpresarioSerialization::FrequencyBand::upperMidrange, {2000, 4000}},
            {ImpresarioSerialization::FrequencyBand::presence,      {4000, 6000}},
            {ImpresarioSerialization::FrequencyBand::brilliance,    {6000, 20000}}
    };

    std::unique_ptr<PacketSpout> input;
    std::unique_ptr<PacketConduit> output;
    ImpresarioSerialization::FrequencyBand frequencyBand;
    Iir::Butterworth::BandPass<ORDER> filter;

public:
    BandpassProcessor(std::unique_ptr<PacketSpout> input, std::unique_ptr<PacketConduit> output,
                      ImpresarioSerialization::FrequencyBand frequencyBand);

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_BANDPASSPROCESSOR_H
