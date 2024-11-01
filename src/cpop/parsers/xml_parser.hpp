#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>

namespace cpop {
  class XMLParser {
  public:
      static cpop::Tree parse(const std::string& xml_string) {
          boost::property_tree::ptree pt;
          std::stringstream ss(xml_string);
          boost::property_tree::read_xml(ss, pt);
          
          return parseTree(pt);
      }

      static cpop::Tree parseFromFile(const std::string& filename) {
          boost::property_tree::ptree pt;
          boost::property_tree::read_xml(filename, pt);
          
          return parseTree(pt);
      }

  private:
      static cpop::Tree parseTree(const boost::property_tree::ptree& pt) {
          Tree result;
          
          for (const auto& child : pt) {
              result.push_back(parseElement(child.first, child.second));
          }
          
          return result;
      }
      
      static cpop::Element parseElement(const std::string& key, const boost::property_tree::ptree& pt) {
          cpop::Element element;
          element.key = key;
          
          // Check if this node has children
          if (pt.empty()) {
              // This is a leaf node
              cpop::Node node;
              node.value = pt.data();
              element.content = node;
          } else {
              // This is a parent node with children
              std::vector<cpop::Element> children;
              for (const auto& child : pt) {
                  children.push_back(parseElement(child.first, child.second));
              }
              element.content = children;
          }
          
          return element;
      }
  };
}
