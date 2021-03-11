#include <gtest/gtest.h>
#include "packet/AudioPacket.h"

namespace conductor {

class AudioPacketTest : public testing::Test {
protected:
    static inline std::unique_ptr<AudioPacket> createFinalizedAudioPacket() {
        auto audioPacket = std::make_unique<AudioPacket>();
        for (int i = 0; i < audioPacket->size(); i++) {
            audioPacket->addSample(static_cast<float>(i));
        }
        return audioPacket;
    }
};

TEST_F(AudioPacketTest, AddSampleSuccess) {
    AudioPacket audioPacket{};
    audioPacket.addSample(0.0f);
}

TEST_F(AudioPacketTest, AddSampleToFinalizedAudioPacket) {
    auto audioPacket = createFinalizedAudioPacket();
    EXPECT_THROW(audioPacket->addSample(0.0f), std::runtime_error);
}

TEST_F(AudioPacketTest, GetSample) {
    auto audioPacket = createFinalizedAudioPacket();
    auto sample = audioPacket->getSample(0);
    EXPECT_EQ(sample, 0);
}

TEST_F(AudioPacketTest, GetSampleOutOfRange) {
    auto audioPacket = createFinalizedAudioPacket();
    EXPECT_THROW(audioPacket->getSample(-1), std::out_of_range);
    EXPECT_THROW(audioPacket->getSample(audioPacket->size()), std::out_of_range);
}

TEST_F(AudioPacketTest, GetSampleWhileNotFinalized) {
    AudioPacket audioPacket{};
    EXPECT_THROW(audioPacket.getSample(0), std::runtime_error);
}

}
