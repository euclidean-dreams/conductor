#ifndef CONDUCTOR_MELSIGNALPACKET_H
#define CONDUCTOR_MELSIGNALPACKET_H

#include "packet/AudioPacket.h"
#include "packet/HarmonicTransformPacket.h"
#include "packet/Signal.h"

namespace conductor {

class MelSignalPacket : public AudioPacket {
private:
    Signal<double> signal;
    std::shared_ptr<const HarmonicTransformPacket> harmonicTransformPacket;

public:
    MelSignalPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size,
                    std::shared_ptr<const HarmonicTransformPacket> harmonicTransformPacket);

    const HarmonicTransformPacket &getHarmonicTransformPacket() const;

    void addSample(double sample);

    double getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_MELSIGNALPACKET_H
