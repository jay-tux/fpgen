#ifndef _FPGEN_MANIP
#define _FPGEN_MANIP

#include "generator.hpp"
#include <array>
#include <type_traits>

/**
 *  \brief The namespace containing all of fpgen's code.
 */
namespace fpgen {
/**
 *  \brief Maps a function over a generator.
 *
 *  Creates a new generator whose values are the transformed values generated by
 * applying the given mapping function on each value in the original generator.
 * Using the provided generator after calling fpgen::map is undefined behaviour.
 *
 *  \tparam TIn The type contained in the provided generator.
 *  \tparam Fun The function signature of the mapping function.
 *  \param[in,out] gen The generator to map over. Will be in unusable state
 * afterwards.
 *  \param[in] func The function to map with.
 *  \returns A new generator whose contained type is the return type of the
 * mapping function.
 */
template <typename TIn, typename Fun>
auto map(generator<TIn> &gen, Fun func)
    -> generator<typename std::invoke_result<Fun, TIn>::type> {
  while (gen) {
    co_yield func(gen());
  }
  co_return;
}

/**
 *  \brief Combines two generators into a single generator.
 *
 *  The result is a tuple of values, one taken from each generator. Once one of
 * the generators runs out of values, the newly generator stops as well. Using
 * either generator after calling this function is undefined behaviour.
 *
 *  \tparam T1 The type contained in the first generator.
 *  \tparam T2 The type contained in the second generator.
 *  \param[in, out] gen1 The first generator to use.
 *  \param[in, out] gen2 The second generator to use.
 *  \returns A new generator containing tuples of values from both generators.
 */
template <typename T1, typename T2>
generator<std::tuple<T1, T2>> zip(generator<T1> &gen1, generator<T2> &gen2) {
  while (gen1 && gen2) {
    co_yield {gen1(), gen2()};
  }
  co_return;
}

/**
 *  \brief Filters a generator, keeping only values matching a predicate.
 *
 *  The result is a generator with as much or possibly fewer elements. Each
 * element is an element in the original generator. Elements not matching the
 * predicate are removed.
 *
 *  \tparam T The type contained in the generator.
 *  \tparam Pred The function type of the predicate function. Should return a
 * bool.
 *  \param gen The generator containing the original values.
 *  \param p The predicate.
 *  \returns A new generator which yields all values in the original generator
 * except those not matching the predicate.
 */
template <typename T, typename Pred>
generator<T> filter(generator<T> &gen, Pred p) {
  while (gen) {
    T val(gen());
    if (p(val))
      co_yield val;
  }
  co_return;
}
} // namespace fpgen

#endif
