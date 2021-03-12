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

TEST_F(PacketPublisherTest, RegisterMultipleSubscribers) {
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

TEST_F(PacketPublisherTest, MultiplePacketsAvailable) {
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

TEST_F(PacketPublisherTest, GetSinglePacket) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 1);

    packetPublisher.getNextPackets(subscriberId, 1);
    packetPublisher.concludePacketUse(subscriberId, 1);

    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);
}

TEST_F(PacketPublisherTest, GetMultiplePackets) {
    auto packetCount = 5;
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();

    for (int i = 0; i < packetCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetPublisher.addPacket(move(packet));
    }
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), packetCount);

    packetPublisher.getNextPackets(subscriberId, packetCount);
    packetPublisher.concludePacketUse(subscriberId, packetCount);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);
}

TEST_F(PacketPublisherTest, GetTooManyPackets) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));

    EXPECT_THROW(packetPublisher.getNextPackets(subscriberId, 2), std::out_of_range);
}

TEST_F(PacketPublisherTest, ConcludeTooManyPackets) {
    PacketPublisher packetPublisher{};
    auto subscriberId = packetPublisher.registerSubscriber();

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));

    EXPECT_THROW(packetPublisher.concludePacketUse(subscriberId, 2), std::out_of_range);
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

    packetPublisher.getNextPackets(subscriberId0, 2);
    packetPublisher.concludePacketUse(subscriberId0, 2);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), initialPacketCount);

    packetPublisher.getNextPackets(subscriberId1, 4);
    packetPublisher.concludePacketUse(subscriberId1, 4);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 1);

    packetPublisher.getNextPackets(subscriberId1, 1);
    packetPublisher.concludePacketUse(subscriberId1, 1);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 0);

    packetPublisher.getNextPackets(subscriberId0, 3);
    packetPublisher.concludePacketUse(subscriberId0, 3);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId0), 0);
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId1), 0);
}

TEST_F(PacketPublisherTest, CleanNoSubscribers) {
    PacketPublisher packetPublisher{};

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));

    packetPublisher.clean();

    auto subscriberId = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId), 0);
}

TEST_F(PacketPublisherTest, CleanSinglePacket) {
    PacketPublisher packetPublisher{};
    auto subscriberId0 = packetPublisher.registerSubscriber();

    auto packet = std::make_unique<PacketMock>();
    packetPublisher.addPacket(move(packet));

    packetPublisher.getNextPackets(subscriberId0, 1);
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

    packetPublisher.getNextPackets(subscriberId0, 3);
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

    packetPublisher.getNextPackets(subscriberId0, 4);
    packetPublisher.concludePacketUse(subscriberId0, 4);

    packetPublisher.getNextPackets(subscriberId0, 3);
    packetPublisher.concludePacketUse(subscriberId0, 3);

    packetPublisher.clean();

    auto subscriberId2 = packetPublisher.registerSubscriber();
    EXPECT_EQ(packetPublisher.availablePackets(subscriberId2), 2);
}

}
