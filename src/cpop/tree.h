#pragma once

#include <string>
#include <variant>
#include <vector>

namespace cpop
{

struct Node { 
    std::string value; 
};

struct Element;
using Content = std::variant<std::vector<Element>, Node>;

struct Element {
    std::string key;
    Content content;
};

using Tree = std::vector<Element>;

}
