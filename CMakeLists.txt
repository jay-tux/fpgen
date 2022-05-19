cmake_minimum_required(VERSION 3.15)
project(fpgen CXX)

add_library(fpgen PUBLIC)
target_include_directories(fpgen PUBLIC inc)

set_target_properties(
  fpgen
  PROPERTIES PUBLIC_HEADER
  "inc/fpgen.hpp" "inc/aggregators.hpp" "inc/generator.hpp"
  "inc/manipulators.hpp" "inc/sources.hpp" "inc/type_traits.hpp"
)

install(TARGETS fpgen)
