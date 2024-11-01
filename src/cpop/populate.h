#pragma once

#include "cpop/tree.h"
#include "cpop/detail/concepts.h"
#include "cpop/detail/populator.h"

#include <boost/pfr/core.hpp>
#include <type_traits>

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
    });
}

}
