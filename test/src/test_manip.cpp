#include "doctest/doctest.h"
#include "generator.hpp"
#include "manipulators.hpp"
#include "sources.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

fpgen::generator<size_t> manip_empty() { co_return; }

fpgen::generator<size_t> manip() {
  size_t i = 1;
  while (i <= 1024) {
    co_yield i;
    i *= 2;
  }
  co_return; // i;
}

fpgen::generator<size_t> until12() {
  for (int i = 0; i <= 12; i++) {
    co_yield i;
  }
  co_return; // 12;
}

size_t mapper(size_t v) { return v * v; }
bool is_even(size_t v) { return (v % 2 == 0); }
bool over_100(size_t v) { return (v > 100); }

TEST_CASE("Map over empty generator") {
  auto gen = manip_empty();

  for (auto v : fpgen::map(gen, mapper)) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Map over a non-empty generator") {
  auto gen = manip();
  size_t i = 1;
  for (auto v : fpgen::map(gen, mapper)) {
    CHECK(i * i == v);
    CHECK(i <= 1024);
    i *= 2;
  }
}

TEST_CASE("Zip over two empty generator") {
  auto gen = manip_empty();
  auto gen2 = manip_empty();

  for (auto v : fpgen::zip(gen, gen2)) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Zip, first generator is empty") {
  auto gen = manip_empty();
  auto gen2 = fpgen::inc((size_t)0);

  for (auto v : fpgen::zip(gen, gen2)) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Zip, second generator is empty") {
  auto gen = fpgen::inc((size_t)0);
  auto gen2 = manip_empty();

  for (auto v : fpgen::zip(gen, gen2)) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Zip, both generators are non-empty") {
  auto gen = fpgen::inc((size_t)0);
  auto gen2 = manip();

  size_t i = 0;
  size_t j = 1;

  for (auto v : fpgen::zip(gen, gen2)) {
    CHECK(std::get<0>(v) == i);
    CHECK(std::get<1>(v) == j);
    CHECK(j <= 1024);
    CHECK(i <= 10);

    i++;
    j *= 2;
  }
}

TEST_CASE("Filter an empty generator") {
  auto gen = manip_empty();

  size_t i = 0;

  for (auto v : fpgen::filter(gen, is_even)) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Filter to an empty generator") {
  auto gen = until12();

  for (auto v : fpgen::filter(gen, over_100)) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Filter a generator") {
  auto gen = until12();
  size_t i = 0;
  for (auto v : fpgen::filter(gen, is_even)) {
    CHECK(v == i);
    CHECK(i <= 12);
    i += 2;
  }
}

TEST_CASE("Drop from an empty generator") {
  auto gen = drop(manip_empty(), 5);
  for (auto v : gen) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Drop from a generator") {
  auto gen = drop(until12(), 5);
  size_t exp = 5;
  for (auto v : gen) {
    CHECK(v == exp);
    CHECK(exp <= 12);
    exp++;
  }
  CHECK(exp == 13);
}

TEST_CASE("Take from an empty generator") {
  auto gen = take(manip_empty(), 4);
  for (auto v : gen) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Take from a generator") {
  auto gen = take(fpgen::inc((size_t)0), 8);
  size_t exp = 0;
  for (auto v : gen) {
    CHECK(v == exp);
    CHECK(exp <= 8);
    exp++;
  }
  CHECK(exp == 8);
}

TEST_CASE("Drop & take from generator") {
  auto gen = take(drop(fpgen::inc((size_t)0), 4), 9);
  for (size_t exp = 4; exp < 13; exp++) {
    CHECK(static_cast<bool>(gen));
    CHECK(exp == gen());
  }

  for (auto v : gen) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Dropwhile over an empty generator") {
  auto gen = drop_while(manip_empty(), [](size_t v) { return v > 3; });
  for (auto v : gen) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Dropwhile over a generator") {
  auto gen = drop_while(until12(), [](size_t v) { return v < 5; });
  size_t exp = 5;
  for (auto v : gen) {
    CHECK(v == exp);
    CHECK(exp <= 12);
    exp++;
  }
  CHECK(exp == 13);
}

TEST_CASE("Takewile over an empty generator") {
  auto gen = take_while(manip_empty(), [](size_t v) { return v < 4; });
  for (auto v : gen) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Takewhile over a generator") {
  auto gen = take_while(fpgen::inc((size_t)0), [](size_t v) { return v < 8; });
  size_t exp = 0;
  for (auto v : gen) {
    CHECK(v == exp);
    CHECK(exp <= 8);
    exp++;
  }
  CHECK(exp == 8);
}
