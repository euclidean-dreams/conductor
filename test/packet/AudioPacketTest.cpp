#include <gtest/gtest.h>
#include "packet/RawAudioPacket.h"

namespace conductor {

class AudioPacketTest : public testing::Test {
protected:
    static inline std::unique_ptr<RawAudioPacket> createFinalizedAudioPacket() {
        auto rawAudioPacket = std::make_unique<RawAudioPacket>(0, ImpresarioSerialization::FrequencyBand::all, 10);
        for (int i = 0; i < rawAudioPacket->size(); i++) {
            rawAudioPacket->addSample(static_cast<float>(i));
        }
        return rawAudioPacket;
    }
};

TEST_F(AudioPacketTest, From) {
    RawAudioPacket rawAudioPacket{0, ImpresarioSerialization::FrequencyBand::all, 10};
    Packet &packet = rawAudioPacket;
    auto &result = RawAudioPacket::from(packet);
    EXPECT_EQ(&result, &rawAudioPacket);
}

TEST_F(AudioPacketTest, AddSample) {
    RawAudioPacket rawAudioPacket{0, ImpresarioSerialization::FrequencyBand::all, 10};
    rawAudioPacket.addSample(0.0f);
}

TEST_F(AudioPacketTest, AddSampleToFinalizedAudioPacket) {
    auto rawAudioPacket = createFinalizedAudioPacket();
    EXPECT_THROW(rawAudioPacket->addSample(0.0f), std::runtime_error);
}

TEST_F(AudioPacketTest, GetSample) {
    auto rawAudioPacket = createFinalizedAudioPacket();
    auto sample = rawAudioPacket->getSample(0);
    EXPECT_EQ(sample, 0);
}

TEST_F(AudioPacketTest, GetSampleOutOfRange) {
    auto rawAudioPacket = createFinalizedAudioPacket();
    EXPECT_THROW(rawAudioPacket->getSample(-1), std::out_of_range);
    EXPECT_THROW(rawAudioPacket->getSample(rawAudioPacket->size()), std::out_of_range);
}

TEST_F(AudioPacketTest, GetSampleWhileNotFinalized) {
    RawAudioPacket rawAudioPacket{0, ImpresarioSerialization::FrequencyBand::all, 10};
    EXPECT_THROW(rawAudioPacket.getSample(0), std::runtime_error);
}

}
