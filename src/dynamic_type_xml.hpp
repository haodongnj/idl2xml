#pragma once

#include <optional>
#include <set>
#include <string>

namespace idl2xml {

std::set<std::string> get_typenames_from_idl(const std::string& idl_spec);

std::optional<std::string> get_xml_from_idl(const std::string& idl_spec, const std::string& type_name);

}  // namespace idl2xml
