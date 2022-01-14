#include "AudioPacket.h"

namespace conductor {

AudioPacket::AudioPacket(uint64_t originTimestamp)
        : originTimestamp{originTimestamp} {

}

uint64_t AudioPacket::getOriginTimestamp() const {
    return originTimestamp;
}

}
