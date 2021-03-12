#include "PacketSpout.h"

namespace conductor {

PacketSpout::PacketSpout(PacketConduit &packetConduit)
        : packetConduit{packetConduit},
          spoutId{packetConduit.registerSpout()} {

}

std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> PacketSpout::getPackets(int packetCount) {
    while (packetConduit.availablePackets(spoutId) < packetCount) {
        std::this_thread::sleep_for(std::chrono::microseconds(PACKET_SPOUT_WAKE_INTERVAL));
    }
    return packetConduit.getPackets(spoutId, packetCount);
}

std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> PacketSpout::getAllAvailablePackets() {
    auto packetCount = packetConduit.availablePackets(spoutId);
    if (packetCount > 0) {
        return packetConduit.getPackets(spoutId, packetCount);
    } else {
        return nullptr;
    }
}

void PacketSpout::concludePacketUse(std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> packets) {
    packetConduit.concludePacketUse(spoutId, packets->size());
}

}
