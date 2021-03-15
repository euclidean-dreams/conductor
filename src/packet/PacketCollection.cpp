#include "PacketCollection.h"

namespace conductor {

PacketCollection::PacketCollection(PacketCollectionManager &packetCollectionManager)
        : packetCollectionManager{packetCollectionManager},
          packets{} {

}

void PacketCollection::addPacket(std::shared_ptr<const Packet> packet) {
    packets.push_back(move(packet));
}

const Packet &PacketCollection::getPacket(int index) {
    return *packets[index];
}

void PacketCollection::concludeUse() {
    packetCollectionManager.concludePacketCollectionUse(packets.size());
}

std::vector<std::shared_ptr<const Packet>>::const_iterator PacketCollection::begin() const {
    return packets.cbegin();
}

std::vector<std::shared_ptr<const Packet>>::const_iterator PacketCollection::end() const {
    return packets.cend();
}

}
