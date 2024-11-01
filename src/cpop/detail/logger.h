#pragma once

#include "cpop/detail/to_string_with_delims.h"

#include <format>
#include <print>
#include <string>
#include <string_view>
#include <vector>

namespace cpop::detail {
  class Logger {
  public:
      static void warn(std::string_view message, const std::vector<std::string>& path = {}) {
          std::string pathStr;
          if (!path.empty()) {
              pathStr = std::format(" (at path: {})", toStringWithDelims(path, " -> "));
          }
          std::println("Warning: {}{}", message, pathStr);
      }
  };
}
