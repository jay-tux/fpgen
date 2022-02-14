#ifndef _FPGEN_GENERATOR
#define _FPGEN_GENERATOR

#ifdef __clang__
#include <concepts>
#include <experimental/coroutine>
namespace std {
using namespace experimental;
}
#else
#include <concepts>
#include <coroutine>
#endif

#include <exception>

namespace fpgen {
#ifdef __clang__
template <typename T>
using enabler =
    typename std::enable_if<std::is_copy_assignable<T>::value, bool>::type;

template <typename T, enabler<T> = true> class generator {
#else
template <std::copyable T> class generator {
#endif
public:
  struct promise_type {
    using value_type = T;
    using except_type = std::exception_ptr;
    using gen_type = generator<T>;
    using suspend_type = std::suspend_always;

    value_type value;
    except_type ex;

    gen_type get_return_object() { return gen_type(*this); }
    suspend_type initial_suspend() { return {}; }
    suspend_type final_suspend() noexcept { return {}; }
    void return_value(value_type v) { value = v; }
    suspend_type yield_value(value_type v) {
      value = v;
      return {};
    }

    void unhandled_exception() { ex = std::current_exception(); }
  };
  struct iterator_type {
    using gen_t = generator<T>;
    using iter_t = iterator_type;
    using value_t = T;

    gen_t &source;
    bool is_finished;
    value_t value;

    iterator_type(gen_t &source, bool is_finised)
        : source{source}, is_finished{is_finised} {
      if (!is_finised)
        ++(*this);
    }
    iterator_type(gen_t &source, bool is_finished, value_t value)
        : source{source}, is_finished{is_finished}, value{value} {}

    iter_t operator++() {
      if (!source)
        is_finished = true;
      value = source();
      return {source, is_finished, value};
    }
    operator bool() { return static_cast<bool>(source); }
    bool operator!=(iter_t &other) { return is_finished != other.is_finished; }
    value_t operator*() { return value; }
    operator value_t() { return value; }
  };

  using handle_type = std::coroutine_handle<promise_type>;
  using value_type = T;

  generator(promise_type &p) : _h{handle_type::from_promise(p)} {}
  operator handle_type() const { return _h; }
  operator bool() const { return !_h.done(); }

  iterator_type begin() { return {*this, false}; }
  iterator_type end() { return {*this, true}; }

  value_type operator()() {
    if (*this)
      _h();
    if (_h.promise().ex)
      std::rethrow_exception(_h.promise().ex);
    return _h.promise().value;
  }

private:
  handle_type _h;
};

} // namespace fpgen

#endif
