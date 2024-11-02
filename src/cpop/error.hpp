#pragma once

#include "cpop/detail/to_string_with_delims.hpp"

#include <format>
#include <stdexcept>
#include <string>
#include <vector>
#include <string_view>

namespace cpop {

class PopulateError : public std::runtime_error {
public:
    PopulateError(std::string_view message, std::vector<std::string> path)
        : std::runtime_error(buildMessage(message, path)), path_(std::move(path)) {}

    [[nodiscard]] const std::vector<std::string>& path() const & noexcept { 
      return path_; 
    }

    std::vector<std::string> path() && noexcept { 
      return std::move(path_); 
    }

private:
    std::vector<std::string> path_;

    static std::string buildMessage(std::string_view message, const std::vector<std::string>& path) {
        return std::format("Error at path: {}\nDetails: {}", detail::toStringWithDelims(path, " -> "), message);
    }
};

}
