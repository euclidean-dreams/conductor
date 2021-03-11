#include <gtest/gtest.h>
#include "audioStream/RingBuffer.h"

namespace conductor {

class RingBufferTest : public testing::Test {
protected:
    RingBuffer ringBuffer{2, 3};

    void static nextPacketMatchesExpected(RingBuffer &ringBuffer, float *expected) {
        ASSERT_TRUE(ringBuffer.nextPacketIsReady());
        auto packet = ringBuffer.getNextPacket();
        for (int i = 0; i < ringBuffer.getPacketSize(); i++) {
            ASSERT_FLOAT_EQ((*packet)[i], expected[i]);
        }
    }
};

TEST_F(RingBufferTest, EmptyInitialization) {
    ASSERT_FALSE(ringBuffer.nextPacketIsReady());
}

TEST_F(RingBufferTest, OneSample) {
    float sample[] = {0.0, 1.0};
    ringBuffer.addSamples(sample, 2);
    nextPacketMatchesExpected(ringBuffer, sample);
}

TEST_F(RingBufferTest, TwoSamples) {
    float sample0[] = {0.0, 1.0};
    ringBuffer.addSamples(sample0, 2);
    float sample1[] = {2.0, 3.0};
    ringBuffer.addSamples(sample1, 2);
    nextPacketMatchesExpected(ringBuffer, sample0);
    nextPacketMatchesExpected(ringBuffer, sample1);
}

TEST_F(RingBufferTest, SmallSamples) {
    float sample0[] = {0.0};
    ringBuffer.addSamples(sample0, 1);
    ASSERT_FALSE(ringBuffer.nextPacketIsReady());
    float sample1[] = {1.0};
    ringBuffer.addSamples(sample1, 1);
    float expected[] = {0.0, 1.0};
    nextPacketMatchesExpected(ringBuffer, expected);
}

TEST_F(RingBufferTest, WrapAroundBasic) {
    float sample0[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    ringBuffer.addSamples(sample0, 6);
    float expected0[] = {0.0, 1.0};
    nextPacketMatchesExpected(ringBuffer, expected0);
    float expected1[] = {2.0, 3.0};
    nextPacketMatchesExpected(ringBuffer, expected1);
    float expected2[] = {4.0, 5.0};
    nextPacketMatchesExpected(ringBuffer, expected2);
    float sample1[] = {6.0, 7.0};
    ringBuffer.addSamples(sample1, 2);
    nextPacketMatchesExpected(ringBuffer, sample1);
}

TEST_F(RingBufferTest, WrapAroundNotEnoughForNextPacket) {
    float sample0[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    ringBuffer.addSamples(sample0, 6);
    float expected0[] = {0.0, 1.0};
    nextPacketMatchesExpected(ringBuffer, expected0);
    float expected1[] = {2.0, 3.0};
    nextPacketMatchesExpected(ringBuffer, expected1);
    float expected2[] = {4.0, 5.0};
    nextPacketMatchesExpected(ringBuffer, expected2);
    float sample1[] = {6.0};
    ringBuffer.addSamples(sample1, 1);
    ASSERT_FALSE(ringBuffer.nextPacketIsReady());
    float sample2[] = {7.0};
    ringBuffer.addSamples(sample2, 1);
    float expected3[] = {6.0, 7.0};
    nextPacketMatchesExpected(ringBuffer, expected3);
}

TEST_F(RingBufferTest, DoubleWrap) {
    float sample0[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    ringBuffer.addSamples(sample0, 6);
    float expected0[] = {0.0, 1.0};
    nextPacketMatchesExpected(ringBuffer, expected0);
    float expected1[] = {2.0, 3.0};
    nextPacketMatchesExpected(ringBuffer, expected1);
    float expected2[] = {4.0, 5.0};
    nextPacketMatchesExpected(ringBuffer, expected2);
    float sample1[] = {6.0, 7.0, 8.0, 9.0};
    ringBuffer.addSamples(sample1, 4);
    float expected3[] = {6.0, 7.0};
    nextPacketMatchesExpected(ringBuffer, expected3);
    float expected4[] = {8.0, 9.0};
    nextPacketMatchesExpected(ringBuffer, expected4);
    float sample2[] = {10.0, 11.0, 12.0, 13.0};
    ringBuffer.addSamples(sample2, 4);
    float expected5[] = {10.0, 11.0};
    nextPacketMatchesExpected(ringBuffer, expected5);
    float expected6[] = {12.0, 13.0};
    nextPacketMatchesExpected(ringBuffer, expected6);
}

}
