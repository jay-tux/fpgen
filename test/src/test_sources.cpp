#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "doctest/doctest.h"
#include "generator.hpp"
#include "sources.hpp"


TEST_CASE("Generator from std::vector") {
  std::vector<int> values = {0, 5, 1, 4, 2, 3};
  size_t idx = 0;
  for (auto v : fpgen::from(values)) {
    CHECK(values[idx] == v);
    idx++;
  }
}

TEST_CASE("Generator from std::set") {
  std::set<std::string> srcs = {"key 1", "key 2", "key 3", "something"};
  std::set<std::string> todo = {"key 1", "key 2", "key 3", "something"};

  for (auto v : fpgen::from(srcs)) {
    CHECK(todo.find(v) != todo.end());
    todo.erase(v);
  }
  CHECK(todo.empty());
}

TEST_CASE("Generator from enumerate over std::vector") {
  std::vector<char> values = {'a', 'c', 'e', 'k', 'j', 't'};
  size_t prev = 0;
  for (auto v : fpgen::enumerate(values)) {
    CHECK(std::get<0>(v) == prev);
    CHECK(values[prev] == std::get<1>(v));
    prev++;
  }
}

TEST_CASE("Generator from std::map") {
  std::map<std::string, std::string> map = {{"key 1", "value 1"},
                                            {"key 2", "value 2"},
                                            {"key 3", "value 3"},
                                            {"something", "else"}};
  std::set<std::string> todo = {"key 1", "key 2", "key 3", "something"};
  for (auto v : fpgen::from_tup(map)) {
    CHECK(todo.find(std::get<0>(v)) != todo.end());
    CHECK(map[std::get<0>(v)] == std::get<1>(v));
    todo.erase(std::get<0>(v));
  }
  CHECK(todo.empty());
}

TEST_CASE("Incrementable generator (int)") {
  auto gen = fpgen::inc<int>(0);
  for (int i = 0; i < 25; i++) {
    CHECK(gen() == i);
  }
}

TEST_CASE("Incrementable generator (struct)") {
  struct inc_struct {
    int value;
    inline inc_struct operator++() {
      value++;
      return {value};
    }
    inline inc_struct operator++(int) {
      inc_struct next = {value};
      value++;
      return next;
    }
  } v1;
  v1.value = 0;

  auto gen = fpgen::inc<inc_struct>(v1);
  for (int i = 0; i < 25; i++) {
    CHECK(gen().value == i);
  }
}

TEST_CASE("Generator from std::istream") {
  std::vector<int> numbers = {1, 2, 3, 4, 5};
  std::stringstream str;
  for (auto v : numbers)
    str << " " << v;
  auto func = [](std::istream &strm) {
    int i;
    strm >> i;
    return i;
  };

  auto gen = fpgen::from_stream(str, func);
  for (int i = 0; i < 5; i++) {
    bool genstate = gen;
    CHECK(genstate);
    auto tmp = gen();
    CHECK_EQ(tmp, numbers[i]);
  }

  bool genstate = gen;
  CHECK(!genstate);
}

TEST_CASE("Generator from std::istream, by lines") {
  std::string lipsum = R"(
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque diam magna,
laoreet non dictum eget, scelerisque eu nibh. Cras luctus purus sit amet
sodales aliquet. Proin vulputate risus quam. Curabitur ultricies, elit nec
pharetra accumsan, leo eros mollis nibh, pulvinar lobortis dolor diam non quam.
Vivamus odio arcu, aliquet ornare leo quis, mollis porta nisl. Mauris malesuada
semper efficitur. Vestibulum nulla diam, hendrerit in diam a, tempor dignissim
turpis. Maecenas eleifend laoreet velit id semper. Aliquam quis mattis enim.
Cras gravida, felis vitae porta auctor, magna purus aliquet lorem, ut maximus
tortor tortor sit amet mauris. Mauris eleifend enim eget arcu blandit auctor.
Etiam vel porta augue. Maecenas volutpat odio in lacus sagittis fermentum.
)";
  std::string lines[] = {"Lorem ipsum dolor sit amet, consectetur adipiscing "
                         "elit. Quisque diam magna,",
                         "laoreet non dictum eget, scelerisque eu nibh. Cras "
                         "luctus purus sit amet",
                         "sodales aliquet. Proin vulputate risus quam. "
                         "Curabitur ultricies, elit nec",
                         "pharetra accumsan, leo eros mollis nibh, pulvinar "
                         "lobortis dolor diam non quam.",
                         "Vivamus odio arcu, aliquet ornare leo quis, mollis "
                         "porta nisl. Mauris malesuada",
                         "semper efficitur. Vestibulum nulla diam, hendrerit "
                         "in diam a, tempor dignissim",
                         "turpis. Maecenas eleifend laoreet velit id semper. "
                         "Aliquam quis mattis enim.",
                         "Cras gravida, felis vitae porta auctor, magna purus "
                         "aliquet lorem, ut maximus",
                         "tortor tortor sit amet mauris. Mauris eleifend enim "
                         "eget arcu blandit auctor.",
                         "Etiam vel porta augue. Maecenas volutpat odio in "
                         "lacus sagittis fermentum."};
  std::stringstream strm;
  strm << lipsum;
  auto gen = fpgen::from_lines(strm);
  CHECK_EQ("", gen());
  for (size_t i = 0; i < 10; i++) {
    bool gens = gen;
    CHECK(gens);
    CHECK(gen() == lines[i]);
  }
  CHECK_EQ(gen(), "");
  bool gens = gen;
  CHECK(!gens);
}
