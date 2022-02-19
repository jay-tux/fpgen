#include "aggregators.hpp"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"
#include "stream.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// temporary
#include <cxxabi.h>

char *demangle(const char *name) {
  return abi::__cxa_demangle(name, nullptr, nullptr, nullptr);
}

char mapper(int i) { return 'a' + i; }
fpgen::generator<char> map(fpgen::generator<int> in) { return map(in, mapper); }

TEST(stream, inout) {
  std::vector<int> in = {0, 1, 2, 3};
  fpgen::stream<int> strm = fpgen::from(in);
  std::vector<int> output;
  strm >> output;

  std::vector<int> expected;
  fpgen::generator<int> gen = fpgen::from(in);
  expected = fpgen::aggregate_to(gen, expected);

  EXPECT_EQ(output, expected);
}
