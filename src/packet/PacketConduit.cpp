#include "PacketConduit.h"

namespace conductor {

PacketConduit::PacketConduit()
        : mutex{},
          newPacketAddedExpectant{},
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
    std::lock_guard<std::mutex> lock{mutex};
    packets.push_back(move(packet));
    for (auto &spoutIterator: spoutIterators) {
        if (spoutIterator == packets.cend()) {
            spoutIterator--;
        }
    }
    newPacketAddedExpectant.notify_all();
}

int PacketConduit::registerSpout() {
    std::lock_guard<std::mutex> lock{mutex};
    spoutIterators.push_back(packets.cbegin());
    return static_cast<int>(spoutIterators.size() - 1);
}

int PacketConduit::availablePackets(int spoutId) {
    validateSpoutId(spoutId);
    auto copiedIterator{spoutIterators[spoutId]};
    auto packetCount = 0;
    while (copiedIterator != packets.cend()) {
        copiedIterator++;
        packetCount++;
    }
    return packetCount;
}

void PacketConduit::waitForAvailablePackets(int spoutId, int packetCount) {
    std::unique_lock<std::mutex> lock{mutex};
    while (availablePackets(spoutId) < packetCount) {
        newPacketAddedExpectant.wait(lock);
    }
}

std::unique_ptr<PacketCollection> PacketConduit::getPackets(int spoutId, int packetCount,
                                                            PacketCollectionManager &packetCollectionManager) {
    validateSpoutId(spoutId);
    validatePacketsAvailable(spoutId, packetCount);
    auto result = std::make_unique<PacketCollection>(packetCollectionManager);
    auto copiedIterator{spoutIterators[spoutId]};
    while (packetCount > 0) {
        result->addPacket(*copiedIterator);
        copiedIterator++;
        packetCount--;
    }
    return result;
}

void PacketConduit::concludePacketUse(int spoutId, int packetCount) {
    std::lock_guard<std::mutex> lock{mutex};
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
