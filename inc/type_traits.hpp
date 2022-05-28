/////////////////////////////////////////////////////////////////////////////
// Name:        type_traits.hpp
// Purpose:     some helper type traits for fpgen.
// Author:      jay-tux
// Copyright:   (c) 2022 jay-tux
// Licence:     MPL
/////////////////////////////////////////////////////////////////////////////
#ifndef _FPGEN_TYPE_TRAITS
#define _FPGEN_TYPE_TRAITS

#include <functional>
#include <type_traits>

/**
 *  \brief The namespace containing some type helpers for fpgen.
 */
namespace fpgen::type {

/**
 *  \brief Type trait deducing whether a type is a function from the input types
 * to the output type.
 *
 *  In short, the function type `TFun` should have the signature `(TIns...) ->
 * TOut`. This can include any "free" function (plain C function pointer), any
 * lambda function, or any struct/class type supporting `TOut
 * operator()(TIns...)`. This is implemented using the type traits
 * `std::enable_if` and `std::is_convertible`. Usage: use as an extra template
 * type, like so:
 *        `typename _ = fpgen::type::is_function_to<TFun, TOut, TIns...>`.
 *
 *  \tparam TFun The function type.
 *  \tparam TOut The output type for the function.
 *  \tparam TIns The input type(s) for the function.
 */
template <typename TFun, typename TOut, typename... TIns>
using is_function_to = typename std::enable_if<
    std::is_convertible<TFun, std::function<TOut(TIns...)>>::value>::type;

/**
 *  \brief Type trait deducing whether a type is a predicate.
 *
 *  In short, the function type `TFun` should have the signature `(TIns...) ->
 * bool`. This can include any "free" function (plain C function pointer), any
 * lambda function, or any struct/class type supporting `bool
 * operator()(TIns...)`. This is implemented using fpgen::type::is_function_to.
 * Usage: use as an extra template type, like so:
 *        `typename _ = fpgen::type::is_predicate<TFun, TIns...>`;
 * this is synonymous to using
 *        `typename _ = fpgen::type::is_function_to<TFun, bool, TIns...>`.
 *
 *  \tparam TFun The predicate function type.
 *  \tparam TIns The input type(s) for the function.
 */
template <typename TFun, typename... TIns>
using is_predicate = is_function_to<TFun, bool, TIns...>;

/**
 *  \brief Type trait deducing the output type of a functional type.
 *
 *  If the type `TFun` is a function type `(TIns...) -> TOut`, this type trait
 * will return `TOut`. Thie type trait is implemented using
 * `std::invoke_result`.
 *
 *  \tparam TFun The function type.
 *  \tparam TIns The input type(s) for the function.
 */
template <typename TFun, typename... TIns>
using output_type = typename std::invoke_result<TFun, TIns...>::type;

/**
 *  \brief Type trait deducing whether a type is fit as data type for a
 * generator.
 *
 *  The type `T` will pass the test if it's copy-assignable.
 *
 *  \tparam T The type to check.
 */
template <typename T>
using is_generator_type =
    typename std::enable_if<std::is_copy_assignable<T>::value>::type;

} // namespace fpgen::type

#endif
