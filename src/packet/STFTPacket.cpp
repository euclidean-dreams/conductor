#include "STFTPacket.h"

namespace conductor {

STFTPacket::STFTPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size,
                       int fftSize)
        : AudioPacket{originTimestamp, frequencyBand},
          signal{size},
          fftSize{fftSize} {

}

STFTPacket::STFTPacket(const STFTPacket &original)
        : AudioPacket{original.originTimestamp, original.frequencyBand},
          signal{original.size()},
          fftSize{original.fftSize} {
    for (int i = 0; i < original.size(); i++) {
        signal.addSample(original.getSample(i));
    }
}

void STFTPacket::addSample(std::complex<float> sample) {
    signal.addSample(sample);
}

std::complex<float> STFTPacket::getSample(int index) const {
    return signal.getSample(index);
}

float STFTPacket::getMagnitude(int index) const {
    auto sample = signal.getSample(index);
    return static_cast<float>(std::sqrt(std::pow(sample.real(), 2) + std::pow(sample.imag(), 2)));
}

int STFTPacket::size() const {
    return signal.size();
}

int STFTPacket::getFFTSize() const {
    return fftSize;
}

}
