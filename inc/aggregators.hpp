#ifndef _FPGEN_AGGREGATORS
#define _FPGEN_AGGREGATORS

#include "generator.hpp"
#include <forward_list>
#include <tuple>
#include <type_traits>

namespace fpgen {
template <typename T, typename... Args,
          template <typename...> typename Container>
Container<T, Args...> &aggregate_to(generator<T> &gen,
                                    Container<T, Args...> &out) {
  while (gen) {
    out.push_back(gen());
  }
  return out;
}

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
} // namespace fpgen

#endif
