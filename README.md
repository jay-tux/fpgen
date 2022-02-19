# fpgen
*Functional programming in C++ using C++20 coroutines*  
![](https://img.shields.io/badge/test_coverage-97%25-brightgreen)


## Aim
`fpgen` aims to bring you an easy-to-use framework for stream programming in C++. Generators can be created, manipulated and lazily aggregated at will using a set of simple functions. Iterators over the generator make it easy to iterate over lazy functions.

## Features
Currently supported features:
 - Templated generator type with iterators, supporting any coroutine.
 - Commonly used sources:
   - Create generators from `std::` containers with a single type argument, with and without indexing.
   - Create generators from `std::` containers with two type arguments.
   - Create generators from incrementable types (using `operator++(void)`).
 - Commonly used manipulators:
   - Lazy `map`ping over generators.
   - Lazy `zip`ping of generators.
   - Lazy `filter`ing of generators.
 - Commonly used aggregators:
   - Lazy `fold`ing of generators.
   - Lazy `sum`ming of generators.

Planned/In progress features:
 - Ergonomic improvements:
   - Enable streaming `<<` of generators.
   - Simple structure which allows generator construction, manipulation and aggregation using member functions.

Got another idea? Drop a feature request on the repo.

## Requirements
This project strongly depends on C++20. For an optimal experience, I recommend GCC version 11.2 or greater.  
For the tests, we rely on Google Test via the Conan package manager, so make sure you have that installed as well.  
To generate coverage reports, we require `gcov`, `lcov` and `genhtml`.
