/////////////////////////////////////////////////////////////////////////////
// Name:        stream.hpp
// Purpose:     wrapper around raw generators (utility).
// Author:      jay-tux
// Copyright:   (c) 2022 jay-tux
// Licence:     MPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FPGEN_STREAM
#define _FPGEN_STREAM

#include <any>
#include <type_traits>
#include <utility>
#include "generator.hpp"

namespace fpgen {

template <typename TIn, typename TOut, typename Src, typename Op>
class stream {};

namespace __int {
template <typename T> struct remove_generator {};
template <typename T> struct remove_generator<generator<T>> {
  using type = T;
};

template <typename F, typename O>
using __inv = typename remove_generator<
    typename std::invoke_result<F, generator<O>>::type>::type;

template <typename S>
using __nop =
    typename std::remove_cv<typename std::remove_pointer<S>::type>::type;

template <typename TOut, typename Op, typename Self>
using ret = stream<TOut, __inv<Op, TOut>, __nop<Self>, Op>;

template <typename T> generator<T> empty() { co_return; }
} // namespace __int

template <typename TIn> class stream<TIn, TIn, generator<TIn>, void> {
public:
  using iter_t = typename generator<TIn>::iterator_type;
  using source_t = generator<TIn>;
  using input_t = TIn;
  using output_t = TIn;

  explicit stream(source_t &&source) : src{__int::empty<TIn>()} {
    std::swap(src, source);
  }

  operator bool() { return src.operator bool(); }
  TIn operator()() { return src(); }

  template <typename Op>
  auto operator>>(Op call) -> __int::ret<output_t, Op, decltype(this)> & {
    auto *val =
        new __int::ret<output_t, Op, decltype(this)>(std::move(*this), call);
    val->was_allocd = true;
    return *val;
  }

  iter_t begin() { return src.begin(); }
  iter_t end() { return src.end(); }

private:
  generator<TIn> src;
};

template <typename TIn, typename TOut, typename Op, typename _SubIn,
          typename _SubCtr, typename _SubOp>
class stream<TIn, TOut, stream<_SubIn, TIn, _SubCtr, _SubOp>, Op> {
public:
  using self_t = stream<TIn, TOut, stream<_SubIn, TIn, _SubCtr, _SubOp>, Op>;
  using iter_t = typename generator<TOut>::iterator_type;
  using source_t = stream<_SubIn, TIn, _SubCtr, _SubOp>;
  using input_t = TIn;
  using output_t = TOut;
  using operation_type = Op;

  explicit stream(self_t &) = delete;
  explicit stream(self_t &&) = default;

  stream(source_t &&source, Op callback)
      : src{std::move(source)}, gen{callback(inner_gen())} {}

  self_t &operator=(self_t &) = delete;
  self_t &operator=(self_t &&) = default;

  operator bool() { return gen.operator bool(); }
  TIn operator()() { return gen(); }

  iter_t begin() { return gen.begin(); }
  iter_t end() { return gen.end(); }

  template <typename Nxt>
  auto operator>>(Nxt call) -> __int::ret<output_t, Nxt, decltype(this)> & {
    auto *val =
        new __int::ret<output_t, Nxt, decltype(this)>(std::move(*this), call);
    val->was_allocd = true;
    return *val;
  }

  ~stream() {
    if(was_allocd) {
      delete this;
    }
  }

private:
  generator<TIn> inner_gen() {
    for (auto t : src) {
      co_yield t;
    }
  }

  source_t src;
  generator<TOut> gen;
  bool was_allocd;

  template <typename FTIn, typename FTOut, typename FSrc, typename FOp> friend class stream;
};

template <typename TIn>
stream<TIn, TIn, generator<TIn>, void> to_stream(generator<TIn> &&g) {
  return stream<TIn, TIn, generator<TIn>, void>(std::move(g));
}
} // namespace fpgen

#endif
