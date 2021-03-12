#ifndef CONDUCTOR_PACKETSPOUT_H
#define CONDUCTOR_PACKETSPOUT_H

#include <thread>
#include "Config.h"
#include "packet/PacketConduit.h"

namespace conductor {

class PacketSpout {
private:
    PacketConduit &packetConduit;
    int spoutId;

public:
    explicit PacketSpout(PacketConduit &packetConduit);

    std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> getPackets(int packetCount);

    std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> getAllAvailablePackets();

    void concludePacketUse(std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> packets);
};

}

#endif //CONDUCTOR_PACKETSPOUT_H
