#include "PacketSubscriber.h"

namespace conductor {

PacketSubscriber::PacketSubscriber(PacketPublisher &packetPublisher, int packetIncrement)
        : packetPublisher{packetPublisher},
          subscriberId{packetPublisher.registerSubscriber()},
          packetIncrement{packetIncrement} {

}

std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> PacketSubscriber::getPackets() {
    while (packetPublisher.availablePackets(subscriberId) < packetIncrement) {
        std::this_thread::sleep_for(std::chrono::microseconds(PACKET_SUBSCRIBER_QUERY_INTERVAL));
    }
    return packetPublisher.getPackets(subscriberId, packetIncrement);
}

void PacketSubscriber::concludePacketUse() {
    packetPublisher.concludePacketUse(subscriberId, packetIncrement);
}

}
