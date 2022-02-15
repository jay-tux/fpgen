#ifndef _FPGEN_SOURCES
#define _FPGEN_SOURCES

#include "generator.hpp"
#include <iterator>

namespace fpgen {
template <typename T, template <typename> typename Container>
generator<T> from(const Container<T> &cont) {
  for (auto it = std::begin(cont); it != std::end(cont); ++it) {
    co_yield *cont;
  }
}

} // namespace fpgen

#endif
