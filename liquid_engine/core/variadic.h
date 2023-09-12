#if !defined(CORE_VARIADIC_HPP)
#define CORE_VARIADIC_HPP
// * Description:  Variadic macros
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 29, 2023

// TODO(alicia): perhaps it may be better to not implement these by hand

typedef __builtin_va_list va_list;
#define va_arg __builtin_va_arg
#define va_start __builtin_va_start
#define va_end __builtin_va_end

#endif // header guard
