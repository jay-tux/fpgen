#include <utility>
#include <vector>
#include "doctest/doctest.h"
#include "stream.hpp"
#include "sources.hpp"

std::vector<int> primes{2,3,5,7,11,13,17,19,23,31};

fpgen::generator<int> str_empty() { co_return; }

fpgen::generator<char> chgen() {
  for(char c = 'a'; c <= 'z'; c++) co_yield c;
}

fpgen::generator<int> square_gen(fpgen::generator<int> g) {
  for(auto v : g) co_yield v*v;
}

fpgen::generator<int> halve_gen(fpgen::generator<int> g) {
  for(auto v : g) co_yield v/2;
}

TEST_CASE("Stream accepts empty generator") {
  fpgen::stream<int, int, fpgen::generator<int>, void> strm(str_empty());
  for(auto val : strm) {
    CHECK(false); // should fail
  }
}

TEST_CASE("Stream can run over normal generator") {
  fpgen::stream<char, char, fpgen::generator<char>, void> strm(chgen());
  char curr = 'a';
  for(auto ch : strm) {
    CHECK_EQ(curr, ch);
    CHECK_LE(curr, 'z');
    curr++;
  }
  CHECK_EQ(curr, 'z'+1);
}

TEST_CASE("Stream can accept map by function") {
  auto prestrm = to_stream(std::move(fpgen::from(primes)));
  auto &strm = prestrm >> square_gen;
  auto it = primes.begin();

  for(auto v : (strm)) {
    CHECK_EQ(v, (*it)*(*it));
    CHECK_NE(it, primes.end());
    ++it;
  }
  CHECK_EQ(it, primes.end());
}

TEST_CASE("Stream can accept multiple maps by function") {
  auto prestrm = to_stream(std::move(fpgen::from(primes)));
  auto &strm = prestrm >> square_gen >> halve_gen;
  auto it = primes.begin();

  while(strm) {
    auto v = strm();
    CHECK_EQ(v, (*it)*(*it)/2);
    CHECK_NE(it, primes.end());
    ++it;
  }
  CHECK_EQ(it, primes.end());
}
