#include "generator.hpp"
#include "sources.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

TEST(sources, from_vector) {
  std::vector<int> values = {0, 5, 1, 4, 2, 3};
  size_t idx = 0;
  for (auto v : fpgen::from(values)) {
    EXPECT_EQ(values[idx], v);
    idx++;
  }
}

TEST(sources, from_set) {
  std::set<std::string> srcs = {"key 1", "key 2", "key 3", "something"};
  std::set<std::string> todo = {"key 1", "key 2", "key 3", "something"};

  for (auto v : fpgen::from(srcs)) {
    EXPECT_NE(todo.find(v), todo.end());
    todo.erase(v);
  }
  EXPECT_TRUE(todo.empty());
}

TEST(sources, enumerate_vector) {
  std::vector<char> values = {'a', 'c', 'e', 'k', 'j', 't'};
  size_t prev = 0;
  for (auto v : fpgen::enumerate(values)) {
    EXPECT_EQ(std::get<0>(v), prev);
    EXPECT_EQ(values[prev], std::get<1>(v));
    prev++;
  }
}

TEST(sources, from_map_tup) {
  std::map<std::string, std::string> map = {{"key 1", "value 1"},
                                            {"key 2", "value 2"},
                                            {"key 3", "value 3"},
                                            {"something", "else"}};
  std::set<std::string> todo = {"key 1", "key 2", "key 3", "something"};
  for (auto v : fpgen::from_tup(map)) {
    EXPECT_NE(todo.find(std::get<0>(v)), todo.end());
    EXPECT_EQ(map[std::get<0>(v)], std::get<1>(v));
    todo.erase(std::get<0>(v));
  }
  EXPECT_TRUE(todo.empty());
}

TEST(sources, incrementable) {
  auto gen = fpgen::inc<int>(0);
  for (int i = 0; i < 25; i++) {
    EXPECT_EQ(gen(), i);
  }
}

TEST(sources, incrementable_struct) {
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
    EXPECT_EQ(gen().value, i);
  }
}

TEST(sources, instream) {
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
    EXPECT_TRUE(genstate);
    auto tmp = gen();
    EXPECT_EQ(tmp, numbers[i]);
    std::cout << i << "," << tmp << std::endl;
  }

  bool genstate = gen;
  EXPECT_FALSE(genstate);
}

TEST(sources, lipsum_lines) {
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
  EXPECT_EQ("", gen());
  for (size_t i = 0; i < 10; i++) {
    bool gens = gen;
    EXPECT_TRUE(gens);
    EXPECT_EQ(gen(), lines[i]);
  }
  EXPECT_EQ(gen(), "");
  bool gens = gen;
  EXPECT_FALSE(gens);
}
