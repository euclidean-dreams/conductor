#ifndef CONDUCTOR_HARMONICTRANSFORMPACKET_H
#define CONDUCTOR_HARMONICTRANSFORMPACKET_H

#include <vector>
#include "packet/AudioPacket.h"
#include "packet/STFTPacket.h"

namespace conductor {

class HarmonicTransformPacket : public AudioPacket {
private:
    std::vector<double> samples;
    std::shared_ptr<const STFTPacket> stftPacket;
    int maxSize;
    int addIndex;
    bool finalized;

    void validateRetrieve(int index) const;

public:
    HarmonicTransformPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size,
                            std::shared_ptr<const STFTPacket> stftPacket);

    void addSample(double sample);

    double getSample(int index) const;

    const STFTPacket &getSTFTPacket() const;

    int size() const;
};

}

#endif //CONDUCTOR_HARMONICTRANSFORMPACKET_H
