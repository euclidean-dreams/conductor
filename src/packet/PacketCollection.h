#ifndef CONDUCTOR_PACKETCOLLECTION_H
#define CONDUCTOR_PACKETCOLLECTION_H

#include <memory>
#include <vector>
#include <NonCopyable.h>
#include "packet/Packet.h"
#include "packet/PacketCollectionManager.h"

namespace conductor {

class PacketCollection : impresarioUtils::NonCopyable {
private:
    std::vector<std::shared_ptr<const Packet>> packets;

public:
    explicit PacketCollection();

    void addPacket(std::shared_ptr<const Packet> packet);

    const Packet &getPacket(int index);

    std::vector<std::shared_ptr<const Packet>>::const_iterator begin() const;

    std::vector<std::shared_ptr<const Packet>>::const_iterator end() const;
};

}

#endif //CONDUCTOR_PACKETCOLLECTION_H
