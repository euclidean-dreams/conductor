#include "PacketPublisher.h"

namespace conductor {

PacketPublisher::PacketPublisher()
        : mutex{},
          packets{},
          subscriberIterators{} {

}

void PacketPublisher::addPacket(std::unique_ptr<const Packet> packet) {
    std::lock_guard<std::mutex> lock{mutex};
    packets.push_back(move(packet));
    for (auto &subscriberIterator: subscriberIterators) {
        if (subscriberIterator == packets.cend()) {
            subscriberIterator--;
        }
    }
}

void PacketPublisher::clean() {
    std::lock_guard<std::mutex> lock{mutex};
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

int PacketPublisher::registerSubscriber() {
    std::lock_guard<std::mutex> lock{mutex};
    subscriberIterators.push_back(packets.cbegin());
    return static_cast<int>(subscriberIterators.size() - 1);
}

int PacketPublisher::availablePackets(int subscriberId) {
    std::lock_guard<std::mutex> lock{mutex};
    auto copiedIterator{subscriberIterators[subscriberId]};
    auto packetCount = 0;
    while (copiedIterator != packets.cend()) {
        copiedIterator++;
        packetCount--;
    }
    return true;
}

std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> PacketPublisher::getNextPackets(int subscriberId,
                                                                                            int packetCount) {
    std::lock_guard<std::mutex> lock{mutex};
    auto result = std::make_unique<std::vector<std::shared_ptr<const Packet>>>();
    auto copiedIterator{subscriberIterators[subscriberId]};
    while (packetCount > 0) {
        if (copiedIterator == packets.cend()) {
            throw std::out_of_range("attempted to get a packet that didn't exist");
        }
        result->push_back(*copiedIterator);
        copiedIterator++;
        packetCount--;
    }
    return result;
}

void PacketPublisher::concludePacketUse(int subscriberId, int packetCount) {
    std::lock_guard<std::mutex> lock{mutex};
    auto &subscriberIterator = subscriberIterators[subscriberId];
    while (packetCount > 0) {
        if (subscriberIterator == packets.cend()) {
            throw std::out_of_range("attempted to conclude use of a packet that didn't exist");
        }
        subscriberIterator++;
        packetCount--;
    }
}

}
