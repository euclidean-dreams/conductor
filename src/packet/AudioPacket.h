#ifndef CONDUCTOR_AUDIOPACKET_H
#define CONDUCTOR_AUDIOPACKET_H

#include <array>
#include "Config.h"
#include "packet/Packet.h"

namespace conductor {

class AudioPacket : public Packet {
private:
    std::array<float, AUDIO_PACKET_SIZE> data;
    int addIndex;
    bool finalized;

public:
    static const AudioPacket &from(const Packet &packet);

    AudioPacket();

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_AUDIOPACKET_H
