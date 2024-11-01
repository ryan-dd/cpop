#pragma once

#include "cpop/detail/convert.h"
#include "cpop/detail/concepts.h"
#include "cpop/error.h"
#include "cpop/tree.h"
#include "cpop/detail/logger.h"

#include <exception>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
#include <algorithm>
#include <ranges>
#include <format>
#include <cassert>

namespace cpop::detail { 
  class Populator {
  private:
      const Tree& tree_;
      mutable std::vector<std::string> current_path_;

      void pushPath(std::string_view key) const {
          current_path_.emplace_back(key);
      }

      void popPath() const {
          if (!current_path_.empty()) {
              current_path_.pop_back();
          }
      }

      static auto findInTree(const Tree& tree, std::string_view key) {
          return std::ranges::find_if(tree, [key](const auto& elem) { 
              return elem.key == key; 
          });
      }

      template<typename ValueType>
      auto populateValue(const Element& element) const {
          if (!std::holds_alternative<Node>(element.content)) {
              throw PopulateError("Expected Node type", current_path_);
          }

          const auto& node_value = std::get<Node>(element.content).value;
          return TypeConverter::convert<ValueType>(node_value, current_path_);
      }

      template<typename ValueType>
      void populateNested(ValueType& value, const Element& element) const {
          if (!std::holds_alternative<std::vector<Element>>(element.content)) {
              throw PopulateError("Expected nested structure", current_path_);
          }
          populateFromTree(value, std::get<std::vector<Element>>(element.content));
      }

  public:
      explicit Populator(const Tree& tree) : tree_(tree) {}

      template<RequiredParamType Field>
      void populateRequired(Field& field) const {
          using ValueType = typename std::remove_cvref_t<decltype(field.value)>;

          pushPath(field.key);
          try {
              auto iter = findInTree(tree_, field.key);
              if (iter == tree_.end()) {
                  throw PopulateError("Required key not found", current_path_);
              }

              if constexpr (StructType<ValueType>) {
                  populateNested(field.value, *iter);
              } else {
                  field.value = populateValue<ValueType>(*iter);
              }
          }
          catch (const PopulateError&) {
              throw;
          }
          catch (const std::exception& e) {
              throw PopulateError(e.what(), current_path_);
          }
          popPath();
      }

      template<OptionalParamType Field>
      void populateOptional(Field& field) const {
          using OptionalType = typename std::remove_cvref_t<decltype(field.value)>::value_type;

          pushPath(field.key);
          try {
              auto iter = findInTree(tree_, field.key);
              if (iter == tree_.end()) {
                  popPath();
                  return;
              }

              if constexpr (StructType<OptionalType>) {
                  if (std::holds_alternative<std::vector<Element>>(iter->content)) {
                      OptionalType nestedObj;
                      populateNested(nestedObj, *iter);
                      field.value = std::move(nestedObj);
                  } else {
                      Logger::warn("Optional nested structure found but has wrong type", current_path_);
                  }
              } else {
                  if (std::holds_alternative<Node>(iter->content)) {
                      const auto& node = std::get<Node>(iter->content);
                      auto converted = TypeConverter::tryConvert<OptionalType>(node.value);
                      if (converted) {
                          field.value = std::move(*converted);
                      } else {
                          Logger::warn(std::format(
                              "Failed to convert optional parameter with value '{}'", 
                              node.value), current_path_);
                      }
                  } else {
                      Logger::warn("Optional parameter found but has wrong type", current_path_);
                  }
              }
          }
          catch (const std::exception& e) {
              Logger::warn(std::format("Failed to parse optional field: {}", e.what()), 
                  current_path_);
          }
          popPath();
      }

      template<MultipleType Field>
      void populateMultiple(Field& field) const {
          pushPath(field.list_key);
          try {
              auto iter = findInTree(tree_, field.list_key);
              if (iter == tree_.end()) {
                  popPath();
                  return;
              }

              if (!std::holds_alternative<std::vector<Element>>(iter->content)) {
                  Logger::warn("Multiple field found but has wrong type", current_path_);
                  popPath();
                  return;
              }

              const auto& list = std::get<std::vector<Element>>(iter->content);
              auto matching_elements = list | std::views::filter(
                  [&](const auto& elem) { return elem.key == field.element_key; });

              for (const auto& item : matching_elements) {
                  pushPath(field.element_key);
                  try {
                      typename Field::value_type nestedObj;
                      if (std::holds_alternative<std::vector<Element>>(item.content)) {
                          populateNested(nestedObj, item);
                          field.values.push_back(std::move(nestedObj));
                      } else {
                          Logger::warn("Invalid item structure in list", current_path_);
                      }
                  }
                  catch (const std::exception& e) {
                      Logger::warn(std::format("Failed to parse list item: {}", e.what()), 
                          current_path_);
                  }
                  popPath();
              }
          }
          catch (const std::exception& e) {
              throw PopulateError(e.what(), current_path_);
          }
          popPath();
      }
  };
}
