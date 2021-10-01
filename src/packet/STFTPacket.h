#ifndef CONDUCTOR_STFTPACKET_H
#define CONDUCTOR_STFTPACKET_H

#include <complex>
#include "packet/AudioPacket.h"
#include "packet/Signal.h"

namespace conductor {

class STFTPacket : public AudioPacket {
private:
    Signal<std::complex<float>> signal;
    int fftSize;

public:
    STFTPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size, int fftSize);

    STFTPacket(const STFTPacket &original);

    void addSample(std::complex<float> sample);

    std::complex<float> getSample(int index) const;

    float getMagnitude(int index) const;

    int size() const;

    int getFFTSize() const;
};

}

#endif //CONDUCTOR_STFTPACKET_H
