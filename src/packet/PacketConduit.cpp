#include "PacketConduit.h"

namespace conductor {

PacketConduit::PacketConduit()
        : mutex{},
          packets{},
          spoutIterators{} {

}

void PacketConduit::validateSpoutId(int spoutId) {
    if (spoutId < 0 || spoutId >= spoutIterators.size() || spoutIterators.empty()) {
        throw std::out_of_range("spout id does not exist");
    }
}

void PacketConduit::validatePacketsAvailable(int spoutId, int packetCount) {
    if (packetCount < 1 || packetCount > availablePackets(spoutId)) {
        throw std::out_of_range("requested invalid number of packets");
    }
}

void PacketConduit::clean() {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    bool shouldRemovePacket = true;
    while (shouldRemovePacket) {
        for (auto &spoutIterator: spoutIterators) {
            if (spoutIterator == packets.cbegin()) {
                shouldRemovePacket = false;
            }
        }
        if (shouldRemovePacket) {
            packets.pop_front();
        }
    }
}

void PacketConduit::sendPacket(std::unique_ptr<const Packet> packet) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    packets.push_back(move(packet));
    for (auto &spoutIterator: spoutIterators) {
        if (spoutIterator == packets.cend()) {
            spoutIterator--;
        }
    }
}

int PacketConduit::registerSpout() {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    spoutIterators.push_back(packets.cbegin());
    return static_cast<int>(spoutIterators.size() - 1);
}

int PacketConduit::availablePackets(int spoutId) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    validateSpoutId(spoutId);
    auto copiedIterator{spoutIterators[spoutId]};
    auto packetCount = 0;
    while (copiedIterator != packets.cend()) {
        copiedIterator++;
        packetCount++;
    }
    return packetCount;
}

std::unique_ptr<std::vector<std::shared_ptr<const Packet>>> PacketConduit::getPackets(int spoutId,
                                                                                      int packetCount) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    validateSpoutId(spoutId);
    validatePacketsAvailable(spoutId, packetCount);
    auto result = std::make_unique<std::vector<std::shared_ptr<const Packet>>>();
    auto copiedIterator{spoutIterators[spoutId]};
    while (packetCount > 0) {
        result->push_back(*copiedIterator);
        copiedIterator++;
        packetCount--;
    }
    return result;
}

void PacketConduit::concludePacketUse(int spoutId, int packetCount) {
    std::lock_guard<std::recursive_mutex> lock{mutex};
    validateSpoutId(spoutId);
    validatePacketsAvailable(spoutId, packetCount);
    auto &spoutIterator = spoutIterators[spoutId];
    while (packetCount > 0) {
        spoutIterator++;
        packetCount--;
    }
    clean();
}

}
