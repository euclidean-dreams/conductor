#include "PacketSpout.h"

namespace conductor {

PacketSpout::PacketSpout(PacketConduit &packetConduit)
        : packetConduit{packetConduit},
          spoutId{packetConduit.registerSpout()} {

}

std::unique_ptr<PacketCollection> PacketSpout::getPackets(int packetCount) {
    packetConduit.waitForAvailablePackets(spoutId, packetCount);
    return packetConduit.getPackets(spoutId, packetCount, *this);
}

void PacketSpout::concludePacketCollectionUse(int size) {
    packetConduit.concludePacketUse(spoutId, size);
}

}
