#ifndef CONDUCTOR_PACKETSPOUT_H
#define CONDUCTOR_PACKETSPOUT_H

#include <thread>
#include "packet/PacketConduit.h"
#include "packet/PacketCollectionManager.h"

namespace conductor {

class PacketSpout : public PacketCollectionManager {
private:
    PacketConduit &packetConduit;
    int spoutId;

public:
    explicit PacketSpout(PacketConduit &packetConduit);

    std::shared_ptr<const Packet> getPacket();

    std::unique_ptr<PacketCollection> getPackets(int packetCount);
};

}

#endif //CONDUCTOR_PACKETSPOUT_H
