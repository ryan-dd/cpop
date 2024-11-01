#pragma once

#include "cpop/params.h"

#include <concepts>
#include <string>
#include <type_traits>

namespace cpop::detail {
  template<typename T>
  concept Numeric = std::integral<T> || std::floating_point<T>;

  template<typename T>
    concept MultipleType = requires { typename T::value_type; } && 
    std::same_as<T, Multiple<typename T::value_type>>;

  template<typename T>
    concept OptionalParamType = requires { typename T::value_type; } && 
    std::same_as<T, OptParam<typename T::value_type>>;

  template<typename T>
    concept RequiredParamType = requires { typename T::value_type; } && 
    std::same_as<T, Param<typename T::value_type>>;

  template<typename T>
    concept StructType = !std::is_fundamental_v<T> && 
    !std::same_as<T, std::string> &&
    !OptionalParamType<T> &&
    !RequiredParamType<T>;
}
