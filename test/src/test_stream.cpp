#include "aggregators.hpp"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"
#include "stream.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

char mapper(int i) { return 'a' + i; }
fpgen::generator<char> map(fpgen::generator<int> in) { return map(in, mapper); }

TEST(stream, operator) {
  fpgen::stream<int> strm(fpgen::from(std::vector<int>{0, 1, 2, 3, 4}));
  // fpgen::stream<char> strm2 = strm << map;
  fpgen::stream<char> strm2 = strm << [](fpgen::generator<int> in) {
    return map(in, [](int i) { return (char)('a' + i); });
  };
}
