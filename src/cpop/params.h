#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cpop
{
  template<typename T>
  struct Param {
      std::string key;
      T value;
      using value_type = T;

      explicit Param(std::string_view key) : key(key) {}
  };

  template<typename T>
  struct OptParam {
      std::string key;
      std::optional<T> value;
      using value_type = T;

      explicit OptParam(std::string_view key) : key(key) {}
  };

  template<typename T>
  struct Multiple {
      std::string list_key;    // Key for the list container
      std::string element_key; // Key for each element
      std::vector<T> values;
      using value_type = T;

      Multiple(std::string_view list_key, std::string_view element_key) 
          : list_key(list_key), element_key(element_key) {}
  };
}
