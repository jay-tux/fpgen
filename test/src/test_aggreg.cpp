#include "aggregators.hpp"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <map>
#include <sstream>
#include <vector>

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

TEST(aggregate, empty) {
  auto gen = a_empty();
  // gen();
  std::vector<size_t> res;
  EXPECT_EQ(0, fpgen::aggregate_to(gen, res).size());
}

TEST(aggregate, vector) {
  auto gen = values();
  std::vector<size_t> res;
  fpgen::aggregate_to(gen, res);

  EXPECT_EQ(0, res[0]);
  EXPECT_EQ(1, res[1]);
  EXPECT_EQ(1, res[2]);
  EXPECT_EQ(2, res[3]);
  EXPECT_EQ(3, res[4]);
  EXPECT_EQ(5, res[5]);
  EXPECT_EQ(8, res[6]);
  EXPECT_EQ(13, res[7]);
  EXPECT_EQ(21, res[8]);
  EXPECT_EQ(34, res[9]);
  EXPECT_EQ(res.size(), 10);
}

TEST(aggregate, vec_to_vec) {
  std::vector<size_t> in = {0, 1, 2, 3, 4, 5, 6};
  std::vector<size_t> out = {};
  auto gen = fpgen::from(in);
  out = fpgen::aggregate_to(gen, out);
  EXPECT_EQ(in, out);
}

TEST(aggregate, map) {
  fpgen::generator<size_t> sources[2] = {values(), values()};
  auto gen = fpgen::zip(sources[0], sources[1]);
  std::map<size_t, size_t> res;
  fpgen::tup_aggregate_to(gen, res);

  EXPECT_EQ(0, res[0]);
  EXPECT_EQ(1, res[1]);
  EXPECT_EQ(2, res[2]);
  EXPECT_EQ(3, res[3]);
  EXPECT_EQ(5, res[5]);
  EXPECT_EQ(8, res[8]);
  EXPECT_EQ(13, res[13]);
  EXPECT_EQ(21, res[21]);
  EXPECT_EQ(34, res[34]);
  EXPECT_EQ(res.size(), 9);
}

TEST(aggregate, count_empty) {
  auto gen = a_empty();
  // gen();
  EXPECT_EQ(0, fpgen::count(gen));
}

TEST(aggregate, count) {
  auto gen = values();
  EXPECT_EQ(10, fpgen::count(gen));
}

TEST(fold, fold_noin_empty) {
  auto gen = a_empty();
  // gen();
  EXPECT_EQ(0, fpgen::fold<size_t>(gen, sum));
}

TEST(fold, fold_noin) {
  auto gen = values();
  EXPECT_EQ(calc_sum(), fpgen::fold<size_t>(gen, sum));
}

TEST(fold, fold_in_noref_empty) {
  auto gen = a_empty();
  // gen();
  EXPECT_EQ(7, fpgen::fold<size_t>(gen, sum, 7));
}

TEST(fold, fold_in_noref) {
  auto gen = values();
  EXPECT_EQ(calc_sum() + 7, fpgen::fold<size_t>(gen, sum, 7));
}

TEST(fold, fold_in_ref_empty) {
  auto gen = a_empty();
  // gen();
  size_t res = 7;
  EXPECT_EQ(7, fpgen::fold_ref<size_t>(gen, sum, res));
  EXPECT_EQ(7, res);
}

TEST(fold, fold_in_ref) {
  auto gen = values();
  size_t res = 7;
  EXPECT_EQ(calc_sum() + 7, fpgen::fold_ref<size_t>(gen, sum, res));
  EXPECT_EQ(calc_sum() + 7, res);
}

TEST(sum, empty) {
  auto gen = a_empty();
  // gen();
  EXPECT_EQ(0, fpgen::sum(gen));
}

TEST(sum, normal) {
  auto gen = values();
  EXPECT_EQ(calc_sum(), fpgen::sum(gen));
}

TEST(foreach, empty) {
  auto gen = a_empty();
  // gen();
  size_t res = 0;
  fpgen::foreach (gen, [&res](size_t val) { res += val; });
  EXPECT_EQ(res, 0);
}

TEST(foreach, normal) {
  auto gen = values();
  auto gen2 = values();
  size_t res = 0;
  fpgen::foreach (gen, [&res](size_t val) { res += val; });
  EXPECT_EQ(res, fpgen::sum(gen2));
}

TEST(stream, nosep) {
  std::vector<int> vals = {1, 2, 3, 4, 5, 6};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  fpgen::to_stream(gen, strm);
  EXPECT_EQ(strm.str(), "123456");
}

TEST(stream, sep) {
  std::vector<int> vals = {1, 2, 3, 4, 5, 6, 7};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  fpgen::to_stream(gen, strm, " ");
  EXPECT_EQ(strm.str(), "1 2 3 4 5 6 7");
}

TEST(stream, lines_trail) {
  std::vector<int> vals = {1, 2, 3, 4};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  std::stringstream expect;
  for (auto v : vals)
    expect << v << std::endl;
  fpgen::to_lines(gen, strm);
  EXPECT_EQ(strm.str(), expect.str());
}

TEST(stream, lines_no_trail) {
  std::vector<int> vals = {1, 2, 3, 4};
  auto gen = fpgen::from(vals);
  std::stringstream strm;
  std::stringstream expect;
  expect << 1 << std::endl << 2 << std::endl << 3 << std::endl << 4;
  fpgen::to_lines_no_trail(gen, strm);
  EXPECT_EQ(strm.str(), expect.str());
}
