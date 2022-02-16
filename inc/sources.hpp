#ifndef _FPGEN_SOURCES
#define _FPGEN_SOURCES

#include "generator.hpp"
#include <iostream>
#include <iterator>

namespace fpgen {
template <typename T, typename... TArgs,
          template <typename...> typename Container>
generator<T> from(const Container<T, TArgs...> &cont) {
  for (auto it = cont.begin(); it != cont.end(); ++it) {
    co_yield *it;
  }
}

template <typename TKey, typename TVal, typename... TArgs,
          template <typename...> typename Container>
generator<std::tuple<TKey, TVal>>
from_tup(const Container<TKey, TVal, TArgs...> &cont) {
  for (auto it = cont.begin(); it != cont.end(); ++it) {
    co_yield *it;
  }
}

template <typename T> generator<T> inc(T start) {
  T value = start;
  while (true) {
    co_yield value;
    ++value;
  }
}

} // namespace fpgen

#endif
