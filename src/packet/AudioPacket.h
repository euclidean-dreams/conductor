#ifndef CONDUCTOR_AUDIOPACKET_H
#define CONDUCTOR_AUDIOPACKET_H

#include <stdexcept>
#include <vector>
#include <Time.h>
#include "packet/Packet.h"
#include "packet/NotSerializableException.h"

namespace conductor {

class AudioPacket : public Packet {
private:
    std::vector<float> data;
    int maxSize;
    int addIndex;
    bool finalized;
    uint64_t timestamp;

public:
    static const AudioPacket &from(const Packet &packet);

    explicit AudioPacket(int size);

    std::unique_ptr<zmq::multipart_t> serialize() const override;

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;

    uint64_t getTimestamp() const;
};

}

#endif //CONDUCTOR_AUDIOPACKET_H
