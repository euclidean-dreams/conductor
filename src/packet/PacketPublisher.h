#ifndef CONDUCTOR_PACKETPUBLISHER_H
#define CONDUCTOR_PACKETPUBLISHER_H

#include <mutex>
#include <vector>
#include <list>
#include <memory>
#include <NonCopyable.h>
#include "packet/Packet.h"

namespace conductor {

class PacketPublisher : impresarioUtils::NonCopyable {
private:
    std::recursive_mutex mutex;
    std::list<std::shared_ptr<const Packet>> packets;
    std::vector<std::_List_const_iterator<std::shared_ptr<const Packet>>> subscriberIterators;

    void validateSubscriberId(int subscriberId);

    void validatePacketsAvailable(int subscriberId, int packetCount);

public:
    PacketPublisher();

    void addPacket(std::unique_ptr<const Packet> packet);

    void clean();

    int registerSubscriber();

    int availablePackets(int subscriberId);

    std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> getPackets(int subscriberId, int packetCount);

    void concludePacketUse(int subscriberId, int packetCount);
};

}

#endif //CONDUCTOR_PACKETPUBLISHER_H
