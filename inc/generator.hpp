#ifndef _FPGEN_GENERATOR
#define _FPGEN_GENERATOR

#ifdef __clang__
#include <experimental/coroutine>
#else
#include <coroutine>
#endif

#include <exception>
#include <utility>
#include "type_traits.hpp"

/**
 *  \brief The namespace containing all of fpgen's code.
 */
namespace fpgen {
/**
 *  \brief The main generator type.
 *
 *  Represents an interruptible, resumable C++20 coroutine, with iterator. Can
 * be used as a functor type (is a callable object).
 *  To create such a coroutine, simply use the `co_yield` keyword from inside a
 * function and make the return type an `fpgen::generator<T>`. Don't forget to
 * use `co_return` to finish the function - otherwise the compiler will
 * complain.
 *
 *  \tparam T The value type for the generator. This should satisfy
 * `std::copyable`, or on (older) CLang versions, `std::is_copy_assignable<T>`.
 */
template <typename T, typename _ = type::is_generator_type<T>> class generator {
public:
  /**
   *  \brief The promise type for the generator.
   *
   *  This type is required by the C++20 spec for coroutines.
   */
  struct promise_type {
    /**
     *  \brief Type alias for the value type (`T`) for this promise.
     */
    using value_type = T;
    /**
     *  \brief Type alias for the exception type (`std::exception_ptr`).
     */
    using except_type = std::exception_ptr;
    /**
     *  \brief Type alias for the generator type (`fpgen::generator<T>`).
     */
    using gen_type = generator<T>;
    /**
     *  \brief Type alias for the suspend type (`std::suspend_always`).
     */
#ifdef __clang__
    using suspend_type = std::experimental::suspend_always;
#else
    using suspend_type = std::suspend_always;
#endif

    /**
     *  \brief The last value returned from the coroutine itself.
     */
    value_type value;
    /**
     *  \brief The last exception thrown from the coroutine, or none.
     */
    except_type ex;

    /**
     *  \brief Gets the return object.
     *
     *  This method is required by the C++20 spec for coroutines.
     *  \returns A new generator for this promise.
     */
    gen_type get_return_object() { return gen_type(*this); }
    /**
     *  \brief Gets the initial suspend object.
     *
     *  This method is required by the C++20 spec for coroutines.
     *  \returns The initial suspend object.
     */
    suspend_type initial_suspend() { return {}; }
    /**
     *  \brief Gets the final suspend object.
     *
     *  This method is required by the C++20 spec for coroutines.
     *  \returns The final suspend object.
     */
    suspend_type final_suspend() noexcept { return {}; }
    /**
     *  \brief Sets the final value from the coroutine.
     *
     *  This method is required by the C++20 spec for coroutines. When the
     * coroutine calls `co_return`, the environment will call this function.
     *  \param[in] v The value to set.
     */
    // void return_value(value_type v) { value = v; }

    void return_void() {}

    /**
     *  \brief Sets an intermediate value from the coroutine.
     *
     *  This method is required by the C++20 spec for coroutines. When the
     * coroutine calls `co_yield`, the environment will call this function.
     *  \param[in] v The value to set.
     *  \returns An intermediate suspend object.
     */
    suspend_type yield_value(value_type v) {
      value = v;
      return {};
    }

    /**
     *  \brief Sets the exception from the coroutine.
     *
     *  This method is required by the C++20 spec for coroutines. When an error
     * is thrown from the coroutine, this will set the `ex` field to the correct
     * value.
     */
    void unhandled_exception() { ex = std::current_exception(); }
  };

  /**
   *  \brief The iterator type for the generator.
   *
   *  This type makes it possible to use for-each (like `for(auto v :
   * generator)`) over generators.
   */
  struct iterator_type {
    /**
     *  \brief Type alias for the generator type (`fpgen::generator<T>`).
     */
    using gen_t = generator<T>;
    /**
     *  \brief Type alias for the iterator type
     * (`fpgen::generator<T>::iterator_type`).
     */
    using iter_t = iterator_type;
    /**
     *  \brief Type alias for the value type (`T`) for this iterator.
     */
    using value_t = T;

    /**
     *  \brief The generator this iterator is associated with.
     */
    gen_t &source;
    /**
     *  \brief Whether or not this iterator has finished iterating.
     *
     *  This value should only be true when the underlying generator is
     * finished. Iterators are compared based on this value.
     */
    // bool is_finished;
    /**
     *  \brief The last value from the coroutine (and thus, the value currently
     * held by the generator's promise).
     */
    // value_t value;

    /**
     *  \brief Constructs a new iterator from a source generator.
     *
     *  The parameter `is_finished` should only be true if we wish to represent
     * a past-the-end iterator (fpgen::generator<T>::end()). Otherwise,
     * `is_finised` should be false. This constructor immediately fetches the
     * next value in the generator.
     *  \param[in] source The underlying generator. Will be called (and thus
     * modified).
     *  \param[in] is_finised Is this an `end` iterator?
     */
    iterator_type(gen_t &source, bool is_finised) : source{source} {
      if (!is_finised)
        ++(*this);
    }
    /**
     *  \brief Constructs a new iterator from a source generator and a value.
     *
     *  Calling this constructor from outside this struct results in undefined
     * behaviour. The parameter `is_finished` should only be true if we wish to
     * represent a past-the-end iterator (fpgen::generator<T>::end()).
     * Otherwise, `is_finised` should be false.
     *  \param[in] source The underlying generator. Will be called (and thus
     * modified).
     *  \param[in] is_finised Is this iterator finished?
     *  \param[in] value The value this iterator should hold.
     */
    explicit iterator_type(gen_t &source) : source{source} {}

    /**
     *  \brief Steps the generator to the next value.
     *  \returns A new iterator for this generator, with the same state.
     */
    iter_t operator++() {
      source.next();
      return iter_t(source);
    }

    /**
     *  \brief Converts this iterator to a bool.
     *  \returns True if more values remain, otherwise false.
     */
    operator bool() { return static_cast<bool>(source); }
    /**
     *  \brief Checks two iterators for inequality.
     *
     *  This method should only be used to compare against past-the-end
     * iterators for the same generator - other uses are undefined behaviour.
     *  \param[in] other The iterator to compare against.
     *  \returns False if both are equal, otherwise true.
     */
    bool operator!=(const iter_t &) { return static_cast<bool>(source); }
    /**
     *  \brief Gets the current value from the iterator.
     *  \returns The current value.
     */
    value_t operator*() {
      source.contains = false;
      return source._h.promise().value;
    }
    /**
     *  \brief Converts this iterator to the current value.
     *  \returns The current value.
     */
    operator value_t() { return source._h.promise().value; }
  };

  /**
   *  \brief Type alias for the coroutine handle type
   * (`std::coroutine_handle<promise_type>`).
   */
#ifdef __clang__
  using handle_type = std::experimental::coroutine_handle<promise_type>;
#else
  using handle_type = std::coroutine_handle<promise_type>;
#endif
  /**
   *  \brief Type alias for the coroutine's return type (`T`).
   */
  using value_type = T;

  /**
   *  \brief Constructs a new generator from a promise.
   *
   *  This method should only be called from the environment.
   *  \param[in] p The promise to use.
   */
  generator(promise_type &p)
      : _h{handle_type::from_promise(p)}, contains{false} {}

  /**
   *  \brief Copy-constructing generators results in undefined behaviour.
   */
  generator(const generator &other) = default;
  /**
   *  \brief Moves the data from the other generator into this one.
   *  \param[in,out] other The other generator.
   */
  generator(generator &&other) = default;

  /**
   *  \brief Copy-assigning generators results in undefined behaviour.
   */
  generator &operator=(const generator &other) = default;
  /**
   *  \brief Moves the data from the other generator into this one.
   *  \param[in,out] other The other generator.
   *  \returns A reference to this generator.
   */
  generator &operator=(generator &&other) = default;

  /**
   *  \brief Cleans up this generator's resources.
   */
  ~generator() = default;

  /**
   *  \brief Converts this generator its handle.
   *  \returns The handle for this generator.
   */
  operator handle_type() const { return _h; }
  /**
   *  \brief Converts this generator to a bool.
   *  \returns True if more values remain, otherwise false.
   */
  operator bool() {
    if (_h.done())
      return false;
    next();
    return !_h.done();
  }

  /**
   *  \brief Gets an iterator to the current coroutine state.
   *  \returns An iterator for the current state.
   */
  iterator_type begin() { return {*this, false}; }
  /**
   *  \brief Gets a past-the-end iterator for the current coroutine state.
   *  \returns A past-the-end iterator for this generator.
   */
  iterator_type end() { return {*this, true}; }

  /**
   *  \brief Steps the generator forward once.
   *
   *  This effectively will resume the coroutine. Upon excptions, the exception
   * from the coroutine will be rethrown. Otherwise, the last value will be
   * returned. Calling this method while the generator is finished (`operator
   * bool` returns false) is undefined behaviour.
   *  \returns The next value in the coroutine.
   *  \throws `std::exception` Any exception can be thrown from this function
   * that can be thrown from the coroutine.
   */
  value_type operator()() {
    next();
    contains = false;
    return std::move(_h.promise().value);
  }

private:
  handle_type _h;
  bool contains;

  void next() {
    if (!contains) {
      _h();
      if (_h.promise().ex)
        std::rethrow_exception(_h.promise().ex);
      contains = true;
    }
  }
};

} // namespace fpgen

#endif
