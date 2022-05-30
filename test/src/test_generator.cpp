#include "doctest/doctest.h"
#include "generator.hpp"

fpgen::generator<float> empty() { co_return; }

fpgen::generator<int> infinite() {
  int value = 0;
  while (true) {
    co_yield value;
    value++;
  }
}

fpgen::generator<int64_t> finite_squares(int min, int max) {
  for (int val = min; val <= max; val++) {
    co_yield val *val;
  }
  co_return; // max *max;
}

TEST_CASE("Generator accepts empty generator") { auto emptygen = empty(); }

TEST_CASE("Iterator over an empty generator") {
  auto gen = empty();
  for (auto v : gen) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Call-and-continue for a generator") {
  auto intgen = infinite();
  int value = intgen();
  CHECK_EQ(0, value);
  value = intgen();
  CHECK_EQ(1, value);
}

TEST_CASE("Use while over a generator") {
  int value;
  int expect = 0;
  auto intgen2 = finite_squares(0, 12);
  while (intgen2) {
    value = intgen2();
    CHECK_LE(expect, 12);
    CHECK_GE(expect, 0);
    CHECK_EQ(expect * expect, value);
    expect++;
  }
}

TEST_CASE("Iterate over a generator") {
  int expect = -4;
  auto intgen3 = finite_squares(-4, 8);
  for (auto value : intgen3) {
    CHECK_LE(expect, 8);
    CHECK_GE(expect, -4);
    CHECK_EQ(expect * expect, value);
    expect++;
  }
}
