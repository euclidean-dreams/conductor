#ifndef CONDUCTOR_HARMONICTRANSFORMPACKET_H
#define CONDUCTOR_HARMONICTRANSFORMPACKET_H

#include "packet/AudioPacket.h"
#include "packet/STFTPacket.h"
#include "packet/Signal.h"

namespace conductor {

class HarmonicTransformPacket : public AudioPacket {
private:
    Signal<double> signal;
    std::shared_ptr<const STFTPacket> stftPacket;

public:
    HarmonicTransformPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size,
                            std::shared_ptr<const STFTPacket> stftPacket);

    const STFTPacket &getSTFTPacket() const;

    void addSample(double sample);

    double getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_HARMONICTRANSFORMPACKET_H
