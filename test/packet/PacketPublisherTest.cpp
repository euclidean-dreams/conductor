#include <gtest/gtest.h>
#include "packet/PacketPublisher.h"
#include "PacketMock.h"

namespace conductor {

class PacketPublisherTest : public testing::Test {

};

TEST_F(PacketPublisherTest, RegisterSubscriberPriorToAddingPacket) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 1);
}

TEST_F(PacketPublisherTest, RegisterSubscriberAfterAddingPacket) {
    PacketPublisher packetPublisher{};

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));

    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 1);
}

TEST_F(PacketPublisherTest, RegisterSubscriberMultiple) {
    PacketPublisher packetPublisher{};
    auto subscriberId0 = packetPublisher.registerSubscriber();
    auto subscriberId1 = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 0);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 0);

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 1);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 1);
}

TEST_F(PacketPublisherTest, AvailablePacketsWithInvalidSubscriberId) {
    PacketPublisher packetPublisher{};
    EXPECT_THROW(packetPublisher.availablePackets(0), std::out_of_range);
    EXPECT_THROW(packetPublisher.availablePackets(-5), std::out_of_range);
    EXPECT_THROW(packetPublisher.availablePackets(12), std::out_of_range);
}

TEST_F(PacketPublisherTest, AvailablePacketsMultiple) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 1);

    packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 2);
}

TEST_F(PacketPublisherTest, GetPacketsWithInvalidSubscriberId) {
    PacketPublisher packetPublisher{};
    EXPECT_THROW(packetPublisher.getPackets(0, 1), std::out_of_range);
    EXPECT_THROW(packetPublisher.getPackets(-5, 1), std::out_of_range);
    EXPECT_THROW(packetPublisher.getPackets(12, 1), std::out_of_range);
}

TEST_F(PacketPublisherTest, GetPacketsWithInvalidPacketCount) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_THROW(packetPublisher.getPackets(subscriberId, 0), std::out_of_range);
    EXPECT_THROW(packetPublisher.getPackets(subscriberId, -5), std::out_of_range);
    EXPECT_THROW(packetPublisher.getPackets(subscriberId, 100), std::out_of_range);
}

TEST_F(PacketPublisherTest, GetPacketsSingle) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 1);

    packetPublisher.getPackets(subscriberId, 1);
    packetPublisher.concludePacketUse(subscriberId, 1);

    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);
}

TEST_F(PacketPublisherTest, GetPacketsMultiple) {
    auto packetCount = 5;
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();

    for (int i = 0; i < packetCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetPublisher.addPacket(move(packet));
    }
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), packetCount);

    packetPublisher.getPackets(subscriberId, packetCount);
    packetPublisher.concludePacketUse(subscriberId, packetCount);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);
}

TEST_F(PacketPublisherTest, ConcludePacketUseWithInvalidSubscriberId) {
    PacketPublisher packetPublisher{};
    EXPECT_THROW(packetPublisher.concludePacketUse(0, 1), std::out_of_range);
    EXPECT_THROW(packetPublisher.concludePacketUse(-5, 1), std::out_of_range);
    EXPECT_THROW(packetPublisher.concludePacketUse(12, 1), std::out_of_range);
}

TEST_F(PacketPublisherTest, ConcludePacketUseWithInvalidPacketCount) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_THROW(packetPublisher.concludePacketUse(subscriberId, 0), std::out_of_range);
    EXPECT_THROW(packetPublisher.concludePacketUse(subscriberId, -5), std::out_of_range);
    EXPECT_THROW(packetPublisher.concludePacketUse(subscriberId, 100), std::out_of_range);
}

TEST_F(PacketPublisherTest, CleanNoSubscribers) {
    PacketPublisher packetPublisher{};

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetPublisher.addPacket(move(packet));
    }
    packetPublisher.clean();

    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);
}

TEST_F(PacketPublisherTest, CleanSinglePacket) {
    PacketPublisher packetPublisher{};
    auto subscriberId0 = packetPublisher.registerSubscriber();

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));

    packetPublisher.getPackets(subscriberId0, 1);
    packetPublisher.concludePacketUse(subscriberId0, 1);

    packetPublisher.clean();

    auto subscriberId1 = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 0);
}

TEST_F(PacketPublisherTest, CleanMultiplePackets) {
    PacketPublisher packetPublisher{};
    auto subscriberId0 = packetPublisher.registerSubscriber();

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetPublisher.addPacket(move(packet));
    }

    packetPublisher.getPackets(subscriberId0, 3);
    packetPublisher.concludePacketUse(subscriberId0, 3);

    packetPublisher.clean();

    auto subscriberId1 = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 2);
}

TEST_F(PacketPublisherTest, CleanMultiplePacketsMultipleSubscribers) {
    PacketPublisher packetPublisher{};
    auto subscriberId0 = packetPublisher.registerSubscriber();
    auto subscriberId1 = packetPublisher.registerSubscriber();

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetPublisher.addPacket(move(packet));
    }

    packetPublisher.getPackets(subscriberId0, 4);
    packetPublisher.concludePacketUse(subscriberId0, 4);

    packetPublisher.getPackets(subscriberId1, 3);
    packetPublisher.concludePacketUse(subscriberId1, 3);

    packetPublisher.clean();

    auto subscriberId2 = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId2), 2);
}

TEST_F(PacketPublisherTest, MultipleSubscribersMultiplePackets) {
    PacketPublisher packetPublisher{};
    auto subscriberId0 = packetPublisher.registerSubscriber();
    auto subscriberId1 = packetPublisher.registerSubscriber();

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetPublisher.addPacket(move(packet));
    }
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), initialPacketCount);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), initialPacketCount);

    packetPublisher.getPackets(subscriberId0, 2);
    packetPublisher.concludePacketUse(subscriberId0, 2);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), initialPacketCount);

    packetPublisher.getPackets(subscriberId1, 4);
    packetPublisher.concludePacketUse(subscriberId1, 4);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 1);

    packetPublisher.getPackets(subscriberId1, 1);
    packetPublisher.concludePacketUse(subscriberId1, 1);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 0);

    packetPublisher.getPackets(subscriberId0, 3);
    packetPublisher.concludePacketUse(subscriberId0, 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 0);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 0);
}

}
