#include <gtest/gtest.h>
#include "packet/PacketConduit.h"
#include "PacketMock.h"
#include "PacketCollectionManagerMock.h"

namespace conductor {

class PacketConduitTest : public testing::Test {

};

TEST_F(PacketConduitTest, RegisterSpoutPriorToAddingPacket) {
    PacketConduit packetConduit{};
    auto spoutId = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 0);

    auto packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 1);
}

TEST_F(PacketConduitTest, RegisterSpoutAfterAddingPacket) {
    PacketConduit packetConduit{};

    auto packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));

    auto spoutId = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 1);
}

TEST_F(PacketConduitTest, RegisterSpoutMultiple) {
    PacketConduit packetConduit{};
    auto spoutId0 = packetConduit.registerReceiver();
    auto spoutId1 = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), 0);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 0);

    auto packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), 1);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 1);
}

TEST_F(PacketConduitTest, AvailablePacketsWithInvalidSpoutId) {
    PacketConduit packetConduit{};
    EXPECT_THROW(packetConduit.availablePackets(0), std::out_of_range);
    EXPECT_THROW(packetConduit.availablePackets(-5), std::out_of_range);
    EXPECT_THROW(packetConduit.availablePackets(12), std::out_of_range);
}

TEST_F(PacketConduitTest, AvailablePacketsMultiple) {
    PacketConduit packetConduit{};
    auto spoutId = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 0);

    auto packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 1);

    packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 2);
}

TEST_F(PacketConduitTest, GetPacketsWithInvalidSpoutId) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    EXPECT_THROW(packetConduit.getPackets(0, 1, packetCollectionManagerMock), std::out_of_range);
    EXPECT_THROW(packetConduit.getPackets(-5, 1, packetCollectionManagerMock), std::out_of_range);
    EXPECT_THROW(packetConduit.getPackets(12, 1, packetCollectionManagerMock), std::out_of_range);
}

TEST_F(PacketConduitTest, GetPacketsWithInvalidPacketCount) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId = packetConduit.registerReceiver();
    EXPECT_THROW(packetConduit.getPackets(spoutId, 0, packetCollectionManagerMock), std::out_of_range);
    EXPECT_THROW(packetConduit.getPackets(spoutId, -5, packetCollectionManagerMock), std::out_of_range);
    EXPECT_THROW(packetConduit.getPackets(spoutId, 100, packetCollectionManagerMock), std::out_of_range);
}

TEST_F(PacketConduitTest, GetPacketsSingle) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId = packetConduit.registerReceiver();

    auto packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 1);

    packetConduit.getPackets(spoutId, 1, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId, 1);

    EXPECT_EQ(packetConduit.availablePackets(spoutId), 0);
}

TEST_F(PacketConduitTest, GetPacketsMultiple) {
    auto packetCount = 5;
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId = packetConduit.registerReceiver();

    for (int i = 0; i < packetCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetConduit.sendPacket(move(packet));
    }
    EXPECT_EQ(packetConduit.availablePackets(spoutId), packetCount);

    packetConduit.getPackets(spoutId, packetCount, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId, packetCount);
    EXPECT_EQ(packetConduit.availablePackets(spoutId), 0);
}

TEST_F(PacketConduitTest, ConcludePacketUseWithInvalidSpoutId) {
    PacketConduit packetConduit{};
    EXPECT_THROW(packetConduit.concludePacketUse(0, 1), std::out_of_range);
    EXPECT_THROW(packetConduit.concludePacketUse(-5, 1), std::out_of_range);
    EXPECT_THROW(packetConduit.concludePacketUse(12, 1), std::out_of_range);
}

TEST_F(PacketConduitTest, ConcludePacketUseWithInvalidPacketCount) {
    PacketConduit packetConduit{};
    auto spoutId = packetConduit.registerReceiver();
    EXPECT_THROW(packetConduit.concludePacketUse(spoutId, 0), std::out_of_range);
    EXPECT_THROW(packetConduit.concludePacketUse(spoutId, -5), std::out_of_range);
    EXPECT_THROW(packetConduit.concludePacketUse(spoutId, 100), std::out_of_range);
}

TEST_F(PacketConduitTest, CleanSinglePacket) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId0 = packetConduit.registerReceiver();

    auto packet = std::make_unique<PacketMock>();
    packetConduit.sendPacket(move(packet));

    packetConduit.getPackets(spoutId0, 1, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId0, 1);

    auto spoutId1 = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 0);
}

TEST_F(PacketConduitTest, CleanMultiplePackets) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId0 = packetConduit.registerReceiver();

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetConduit.sendPacket(move(packet));
    }

    packetConduit.getPackets(spoutId0, 3, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId0, 3);

    auto spoutId1 = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 2);
}

TEST_F(PacketConduitTest, CleanMultiplePacketsMultipleSpouts) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId0 = packetConduit.registerReceiver();
    auto spoutId1 = packetConduit.registerReceiver();

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetConduit.sendPacket(move(packet));
    }

    packetConduit.getPackets(spoutId0, 4, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId0, 4);

    packetConduit.getPackets(spoutId1, 3, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId1, 3);

    auto spoutId2 = packetConduit.registerReceiver();
    EXPECT_EQ(packetConduit.availablePackets(spoutId2), 2);
}

TEST_F(PacketConduitTest, MultipleSpoutsMultiplePackets) {
    PacketConduit packetConduit{};
    PacketCollectionManagerMock packetCollectionManagerMock{};
    auto spoutId0 = packetConduit.registerReceiver();
    auto spoutId1 = packetConduit.registerReceiver();

    auto initialPacketCount = 5;
    for (int i = 0; i < initialPacketCount; i++) {
        auto packet = std::make_unique<PacketMock>();
        packetConduit.sendPacket(move(packet));
    }
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), initialPacketCount);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), initialPacketCount);

    packetConduit.getPackets(spoutId0, 2, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId0, 2);
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), 3);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), initialPacketCount);

    packetConduit.getPackets(spoutId1, 4, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId1, 4);
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), 3);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 1);

    packetConduit.getPackets(spoutId1, 1, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId1, 1);
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), 3);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 0);

    packetConduit.getPackets(spoutId0, 3, packetCollectionManagerMock);
    packetConduit.concludePacketUse(spoutId0, 3);
    EXPECT_EQ(packetConduit.availablePackets(spoutId0), 0);
    EXPECT_EQ(packetConduit.availablePackets(spoutId1), 0);
}

}
