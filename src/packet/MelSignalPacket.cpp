#include "MelSignalPacket.h"

namespace conductor {

MelSignalPacket::MelSignalPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand,
                                 int size, std::shared_ptr<const HarmonicTransformPacket> harmonicTransformPacket)
        : AudioPacket(originTimestamp, frequencyBand),
          signal{size},
          harmonicTransformPacket{move(harmonicTransformPacket)} {

}

const HarmonicTransformPacket &MelSignalPacket::getHarmonicTransformPacket() const {
    return *harmonicTransformPacket;
}

void MelSignalPacket::addSample(double sample) {
    signal.addSample(sample);

}

double MelSignalPacket::getSample(int index) const {
    return signal.getSample(index);
}

int MelSignalPacket::size() const {
    return signal.size();
}

}
