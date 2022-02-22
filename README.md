# fpgen
*Functional programming in C++ using C++20 coroutines*  
![](https://img.shields.io/badge/test_coverage-96.9%25-brightgreen)


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

Got another idea? Drop a feature request on the repo.

## Building
The build system for fpgen is GNU make. The following targets are available:

 Target name | Usage
 --- | ---
 `all` | Show all targets (Default target)
 `install` | installs the headers under
 `uninstall` | uninstalls the headers
 `docs` | generates the documentation using doxygen
 `test` | builds and runs the tests
 `clean` | cleans up test builds and documentation
 `coverage` | builds and runs the tests, then generates a coverage report

Some targets provide parameters you can overload:

 Parameter name | Usage | Default value | Applicable targets
 --- | --- | --- | ---
 `CC` | The C++ compiler | `g++` | test, coverage
 `EXTRA_CXX` | Additional C++ compilation arguments |  | test, coverage
 `EXTRA_LD` | Additional C++ linking arguments |  | test, coverage
 `BUILD_DIR` | Object directory for tests | `./test/obj` | test, coverage
 `BIN_DIR` | Test binary directory | `./test/bin` | test, coverage
 `TEST_DIR` | Test source directory | `./test/src` | test, coverage
 `INSTALL_DIR` | Header installation directory | `/usr/include/fpgen` | install, uninstall
 `INCL_PATH` | Directory containing the headers | `./inc/` | install, docs, test, coverage
 `DOC_DIR` | Documentation output directory | `./docs/` | docs
 `BROWSER` | Default browser for (HTML) docs and coverage reports | `firefox` | docs, coverage
 `HTMLDIR` | Output directory for HTML coverage reports | `./cov/` | coverage

## Requirements
This project strongly depends on C++20. For an optimal experience, I recommend GCC version 11.2 or greater.  
For the tests, we rely on Google Test via the Conan package manager, so make sure you have that installed as well.  
To generate coverage reports, we require `gcov`, `lcov` and `genhtml`.
