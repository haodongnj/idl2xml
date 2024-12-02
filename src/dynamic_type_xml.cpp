#include "dynamic_type_xml.hpp"

#include <set>

#include "pugixml.hpp"
#include "xtypes/idl/idl.hpp"
#include "xtypes/xtypes.hpp"

using namespace eprosima::xtypes;
namespace idl2xml {

static void save_type_to_xml_node(pugi::xml_node& node, std::set<std::string>& saved_types,
                                  const DynamicType::TypeNode& type_node) {
  const auto& type = type_node.type();

  if (saved_types.count(type.name())) {
    return;
  } else {
    saved_types.insert(type.name());
  }

  switch (type.kind()) {
    case TypeKind::STRUCTURE_TYPE:
      const auto& struct_type = static_cast<const StructType&>(type);
      auto type_node = node.append_child("type");
      auto type_definition_node = type_node.append_child("struct");
      type_definition_node.append_attribute("name") = type.name();
      for (const auto member : struct_type.members()) {
        auto member_node = type_definition_node.append_child("member");
        member_node.append_attribute("name") = member.name();
        member_node.append_attribute("type") = member.type().name();
      }
      break;
  }
}

static pugi::xml_document get_xml_from_dynamic_type(const DynamicType& type,
                                                    const std::string xmlns = "http://www.eprosima.com") {
  std::set<std::string> saved_types;
  pugi::xml_document doc;
  auto node = doc.append_child("types");
  node.append_attribute("xmlns") = xmlns;

  type.for_each(
      [&node, &saved_types](const DynamicType::TypeNode& type_node) { save_type_to_xml_node(node, saved_types, type_node); },
      false);

  return doc;
}

std::set<std::string> get_typenames_from_idl(const std::string& idl_spec) {
  idl::Context context;
  context.ignore_redefinition = true;
  context.allow_keyword_identifiers = true;
  context = idl::parse(idl_spec, context);

  std::set<std::string> type_names;
  for (const auto& [type_name, _] : context.module().get_all_types(true)) {
    type_names.insert(type_name);
  }
  return type_names;
}

std::optional<std::string> get_xml_from_idl(const std::string& idl_content, const std::string& type_name) {
  auto type_names = get_typenames_from_idl(idl_content);

  if (not type_names.count(type_name)) {
    return std::nullopt;
  }

  idl::Context context;
  context.ignore_redefinition = true;
  context.allow_keyword_identifiers = true;
  context = idl::parse(idl_content, context);
  auto struct_type = context.module().structure(type_name);
  auto doc = get_xml_from_dynamic_type(struct_type);

  std::stringstream sstm;
  doc.save(sstm);
  return sstm.str();
}

}  // namespace idl2xml
