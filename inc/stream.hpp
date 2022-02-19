#ifndef _FPGEN_STREAM
#define _FPGEN_STREAM

#include "generator.hpp"
#include "sources.hpp"
#include <variant>

namespace fpgen {
template <typename T> struct remove_generator {};
template <typename T> struct remove_generator<generator<T>> { using type = T; };

template <typename T> class stream {
public:
  stream(generator<T> gen) : source{gen} {}

  template <typename... TArgs, template <typename...> typename Container>
  Container<T, TArgs...> &operator>>(Container<T, TArgs...> &cont) {
    return aggregate_to(source, cont);
  }

  operator generator<T> &() { return source; }
  generator<T> &get_generator() { return source; }

private:
  generator<T> source;
};
} // namespace fpgen

#endif
