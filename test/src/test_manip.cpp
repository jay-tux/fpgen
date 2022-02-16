#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <map>
#include <set>
#include <string>
#include <vector>

fpgen::generator<size_t> manip_empty() { co_return 0; }

fpgen::generator<size_t> manip() {
  size_t i = 1;
  while (i < 1024) {
    co_yield i;
    i *= 2;
  }
  co_return i;
}

fpgen::generator<size_t> until12() {
  for (int i = 0; i < 12; i++) {
    co_yield i;
  }
  co_return 12;
}

size_t mapper(size_t v) { return v * v; }
bool is_even(size_t v) { return (v % 2 == 0); }
bool over_100(size_t v) { return (v > 100); }

TEST(manipulators, map_empty) {
  auto gen = manip_empty();
  gen();

  for (auto v : fpgen::map(gen, mapper)) {
    FAIL() << "Should not return a value";
  }
  SUCCEED();
}

TEST(manipulators, map) {
  auto gen = manip();
  size_t i = 1;
  for (auto v : fpgen::map(gen, mapper)) {
    EXPECT_EQ(i * i, v);
    EXPECT_TRUE(i <= 1024);
    i *= 2;
  }
}

TEST(manipulators, zip_both_empty) {
  auto gen = manip_empty();
  auto gen2 = manip_empty();
  gen();
  gen2();

  for (auto v : fpgen::zip(gen, gen2)) {
    FAIL() << "Should not return a value";
  }
  SUCCEED();
}

TEST(manipulators, zip_first_empty) {
  auto gen = manip_empty();
  auto gen2 = fpgen::inc((size_t)0);
  gen();

  for (auto v : fpgen::zip(gen, gen2)) {
    FAIL() << "Should not return a value";
  }
  SUCCEED();
}

TEST(manipulators, zip_second_empty) {
  auto gen = fpgen::inc((size_t)0);
  auto gen2 = manip_empty();
  gen2();

  for (auto v : fpgen::zip(gen, gen2)) {
    FAIL() << "Should not return a value";
  }
  SUCCEED();
}

TEST(manipulators, zip_none_empty) {
  auto gen = fpgen::inc((size_t)0);
  auto gen2 = manip();

  size_t i = 0;
  size_t j = 1;

  for (auto v : fpgen::zip(gen, gen2)) {
    EXPECT_EQ(std::get<0>(v), i);
    EXPECT_EQ(std::get<1>(v), j);
    EXPECT_TRUE(j <= 1024);
    EXPECT_TRUE(i <= 10);

    i++;
    j *= 2;
  }
}

TEST(manipulators, filter_empty) {
  auto gen = manip_empty();
  gen();

  size_t i = 0;

  for (auto v : fpgen::filter(gen, is_even)) {
    FAIL();
  }
  SUCCEED();
}

TEST(manipulators, filter_to_empty) {
  auto gen = until12();

  for (auto v : fpgen::filter(gen, over_100)) {
    FAIL();
  }
  SUCCEED();
}

TEST(manipulators, filter_normal) {
  auto gen = until12();
  size_t i = 0;
  for (auto v : fpgen::filter(gen, is_even)) {
    EXPECT_EQ(v, i);
    EXPECT_TRUE(i <= 12);
    i += 2;
  }
}
