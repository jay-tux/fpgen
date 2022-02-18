#ifndef _FPGEN_STREAM
#define _FPGEN_STREAM

#include "generator.hpp"
#include "sources.hpp"

namespace fpgen {
template <typename T> struct remove_generator {};
template <typename T> struct remove_generator<generator<T>> { using type = T; };

template <typename T> class stream {
public:
  stream(generator<T> gen) : source{std::move(gen)} {}
  stream(const stream<T> &other) = delete;
  stream(stream<T> &&other) { *this = std::move(other); }

  stream &operator=(const stream<T> &other) = delete;
  stream &operator=(stream<T> &&other) { source = std::move(other); }

  template <typename Func> auto operator<<(Func f) {
    return stream<typename remove_generator<
        std::invoke_result_t<Func, generator<T>>>::type>{f(std::move(source))};
  }

private:
  generator<T> source;
};
} // namespace fpgen

#endif
