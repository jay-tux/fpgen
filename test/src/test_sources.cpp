#include "generator.hpp"
#include "sources.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <map>
#include <set>
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
