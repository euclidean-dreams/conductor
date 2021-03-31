#include "AudioPacket.h"

namespace conductor {

AudioPacket::AudioPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand)
        : originTimestamp{originTimestamp},
          frequencyBand{frequencyBand} {

}

uint64_t AudioPacket::getOriginTimestamp() const {
    return originTimestamp;
}

ImpresarioSerialization::FrequencyBand AudioPacket::getFrequencyBand() const {
    return frequencyBand;
}

}
