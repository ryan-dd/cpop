#pragma once

#include <iterator>
#include <ranges>
#include <sstream>
#include <string_view>

namespace cpop::detail {
  // The pure ranges solutions were all awful to look at even though they worked. 
  // So I made a utility function for this
  template<std::ranges::range Range>
  std::string toStringWithDelims(const Range& range, std::string_view delimiter) {
      std::ostringstream result;
      auto iter = std::begin(range);
      auto end = std::end(range);

      if (iter != end) {
          result << *iter;
          ++iter;
      }

      while (iter != end) {
          result << delimiter << *iter;
          ++iter;
      }

      return result.str();
  }
}
