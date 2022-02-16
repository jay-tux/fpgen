#ifndef _FPGEN___HELPERS
#define _FPGEN___HELPERS

#ifdef __clang__
#include <type_traits>
#define _FPGEN_USE_CONCEPTS 0
#else
#include <concepts>
#define _FPGEN_USE_CONCEPTS 1
#endif

#endif
