#include "HarmonicTransformPacket.h"

namespace conductor {

HarmonicTransformPacket::HarmonicTransformPacket(uint64_t originTimestamp,
                                                 ImpresarioSerialization::FrequencyBand frequencyBand, int size,
                                                 std::shared_ptr<const STFTPacket> stftPacket)
        : AudioPacket(originTimestamp, frequencyBand),
          signal{size},
          stftPacket{move(stftPacket)} {

}

const STFTPacket &HarmonicTransformPacket::getSTFTPacket() const {
    return *stftPacket;
}

void HarmonicTransformPacket::addSample(double sample) {
    signal.addSample(sample);
}

double HarmonicTransformPacket::getSample(int index) const {
    return signal.getSample(index);
}

int HarmonicTransformPacket::size() const {
    return signal.size();
}

}
