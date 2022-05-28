/////////////////////////////////////////////////////////////////////////////
// Name:        sources.hpp
// Purpose:     functions to create fpgen generators from certain sources.
// Author:      jay-tux
// Copyright:   (c) 2022 jay-tux
// Licence:     MPL
/////////////////////////////////////////////////////////////////////////////
#ifndef _FPGEN_SOURCES
#define _FPGEN_SOURCES

#include <istream>
#include <iterator>
#include <type_traits>
#include <string>
#include <tuple>
#include "generator.hpp"
#include "type_traits.hpp"

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
 *  \see fpgen::from_tup, fpgen::enumerate
 */
template <typename T, typename... TArgs,
          template <typename...> typename Container>
generator<T> from(const Container<T, TArgs...> &cont) {
  for (auto it = std::begin(cont); it != std::end(cont); ++it) {
    co_yield *it;
  }
  co_return;
}

/**
 *  \brief Creates a generator over a data source, with indexing.
 *
 *  The data source does not have to allow indexing, but it is recommended for
 * repeatable behaviour. If the index is not needed, use fpgen::from. The data
 * source should support iterating (using `std::begin` and `std::end`).
 *
 *  \tparam T The type contained in the container.
 *  \tparam TArgs Any other template parameters passed to the container.
 *  \tparam Container The container type.
 *  \param[in] cont The container to iterate over.
 *  \returns A new generator which will iterate over the container using index
 * and value.
 * \see fpgen::from_tup, fpgen::enumerate
 */
template <typename T, typename... TArgs,
          template <typename...> typename Container>
generator<std::tuple<size_t, T>> enumerate(const Container<T, TArgs...> &cont) {
  size_t i = 0;
  for (auto it = std::begin(cont); it != std::end(cont); ++it) {
    co_yield {i, *it};
    i++;
  }
  co_return;
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
  co_return;
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

/**
 *  \brief Creates a generator from an input stream.
 *
 *  The generator will apply the given function on the stream each time the
 * generator is invoked. The function should return a value of the type you want
 * in the generator. Once the stream fails (`!stream.good()`) or the stream
 * reaches EOF, the generator stops. Trailing whitespace may result in
 * unpredictable behaviour. Since the stream isn't copied, using the generator
 * after the stream goes out of scope is undefined behaviour.
 *
 *  \tparam Fun The type of the function. Should have the signature
 * (std::istream &) -> T.
 *  \tparam TOut The output type. This type is deduced from the `Fun` type
 * parameter.
 *  \param[in,out] stream The input stream.
 *  \param[in] func The function to extract data from the stream.
 *  \returns A new generator which will iterate over the given stream, each time
 * applying the given function to retrieve the next value.
 */
template <typename Fun, typename TOut = type::output_type<Fun, std::istream &>,
          typename _ = type::is_function_to<Fun, TOut, std::istream &>>
generator<TOut> from_stream(std::istream &stream, Fun func) {
  while (stream.good() && !stream.eof()) {
    co_yield func(stream);
  }
  co_return;
}

/**
 *  \brief Creates a generator supplying each line from a file.
 *
 *  This generator is formed by combining `fpgen::from_stream<>` with
 * `std::getline` on the stream.
 *
 *  \param[in,out] stream The stream to extract lines from.
 *  \returns A new generator which yields each line in the stream.
 */
inline generator<std::string> from_lines(std::istream &stream) {
  return from_stream(stream, [](std::istream &stream) {
    std::string value;
    std::getline(stream, value);
    return value;
  });
}

} // namespace fpgen

#endif
