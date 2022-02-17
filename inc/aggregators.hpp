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

/**
 *  \brief Accumulates each value in the generator using the provided function.
 *
 *  For each element in the generator, the provided function (which should have
 * (TOut, TIn) -> TOut) as signature is called. The result is stored in the
 * accumulator, which is passed down to the next value in the generator. Once
 * all values are extracted, the resulting accumulator is returned. The
 * accumulator is initialized using `TOut value = {};`.
 *
 *  \tparam TOut The output type (accumulator type).
 *  \tparam TIn The input type (type contained in the generator).
 *  \tparam Fun The function type (should have the signature (TOut, TIn) ->
 * TOut).
 *  \param[in,out] gen The generator to fold.
 *  \param[in] folder The folding function.
 *  \returns The final accumulator value.
 */
template <typename TOut, typename TIn, typename Fun>
TOut fold(generator<TIn> &gen, Fun folder) {
  TOut value = {};
  while (gen) {
    value = folder(value, gen());
  }
  return value;
}

/**
 *  \brief Accumulates each value in the generator using the provided function.
 *
 *  For each element in the generator, the provided function (which should have
 * (TOut, TIn) -> TOut) as signature is called. The result is stored in the
 * accumulator, which is passed down to the next value in the generator. Once
 * all values are extracted, the resulting accumulator is returned. The
 * accumulator is initialized using `TOut value(initial);`.
 *
 *  \tparam TOut The output type (accumulator type).
 *  \tparam TIn The input type (type contained in the generator).
 *  \tparam Fun The function type (should have the signature (TOut, TIn) ->
 * TOut).
 *  \param[in,out] gen The generator to fold.
 *  \param[in] folder The folding function.
 *  \param[in] initial The initial value for the accumulator (is copied).
 *  \returns The final accumulator value.
 */
template <typename TOut, typename TIn, typename Fun>
TOut fold(generator<TIn> &gen, Fun folder, TOut initial) {
  TOut value(initial);
  while (gen) {
    value = folder(value, gen());
  }
  return value;
}

/**
 *  \brief Accumulates each value in the generator using the provided function.
 *
 *  For each element in the generator, the provided function (which should have
 * (TOut, TIn) -> TOut) as signature is called. The result is stored in the
 * provided accumulator, which is passed down to the next value in the
 * generator. Once all values are extracted, the resulting accumulator is
 * returned. Each step modifies the accumulator.
 *
 *  \tparam TOut The output type (accumulator type).
 *  \tparam TIn The input type (type contained in the generator).
 *  \tparam Fun The function type (should have the signature (TOut, TIn) ->
 * TOut or (TOut &, TIn) -> TOut).
 *  \param[in,out] gen The generator to fold.
 *  \param[in] folder The folding function.
 *  \param[out] initial The initial value for the accumulator (is overwritten).
 *  \returns A reference to the value which was passed as initial value and is
 * now the output value.
 */
template <typename TOut, typename TIn, typename Fun>
TOut &fold_ref(generator<TIn> &gen, Fun folder, TOut &initial) {
  while (gen) {
    initial = folder(initial, gen());
  }
  return initial;
}

/**
 *  \brief Sums each value in the generator.
 *
 *  The type contained in the generator should support `operator+`. An initial
 * accumulator is constructed using `T accum = {}`. Each next value is added to
 * the accumulator, which is returned afterwards.
 *
 *  \tparam T The type contained in the generator, should support `operator+`.
 *  \param[in,out] gen The generator to sum over.
 *  \returns The sum of all elements.
 */
template <typename T> T sum(generator<T> &gen) {
  T accum = {};
  while (gen) {
    accum = accum + gen();
  }
  return accum;
}

/**
 *  \brief Loops over the generator, calling a function on each element in it.
 *
 *  Each element is extracted from the generator and used only as function
 * argument. All other aggregators can be written as a combination of this
 * function and a lambda function. Afterwards, the generator will be empty.
 *
 *  \tparam T The type of values contained in the generator.
 *  \tparam Fun The function type of the callback.
 *  \param[in,out] gen The generator to iterate over.
 *  \param[in] func The function to use.
 */
template <typename T, typename Fun> void foreach (generator<T> &gen, Fun func) {
  while (gen) {
    func(gen());
  }
}
} // namespace fpgen

#endif
