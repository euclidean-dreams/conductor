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

    std::unique_ptr<PacketCollection> getPackets(int packetCount);

    void concludePacketCollectionUse(int size) override;
};

}

#endif //CONDUCTOR_PACKETSPOUT_H
