#ifndef CONDUCTOR_AUDIOPACKET_H
#define CONDUCTOR_AUDIOPACKET_H

#include <stdexcept>
#include <vector>
#include "packet/Packet.h"

namespace conductor {

class AudioPacket : public Packet {
private:
    std::vector<float> data;
    int maxSize;
    int addIndex;
    bool finalized;

public:
    static const AudioPacket &from(const Packet &packet);

    explicit AudioPacket(int size);

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_AUDIOPACKET_H
