#ifndef _FPGEN_AGGREGATORS
#define _FPGEN_AGGREGATORS

#include "generator.hpp"
#include <forward_list>
#include <tuple>
#include <type_traits>

/**
 *  \brief The namespace containing all of fpgen's code.
 */
namespace fpgen {
/**
 *  \brief Aggregates all data in the generator to a dataset.
 *
 *  The supplied container should support `push_back` (like most of the `std::`
 * containers). Each element is extracted from the generator and inserted into
 * the container. The container is not cleared before inserting. Elements
 * already extracted (due to previous calls to the generator, ...) cannot be
 * reconstructed.
 *
 *  \tparam T The type contained in the container and generator.
 *  \tparam Args Other parameters to be passed to the container.
 *  \tparam Container The container type to output to.
 *  \param[in, out] gen The generator to extract from.
 *  \param[out] out The container to output to.
 *  \returns A reference to the modified container.
 */
template <typename T, typename... Args,
          template <typename...> typename Container>
Container<T, Args...> &aggregate_to(generator<T> &gen,
                                    Container<T, Args...> &out) {
  while (gen) {
    out.push_back(gen());
  }
  return out;
}

/**
 *  \brief Aggregates all data in the generator into an associative container.
 *
 *  The supplied container should support `operator[]` (by reference, like
 * `std::map`). Duplicate key values will be be overwritten.  The container is
 * not cleared before writing. Elements that have already been extracted from
 * the generator cannot be reconstructed.
 *
 *  \tparam TKey The key type for the container.
 *  \tparam TValue The value type for the container.
 *  \tparam Args Other parameters to be passed to the container.
 *  \tparam Container The container to be used.
 *  \param[in, out] gen The generator to extract from.
 *  \param[out] out The container to insert to.
 *  \returns A reference to the modified container.
 */
template <typename TKey, typename TVal, typename... Args,
          template <typename...> typename Container>
Container<TKey, TVal, Args...> &
tup_aggregate_to(generator<std::tuple<TKey, TVal>> &gen,
                 Container<TKey, TVal, Args...> &out) {
  while (gen) {
    std::tuple<TKey, TVal> tup = gen();
    out[std::get<0>(tup)] = std::get<1>(tup);
  }
  return out;
}

/**
 *  \brief Counts the amount of elements in the generator.
 *
 *  Each element is extracted from the generator. These values are not
 * recoverable. Only values left in the generator are counted. Afterwards, the
 * generator will be empty.
 *
 *  \tparam T The type of values contained in the generator.
 *  \param[in,out] gen The generator to iterate over.
 *  \returns The amount of elements in the generator.
 */
template <typename T> size_t count(generator<T> &gen) {
  size_t cnt = 0;
  while (gen) {
    gen();
    cnt++;
  }
  return cnt;
}
} // namespace fpgen

#endif
