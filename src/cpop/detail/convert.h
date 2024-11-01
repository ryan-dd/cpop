#pragma once

#include "cpop/error.h"
#include "cpop/detail/logger.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <exception>
#include <format>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cpop::detail {
  class TypeConverter {
    private:
      // Helper for string preprocessing
      static std::string preprocessString(std::string_view value) {
        return std::string(value);
      }

      // Helper to check if string is completely consumed after conversion
      static bool isFullyConsumed(std::string_view value, size_t pos) {
        while (pos < value.length() && std::isspace(value[pos])) {
          pos++;
        }
        return pos == value.length();
      }

      // Boolean conversion implementation
      static std::optional<bool> convertToBool(std::string_view value) {
        std::string lower{value};
        std::ranges::transform(lower, lower.begin(), ::tolower);
        if (lower == "true") return true;
        if (lower == "false") return false;
        return std::nullopt;
      }

      // Floating point conversion implementation
      static std::optional<double> convertToDouble(std::string_view value) {
        std::size_t pos{};
        try {
          const double result = std::stod(preprocessString(value), &pos);
          return isFullyConsumed(value, pos) ? std::optional<double>{result} : std::nullopt;
        }
        catch (const std::exception&) {
          return std::nullopt;
        }
      }

      // String conversion implementation
      static std::optional<std::string> convertToString(std::string_view value) {
        return std::string(value);
      }

      // Unsigned integral conversion implementation
      template<typename T>
        static std::optional<T> convertToUnsigned(std::string_view value, const std::vector<std::string>& path) {
          static_assert(std::is_unsigned_v<T>, "T must be unsigned");

          try {
            size_t pos{};
            auto result = std::stoull(preprocessString(value), &pos);

            if (!isFullyConsumed(value, pos)) {
              return std::nullopt;
            }

            if (result > std::numeric_limits<T>::max()) {
              Logger::warn(std::format("Value '{}' exceeds maximum limit of type ({})", 
                    result, std::to_string(std::numeric_limits<T>::max())));
              return std::nullopt;
            }

            return static_cast<T>(result);
          }
          catch (const std::exception&) {
            return std::nullopt;
          }
        }

      // Signed integral conversion implementation
      template<typename T>
        static std::optional<T> convertToSigned(std::string_view value, const std::vector<std::string>& path) {
          static_assert(std::is_signed_v<T>, "T must be signed");

          try {
            size_t pos{};
            auto result = std::stoll(preprocessString(value), &pos);

            if (!isFullyConsumed(value, pos)) {
              return std::nullopt;
            }

            if (result < std::numeric_limits<T>::min()) {
              Logger::warn(std::format("Value '{}' exceeds minimum limit of type ({})", 
                    result, std::to_string(std::numeric_limits<T>::min())));
              return std::nullopt;
            }

            if (result > std::numeric_limits<T>::max()) {
              Logger::warn(std::format("Value '{}' exceeds maximum limit of type ({})", 
                    result, std::to_string(std::numeric_limits<T>::max())));
              return std::nullopt;
            }

            return static_cast<T>(result);
          }
          catch (const std::exception&) {
            return std::nullopt;
          }
        }

    public:
      template<typename T>
        static std::optional<T> tryConvert(std::string_view value, const std::vector<std::string>& path = {}) {
          if (value.empty()) {
            return std::nullopt;
          }

          if constexpr (std::is_same_v<T, bool>) {
            return convertToBool(value);
          }
          else if constexpr (std::is_same_v<T, double>) {
            return convertToDouble(value);
          }
          else if constexpr (std::is_same_v<T, std::string>) {
            return convertToString(value);
          }
          else if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_unsigned_v<T>) {
              return convertToUnsigned<T>(value, path);
            }
            else {
              return convertToSigned<T>(value, path);
            }
          }
          else {
            static_assert(!sizeof(T), "Unsupported type for conversion");
          }
        }

      template<typename T>
        static T convert(std::string_view value, const std::vector<std::string>& path = {}) {
          auto result = tryConvert<T>(value, path);
          if (!result) {
            throw PopulateError(
                std::format("Failed to convert value: '{}' to required type", value), 
                path
                );
          }
          return *result;
        }
  };
}
