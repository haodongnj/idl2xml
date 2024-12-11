#include "dynamic_type_xml.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace py = pybind11;

PYBIND11_MODULE(pyidl2xml, m) {
  m.doc() = "Python binding for idl2xml.";
  m.def(
      "get_typenames_from_idl",
      [](const std::string& idl) -> std::vector<std::string> {
        auto types = idl2xml::get_typenames_from_idl(idl);
        return {types.cbegin(), types.cend()};
      },
      "Get type names from IDL string.");
  m.def(
      "get_xml_from_idl",
      [](const std::string& idl, const std::string& type_name) -> std::string {
        auto xml = idl2xml::get_xml_from_idl(idl, type_name);
        if (xml.has_value()) {
          return xml.value();
        } else {
          return {};
        }
      },
      "Parse IDL string and convert to xml representation.");
}