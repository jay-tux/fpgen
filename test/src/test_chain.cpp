#include "aggregators.hpp"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace fpgen;

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &in, std::tuple<T1, T2> &other) {
  return in << "{ " << std::get<0>(other) << ", " << std::get<1>(other) << " }";
}

TEST(chain, simple_chain) {
  /*
  Chain:
  -> [0..] -> [5..] -> [5..13] -> [15...169]  \
                                               -> {[15...169], ['h'...'p']}
  -> [0..] -> [7..] -> [7..22] -> ['h'...'w'] /
  */
  auto gen =
      map(take(drop(inc((size_t)0), 5), 9), [](size_t in) { return in * in; });

  auto second = map(take(drop(inc((size_t)0), 7), 22),
                    [](size_t in) { return (char)('a' + (in % 26)); });

  size_t value = 5;
  for (auto v : zip(gen, second)) {
    EXPECT_EQ(value * value, std::get<0>(v));
    EXPECT_EQ('a' + 2 + value, std::get<1>(v));
    EXPECT_TRUE(value <= 13);
    value++;
  }
  EXPECT_EQ(value, 14);
}
