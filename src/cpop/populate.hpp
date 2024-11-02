#pragma once

#include "cpop/params.hpp"
#include "cpop/tree.hpp"
#include "cpop/detail/concepts.hpp"
#include "cpop/detail/populator.hpp"

#include <boost/pfr/core.hpp>

#include <string>
#include <type_traits>
#include <utility>

namespace cpop
{

template<typename T>
void populateFromTree(T& obj, const Tree& tree) {
    detail::Populator populator(tree);
    boost::pfr::for_each_field(obj, [&populator](auto& field) {
        using FieldType = std::remove_cvref_t<decltype(field)>;

        if constexpr (detail::RequiredParamType<FieldType>) {
            populator.populateRequired(field);
        }
        else if constexpr (detail::OptionalParamType<FieldType>) {
            populator.populateOptional(field);
        }
        else if constexpr (detail::MultipleType<FieldType>) {
            populator.populateMultiple(field);
        }

        // skip fields that are not params
    });
}

// Most xml docs have an overall element at the top level.
// This is a convenience function so that you don't have to manually create a struct for the element
template<typename T>
void populateFromTree(T& obj, const Tree& tree, std::string topLevelTag) {
    struct Wrapper {
      Param<T> config;
    };

    Wrapper wrapper{.config = Param<T>{std::move(topLevelTag)}};

    populateFromTree(wrapper, tree);

    obj = wrapper.config.value;
}

}
