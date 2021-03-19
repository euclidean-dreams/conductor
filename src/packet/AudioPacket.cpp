#include "AudioPacket.h"

namespace conductor {

AudioPacket::AudioPacket(uint64_t sampleTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand)
        : sampleTimestamp{sampleTimestamp},
          frequencyBand{frequencyBand} {

}

uint64_t AudioPacket::getSampleTimestamp() const {
    return sampleTimestamp;
}

ImpresarioSerialization::FrequencyBand AudioPacket::getFrequencyBand() const {
    return frequencyBand;
}

}
