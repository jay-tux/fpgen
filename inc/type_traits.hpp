#ifndef _FPGEN_TYPE_TRAITS
#define _FPGEN_TYPE_TRAITS

#include <functional>
#include <type_traits>

namespace fpgen::type {

template <typename TFun, typename TOut, typename... TIns>
using is_function_to = typename std::enable_if<
    std::is_convertible<TFun, std::function<TOut(TIns...)>>::value>::type;

template <typename TFun, typename... TIns>
using is_predicate = is_function_to<TFun, bool, TIns...>;

template <typename TFun, typename... TIns>
using output_type = typename std::invoke_result<TFun, TIns...>::type;

template <typename T>
using is_generator_type =
    typename std::enable_if<std::is_copy_assignable<T>::value>::type;

} // namespace fpgen::type

#endif
