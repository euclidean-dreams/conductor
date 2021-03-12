#include "PacketPublisher.h"

namespace conductor {

PacketPublisher::PacketPublisher()
        : mutex{},
          packets{},
          subscriberIterators{} {

}

void PacketPublisher::validateSubscriberId(int subscriberId) {
    if (subscriberId < 0 || subscriberId >= subscriberIterators.size() || subscriberIterators.empty()) {
        throw std::out_of_range("subscriber id does not exist");
    }
}

void PacketPublisher::validatePacketsAvailable(int subscriberId, int packetCount) {
    if (packetCount < 1 || packetCount > availablePackets(subscriberId)) {
        throw std::out_of_range("requested invalid number of packets");
    }
}

void PacketPublisher::addPacket(std::unique_ptr<const Packet> packet) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    packets.push_back(move(packet));
    for (auto &subscriberIterator: subscriberIterators) {
        if (subscriberIterator == packets.cend()) {
            subscriberIterator--;
        }
    }
}

void PacketPublisher::clean() {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    if (subscriberIterators.empty()) {
        packets.clear();
    } else {
        bool shouldRemovePacket = true;
        while (shouldRemovePacket) {
            for (auto &subscriberIterator: subscriberIterators) {
                if (subscriberIterator == packets.cbegin()) {
                    shouldRemovePacket = false;
                }
            }
            if (shouldRemovePacket) {
                packets.pop_front();
            }
        }
    }
}

int PacketPublisher::registerSubscriber() {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    subscriberIterators.push_back(packets.cbegin());
    return static_cast<int>(subscriberIterators.size() - 1);
}

int PacketPublisher::availablePackets(int subscriberId) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    validateSubscriberId(subscriberId);
    auto copiedIterator{subscriberIterators[subscriberId]};
    auto packetCount = 0;
    while (copiedIterator != packets.cend()) {
        copiedIterator++;
        packetCount++;
    }
    return packetCount;
}

std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> PacketPublisher::getPackets(int subscriberId,
                                                                                        int packetCount) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    validateSubscriberId(subscriberId);
    validatePacketsAvailable(subscriberId, packetCount);
    auto result = std::make_unique<std::vector<std::shared_ptr<const Packet>>>();
    auto copiedIterator{subscriberIterators[subscriberId]};
    while (packetCount > 0) {
        result->push_back(*copiedIterator);
        copiedIterator++;
        packetCount--;
    }
    return result;
}

void PacketPublisher::concludePacketUse(int subscriberId, int packetCount) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    validateSubscriberId(subscriberId);
    validatePacketsAvailable(subscriberId, packetCount);
    auto &subscriberIterator = subscriberIterators[subscriberId];
    while (packetCount > 0) {
        subscriberIterator++;
        packetCount--;
    }
}

}
