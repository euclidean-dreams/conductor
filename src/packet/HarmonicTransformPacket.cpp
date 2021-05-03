#include "HarmonicTransformPacket.h"

namespace conductor {

HarmonicTransformPacket::HarmonicTransformPacket(uint64_t originTimestamp,
                                                 ImpresarioSerialization::FrequencyBand frequencyBand, int size,
                                                 std::shared_ptr<const STFTPacket> stftPacket)
        : AudioPacket(originTimestamp, frequencyBand),
          stftPacket{move(stftPacket)},
          samples{},
          maxSize{size},
          addIndex{0},
          finalized{false} {
    samples.reserve(maxSize);
}

void HarmonicTransformPacket::addSample(double sample) {
    if (finalized) {
        throw std::runtime_error("attempted to add a sample to a finalized packet");
    }
    samples[addIndex] = sample;
    addIndex++;
    if (addIndex == maxSize) {
        finalized = true;
    }
}

void HarmonicTransformPacket::validateRetrieve(int index) const {
    if (!finalized) {
        throw std::runtime_error("attempted to use a packet that wasn't finalized");
    }
    if (index < 0 || index >= maxSize) {
        throw std::out_of_range("attempted to access an out of range sample");
    }
}

double HarmonicTransformPacket::getSample(int index) const {
    validateRetrieve(index);
    return samples[index];
}

const STFTPacket &HarmonicTransformPacket::getSTFTPacket() const {
    return *stftPacket;
}

int HarmonicTransformPacket::size() const {
    return maxSize;
}

}
