#define _GLIBCXX_USE_CXX11_ABI 0

#include <gtest/gtest.h>
#include <Isolator.h>
#include "packet/PacketPublisher.h"

namespace conductor {

class PacketPublisherTest : public testing::Test {
    void TearDown() override {
        ISOLATOR_CLEANUP();
    }

};

TEST_F(PacketPublisherTest, RegisterReceiverPriorToAddingPacket) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_FALSE(packetPublisher.nextPacketsAreReady(subscriberId, 1));

    auto packet = std::unique_ptr<Packet>(FAKE<Packet>());
    packetPublisher.addPacket(move(packet));

    EXPECT_TRUE(packetPublisher.nextPacketsAreReady(subscriberId, 1));
}

//TEST_F(PacketPublisherTest, RegisterReceiverAfterAddingAudioPacket) {
//    PacketPublisher packetPublisher{};
//
//    auto audioPacket = createFinalizedAudioPacket();
//    packetPublisher.addPacket(move(audioPacket));
//
//    auto subscriberId = packetPublisher.registerSubscriber();
//    EXPECT_TRUE(packetPublisher.nextPacketsAreReady(subscriberId, 1));
//}
//
//TEST_F(PacketPublisherTest, RegisterMultipleReceiversAtDifferentTimes) {
//    PacketPublisher packetPublisher{};
//    auto subscriberId0 = packetPublisher.registerSubscriber();
//    EXPECT_FALSE(packetPublisher.nextPacketsAreReady(subscriberId0, 1));
//
//    auto audioPacket = createFinalizedAudioPacket();
//    packetPublisher.addPacket(move(audioPacket));
//
//    auto subscriberId1 = packetPublisher.registerSubscriber();
//    EXPECT_TRUE(packetPublisher.nextPacketsAreReady(subscriberId0, 1));
//    EXPECT_TRUE(packetPublisher.nextPacketsAreReady(subscriberId1, 1));
//}

}
