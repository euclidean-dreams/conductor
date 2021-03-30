#include "PacketSpout.h"

namespace conductor {

PacketSpout::PacketSpout(PacketConduit &packetConduit)
        : packetConduit{packetConduit},
          spoutId{packetConduit.registerSpout()} {

}

std::shared_ptr<const Packet> PacketSpout::getPacket() {
    packetConduit.waitForAvailablePackets(spoutId, 1);
    return packetConduit.getPacket(spoutId);
}

std::unique_ptr<PacketCollection> PacketSpout::getPackets(int packetCount) {
    packetConduit.waitForAvailablePackets(spoutId, packetCount);
    return packetConduit.getPackets(spoutId, packetCount);
}

}
