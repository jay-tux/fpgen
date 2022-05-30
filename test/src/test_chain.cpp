#include <tuple>
#include "aggregators.hpp"
#include "doctest/doctest.h"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"

using fpgen::drop;
using fpgen::take;
using fpgen::inc;

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &in, std::tuple<T1, T2> &other) {
  return in << "{ " << std::get<0>(other) << ", " << std::get<1>(other) << " }";
}

TEST_CASE("Simple chain check") {
  /*
  Chain:
  -> [0..] -> [5..] -> [5..13] -> [15...169]  \
                                               -> {[15...169], ['h'...'p']}
  -> [0..] -> [7..] -> [7..22] -> ['h'...'w'] /
  */
  auto gen =
      map(take(drop(inc((size_t)0), 5), 9), [](size_t in) { return in * in; });

  auto second = map(take(drop(inc((size_t)0), 7), 22),
                    [](size_t in) { return static_cast<char>('a' + (in % 26)); });

  size_t value = 5;
  for (auto v : zip(gen, second)) {
    CHECK_EQ(value * value, std::get<0>(v));
    CHECK_EQ('a' + 2 + value, std::get<1>(v));
    CHECK_LE(value, 13);
    value++;
  }
  CHECK_EQ(value, 14);
}
