#ifndef CONDUCTOR_PACKETSUBSCRIBER_H
#define CONDUCTOR_PACKETSUBSCRIBER_H

#include <thread>
#include "Config.h"
#include "packet/PacketPublisher.h"

namespace conductor {

class PacketSubscriber {
private:
    PacketPublisher &packetPublisher;
    int subscriberId;
    int packetIncrement;

public:
    explicit PacketSubscriber(PacketPublisher &packetPublisher, int packetIncrement);

    std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> getPackets();

    void concludePacketUse();
};

}

#endif //CONDUCTOR_PACKETSUBSCRIBER_H
