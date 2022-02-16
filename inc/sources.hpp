#ifndef _FPGEN_SOURCES
#define _FPGEN_SOURCES

#include "generator.hpp"
#include <iostream>
#include <iterator>

/**
 *  \brief The namespace containing all of fpgen's code.
 */
namespace fpgen {
/**
 *  \brief Creates a generator over a data source.
 *
 *  The data source should have an iterator (using `std::begin` and `std::end`).
 * For most builtin containers (`std::vector`, ...) this is already satisfied.
 * For `std::map`, see fpgen::from_tup.
 *
 *  \tparam T The type contained in the container.
 *  \tparam TArgs Any other template parameters passed to the container.
 *  \tparam Container The container type.
 *  \param[in] cont The container to iterate over.
 *  \returns A new generator which will iterate over the container.
 *  \see fpgen::from_tup
 */
template <typename T, typename... TArgs,
          template <typename...> typename Container>
generator<T> from(const Container<T, TArgs...> &cont) {
  for (auto it = std::begin(cont); it != std::end(cont); ++it) {
    co_yield *it;
  }
}

/**
 *  \brief Creates a generator over an associative data source.
 *
 *  The data source should have an iterator (using `std::begin` and `std::end`).
 * For most builtin containers (`std::map`, ...) this is already satisfied.
 * The container should have two type arguments. For single-type containers, see
 * fpgen::from.
 *
 *  \tparam TKey The first type contained in the container (key type).
 *  \tparam TValue The second type contained in the container (value type).
 *  \tparam TArgs Any other template parameters passed to the container.
 *  \tparam Container The container type.
 *  \param[in] cont The container to iterate over.
 *  \returns A new generator which will iterate over the container.
 *  \see fpgen::from
 */
template <typename TKey, typename TVal, typename... TArgs,
          template <typename...> typename Container>
generator<std::tuple<TKey, TVal>>
from_tup(const Container<TKey, TVal, TArgs...> &cont) {
  for (auto it = cont.begin(); it != cont.end(); ++it) {
    co_yield *it;
  }
}

/**
 *  \brief Creates an infinitely incrementing generator.
 *
 *  The generator is contstructed by continuously incrementing (a copy of) the
 * given value. While mainly meant for integral types, any type supporting
 * operator++() (the prefix increment operator) can be used. The first value
 * returned is the start value itself.
 *
 *  \tparam T The type to increment.
 *  \param[in] start The initial value.
 *  \returns An infinite generator which increments a value.
 */
template <typename T> generator<T> inc(T start) {
  T value = start;
  while (true) {
    co_yield value;
    ++value;
  }
}

} // namespace fpgen

#endif
