#ifndef CONDUCTOR_STFTPACKET_H
#define CONDUCTOR_STFTPACKET_H

#include <cmath>
#include <vector>
#include "packet/AudioPacket.h"
#include "packet/Serializable.h"

namespace conductor {

class STFTPacket : public AudioPacket {
private:
    std::vector<float> real;
    std::vector<float> imaginary;
    int maxSize;
    int addIndex;
    bool finalized;

    void validateRetrieve(int index) const;

public:
    STFTPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size);

    STFTPacket(const STFTPacket &original);

    void addSample(float realPart, float imaginaryPart);

    float getRealPart(int index) const;

    float getImaginaryPart(int index) const;

    float getMagnitude(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_STFTPACKET_H
