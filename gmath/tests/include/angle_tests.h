#ifndef ANGLE_TESTS_H
#define ANGLE_TESTS_H

#include <gtest/gtest.h>

#include "gmath.h"

TEST(Angle, Wrapping) {
    // 0 PI should wrap to 0 PI
    EXPECT_EQ(roundf(std::numbers::pi_v<float> * 0.0f * 100.0f) * 0.01f,
        roundf(gmath::Angle::FromWrapped(std::numbers::pi_v<float> * 0.0 + 2 * std::numbers::pi_v<float>) * 100.0f) * 0.01f);

    // 2 PI should wrap to 0 PI
    EXPECT_EQ(roundf(std::numbers::pi_v<float> * 0.0f * 100.0f) * 0.01f,
        roundf(gmath::Angle::FromWrapped(std::numbers::pi_v<float> * 2.0 + 2 * std::numbers::pi_v<float>) * 100.0f) * 0.01f);

    // 1/2 PI should wrap to 1/2 PI
    EXPECT_EQ(roundf(std::numbers::pi_v<float> * 0.5f * 100.0f) * 0.01f,
        roundf(gmath::Angle::FromWrapped(std::numbers::pi_v<float> * 0.5 + 2 * std::numbers::pi_v<float>) * 100.0f) * 0.01f);

    // -1/2 PI should wrap to 3/2 PI
    EXPECT_EQ(roundf(std::numbers::pi_v<float> * 1.5f * 100.0f) * 0.01f,
        roundf(gmath::Angle::FromWrapped(std::numbers::pi_v<float> * -0.5 + 2 * std::numbers::pi_v<float>) * 100.0f) * 0.01f);

    // 5/2 PI should wrap to 1/2 PI
    EXPECT_EQ(roundf(std::numbers::pi_v<float> * 0.5f * 100.0f) * 0.01f,
        roundf(gmath::Angle::FromWrapped(std::numbers::pi_v<float> * 2.5 + 2 * std::numbers::pi_v<float>) * 100.0f) * 0.01f);

    //-5/2 PI should wrap to 3/2 PI
    EXPECT_EQ(roundf(std::numbers::pi_v<float> * 1.5f * 100.0f) * 0.01f,
        roundf(gmath::Angle::FromWrapped(std::numbers::pi_v<float> * -2.5 + 2 * std::numbers::pi_v<float>) * 100.0f) * 0.01f);
}

#endif  // ANGLE_TESTS_H
