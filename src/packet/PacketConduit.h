#ifndef CONDUCTOR_PACKETCONDUIT_H
#define CONDUCTOR_PACKETCONDUIT_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <memory>
#include "packet/Packet.h"
#include "packet/PacketCollection.h"
#include "packet/Cleanable.h"

namespace conductor {

template<class T>
class PacketConduit : public Cleanable {
private:
    std::mutex mutex;
    std::condition_variable newPacketAddedExpectant;
    std::list<std::shared_ptr<const T>> packets;
    std::vector<std::_List_const_iterator<std::shared_ptr<const T>>> receiverIterators;

    void validateReceiverId(int receiverId) {
        if (receiverId < 0 || receiverId >= receiverIterators.size() || receiverIterators.empty()) {
            throw std::out_of_range("receiver id does not exist");
        }
    }

    void validatePacketsAvailable(int receiverId, int packetCount) {
        if (packetCount < 1 || packetCount > availablePackets(receiverId)) {
            throw std::out_of_range("requested invalid number of packets");
        }
    }

public:
    PacketConduit()
            : mutex{},
              newPacketAddedExpectant{},
              packets{},
              receiverIterators{} {

    }

    void sendPacket(std::unique_ptr<const T> packet) {
        std::lock_guard<std::mutex> lock{mutex};
        packets.push_back(move(packet));
        for (auto &receiverIterator: receiverIterators) {
            if (receiverIterator == packets.cend()) {
                receiverIterator--;
            }
        }
        newPacketAddedExpectant.notify_all();
    }

    int registerReceiver() {
        std::lock_guard<std::mutex> lock{mutex};
        receiverIterators.push_back(packets.cbegin());
        return static_cast<int>(receiverIterators.size() - 1);
    }

    int availablePackets(int receiverId) {
        validateReceiverId(receiverId);
        auto copiedIterator{receiverIterators[receiverId]};
        auto packetCount = 0;
        while (copiedIterator != packets.cend()) {
            copiedIterator++;
            packetCount++;
        }
        return packetCount;
    }

    void waitForAvailablePackets(int receiverId, int packetCount) {
        std::unique_lock<std::mutex> lock{mutex};
        while (availablePackets(receiverId) < packetCount) {
            newPacketAddedExpectant.wait(lock);
        }
    }

    std::shared_ptr<const T> getPacket(int receiverId) {
        std::lock_guard<std::mutex> lock{mutex};
        validateReceiverId(receiverId);
        validatePacketsAvailable(receiverId, 1);
        auto &receiverIterator = receiverIterators[receiverId];
        auto result = *receiverIterator;
        receiverIterator++;
        return result;
    }

    std::unique_ptr<PacketCollection<T>> getPackets(int receiverId, int packetCount) {
        std::lock_guard<std::mutex> lock{mutex};
        validateReceiverId(receiverId);
        validatePacketsAvailable(receiverId, packetCount);
        auto result = std::make_unique<PacketCollection<T>>();
        auto &receiverIterator = receiverIterators[receiverId];
        while (packetCount > 0) {
            result->addPacket(*receiverIterator);
            receiverIterator++;
            packetCount--;
        }
        return result;
    }

    void clean() override {
        std::lock_guard<std::mutex> lock{mutex};
        bool shouldRemovePacket = true;
        while (shouldRemovePacket) {
            if (packets.front().use_count() == 1) {
                for (auto &receiverIterator: receiverIterators) {
                    if (receiverIterator == packets.cbegin()) {
                        shouldRemovePacket = false;
                    }
                }
            } else {
                shouldRemovePacket = false;
            }
            if (shouldRemovePacket) {
                packets.pop_front();
            }
        }
    }
};

}

#endif //CONDUCTOR_PACKETCONDUIT_H
