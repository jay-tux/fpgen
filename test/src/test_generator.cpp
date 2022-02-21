#include "generator.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>

fpgen::generator<float> empty() { co_return; }

fpgen::generator<int> infinite() {
  int value = 0;
  while (true) {
    co_yield value;
    value++;
  }
}

fpgen::generator<long> finite_squares(int min, int max) {
  for (int val = min; val <= max; val++) {
    co_yield val *val;
  }
  co_return; // max *max;
}

TEST(generator, accept_empty_gen) {
  auto emptygen = empty();
  SUCCEED();
}

TEST(generator, iterator_empty_gen) {
  auto gen = empty();
  // gen();
  for (auto v : gen) {
    FAIL();
  }
  SUCCEED();
}

TEST(generator, can_call_and_continue) {
  auto intgen = infinite();
  int value = intgen();
  EXPECT_EQ(0, value);
  value = intgen();
  EXPECT_EQ(1, value);
}

TEST(generator, can_while_over) {
  int value;
  int expect = 0;
  auto intgen2 = finite_squares(0, 12);
  while (intgen2) {
    value = intgen2();
    EXPECT_THAT(expect, testing::AllOf(testing::Le(12), testing::Ge(0)));
    EXPECT_EQ(expect * expect, value);
    expect++;
  }
}

TEST(generator, iterator) {
  int expect = -4;
  auto intgen3 = finite_squares(-4, 8);
  for (auto value : intgen3) {
    EXPECT_THAT(expect, testing::AllOf(testing::Le(8), testing::Ge(-4)));
    EXPECT_EQ(expect * expect, value);
    expect++;
  }
}
