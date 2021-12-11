#include <gtest/gtest.h>
#include <libabyss/common/ringbuffer.h>

TEST(RingBuffer, Basics) {
    LibAbyss::RingBuffer rb(4);
    uint8_t data[4] = {1, 2, 3, 4};
    rb.PushData(data);
    rb.ReadData(data);
    for (auto i = 0; i < 4; i++) {
        ASSERT_EQ(data[i], i+1);
    }

    rb.Reset();
    uint8_t data2[2] = {4, 6};
    rb.PushData(data2);
    rb.ReadData(data);
    ASSERT_EQ(data[0], 4);
    ASSERT_EQ(data[1], 6);
    ASSERT_EQ(data[2], 0);
    ASSERT_EQ(data[3], 0);

    rb.Reset();
    uint8_t data3[3] = {7, 8, 9};
    rb.PushData(data3);
    ASSERT_THROW(rb.PushData(data3), std::runtime_error);

    rb.Reset();
    uint8_t data4[2] = {1,2};
    uint8_t data5[2] = {0,0};
    rb.PushData(data4);
    ASSERT_EQ(rb.Available(), 2);
    rb.ReadData(data5);
    ASSERT_EQ(rb.Available(), 0);
    rb.PushData(data4);
    ASSERT_EQ(rb.Available(), 2);
    rb.ReadData(data5);
    ASSERT_EQ(rb.Available(), 0);
}