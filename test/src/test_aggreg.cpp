#include <map>
#include <sstream>
#include <vector>

#include "aggregators.hpp"
#include "doctest/doctest.h"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"


fpgen::generator<size_t> a_empty() { co_return; }

fpgen::generator<size_t> values() {
  size_t i = 0;
  size_t j = 1;
  co_yield 0;
  co_yield 1;

  while (j <= 21) {
    size_t tmp = i + j;
    co_yield tmp;
    i = j;
    j = tmp;
  }

  // co_return i + j;
}

size_t calc_sum() {
  size_t i = 0;
  size_t j = 1;
  size_t sum = 1;
  while (j <= 21) {
    size_t tmp = i + j;
    sum += tmp;
    i = j;
    j = tmp;
  }
  return sum;
}

size_t sum(size_t old, size_t in) { return old + in; }
size_t sum_ref(size_t &old, size_t in) {
  old = old + in;
  return old;
}

TEST_CASE("Aggregate empty generator") {
  auto gen = a_empty();
  std::vector<size_t> res;
  CHECK_EQ(0, fpgen::aggregate_to(gen, res).size());
}

TEST_CASE("Aggregate to std::vector") {
  auto gen = values();
  std::vector<size_t> res;
  fpgen::aggregate_to(gen, res);

  CHECK_EQ(0, res[0]);
  CHECK_EQ(1, res[1]);
  CHECK_EQ(1, res[2]);
  CHECK_EQ(2, res[3]);
  CHECK_EQ(3, res[4]);
  CHECK_EQ(5, res[5]);
  CHECK_EQ(8, res[6]);
  CHECK_EQ(13, res[7]);
  CHECK_EQ(21, res[8]);
  CHECK_EQ(34, res[9]);
  CHECK_EQ(res.size(), 10);
}

TEST_CASE("Aggregate: std::vector to generator to std::vector") {
  std::vector<size_t> in = {0, 1, 2, 3, 4, 5, 6};
  std::vector<size_t> out = {};
  auto gen = fpgen::from(in);
  out = fpgen::aggregate_to(gen, out);
  CHECK(in == out);
}

TEST_CASE("Aggregate to std::map") {
  fpgen::generator<size_t> sources[2] = {values(), values()};
  auto gen = fpgen::zip(sources[0], sources[1]);
  std::map<size_t, size_t> res;
  fpgen::tup_aggregate_to(gen, res);

  CHECK_EQ(0, res[0]);
  CHECK_EQ(1, res[1]);
  CHECK_EQ(2, res[2]);
  CHECK_EQ(3, res[3]);
  CHECK_EQ(5, res[5]);
  CHECK_EQ(8, res[8]);
  CHECK_EQ(13, res[13]);
  CHECK_EQ(21, res[21]);
  CHECK_EQ(34, res[34]);
  CHECK_EQ(res.size(), 9);
}

TEST_CASE("Count empty generator") {
  auto gen = a_empty();
  CHECK_EQ(0, fpgen::count(gen));
}

TEST_CASE("Count generator") {
  auto gen = values();
  CHECK_EQ(10, fpgen::count(gen));
}

TEST_CASE("Fold [using no-input, empty generator]") {
  auto gen = a_empty();
  CHECK_EQ(0, fpgen::fold<size_t>(gen, sum));
}

TEST_CASE("Fold [using no-input, non-empty generator]") {
  auto gen = values();
  CHECK(calc_sum() == fpgen::fold<size_t>(gen, sum));
}

TEST_CASE("Fold [using input, empty generator]") {
  auto gen = a_empty();
  CHECK_EQ(7, fpgen::fold<size_t>(gen, sum, 7));
}

TEST_CASE("Fold [using input, non-empty generator]") {
  auto gen = values();
  CHECK_EQ(calc_sum() + 7, fpgen::fold<size_t>(gen, sum, 7));
}

TEST_CASE("Fold [using ref input, empty generator]") {
  auto gen = a_empty();
  size_t res = 7;
  CHECK_EQ(7, fpgen::fold_ref<size_t>(gen, sum, res));
  CHECK_EQ(7, res);
}

TEST_CASE("Fold [using ref input, non-epty generator]") {
  auto gen = values();
  size_t res = 7;
  CHECK_EQ(calc_sum() + 7, fpgen::fold_ref<size_t>(gen, sum, res));
  CHECK_EQ(calc_sum() + 7, res);
}

TEST_CASE("Sum empty generator") {
  auto gen = a_empty();
  CHECK_EQ(0, fpgen::sum(gen));
}

TEST_CASE("Sum over generator") {
  auto gen = values();
  CHECK_EQ(calc_sum(), fpgen::sum(gen));
}

TEST_CASE("Foreach over empty generator") {
  auto gen = a_empty();
  size_t res = 0;
  fpgen::foreach (gen, [&res](size_t val) { res += val; });
  CHECK_EQ(res, 0);
}

TEST_CASE("Foreach over non-empty generator") {
  auto gen = values();
  auto gen2 = values();
  size_t res = 0;
  fpgen::foreach (gen, [&res](size_t val) { res += val; });
  CHECK_EQ(res, fpgen::sum(gen2));
}

TEST_CASE("Output to stream, no separator") {
  std::vector<int> vals = {1, 2, 3, 4, 5, 6};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  fpgen::to_stream(gen, strm);
  CHECK_EQ(strm.str(), "123456");
}

TEST_CASE("Output to stream, with separator") {
  std::vector<int> vals = {1, 2, 3, 4, 5, 6, 7};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  fpgen::to_stream(gen, strm, " ");
  CHECK_EQ(strm.str(), "1 2 3 4 5 6 7");
}

TEST_CASE("Output to stream (by lines), with trailing line") {
  std::vector<int> vals = {1, 2, 3, 4};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  std::stringstream expect;
  for (auto v : vals)
    expect << v << std::endl;
  fpgen::to_lines(gen, strm);
  CHECK_EQ(strm.str(), expect.str());
}

TEST_CASE("Output to stream (by lines), without trailing line") {
  std::vector<int> vals = {1, 2, 3, 4};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  std::stringstream expect;
  expect << 1 << std::endl << 2 << std::endl << 3 << std::endl << 4;
  fpgen::to_lines_no_trail(gen, strm);
  CHECK_EQ(strm.str(), expect.str());
}
