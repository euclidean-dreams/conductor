#ifndef CONDUCTOR_DISPLAYSIGNALPACKET_H
#define CONDUCTOR_DISPLAYSIGNALPACKET_H

#include <ImpresarioSerialization.h>
#include "packet/AudioPacket.h"
#include "packet/Serializable.h"
#include "packet/Signal.h"

namespace conductor {

class DisplaySignalPacket : public AudioPacket, public Serializable {
private:
    Signal<float> signal;

public:
    DisplaySignalPacket(uint64_t originTimestamp, int size);

    std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const override;

    ImpresarioSerialization::Identifier getIdentifier() const override;

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_DISPLAYSIGNALPACKET_H
