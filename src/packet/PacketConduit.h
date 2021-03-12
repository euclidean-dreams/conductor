#ifndef CONDUCTOR_PACKETCONDUIT_H
#define CONDUCTOR_PACKETCONDUIT_H

#include <mutex>
#include <vector>
#include <list>
#include <memory>
#include <NonCopyable.h>
#include "packet/Packet.h"

namespace conductor {

class PacketConduit : impresarioUtils::NonCopyable {
private:
    std::recursive_mutex mutex;
    std::list<std::shared_ptr<const Packet>> packets;
    std::vector<std::_List_const_iterator<std::shared_ptr<const Packet>>> spoutIterators;

    void validateSpoutId(int spoutId);

    void validatePacketsAvailable(int spoutId, int packetCount);

    void clean();

public:
    PacketConduit();

    void sendPacket(std::unique_ptr<const Packet> packet);

    int registerSpout();

    int availablePackets(int spoutId);

    std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> getPackets(int spoutId, int packetCount);

    void concludePacketUse(int spoutId, int packetCount);
};

}

#endif //CONDUCTOR_PACKETCONDUIT_H
