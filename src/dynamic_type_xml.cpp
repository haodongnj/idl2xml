#include "dynamic_type_xml.hpp"

#include <cstdint>
#include <set>
#include <type_traits>
#include <unordered_map>

#include "pugixml.hpp"
#include "utils.hpp"
#include "xtypes/EnumerationType.hpp"
#include "xtypes/TypeKind.hpp"
#include "xtypes/idl/idl.hpp"
#include "xtypes/xtypes.hpp"

using namespace eprosima::xtypes;
namespace idl2xml {

const static std::unordered_map<std::string, std::string> primitive_typename_map{
    // octet is represented by uint8 and not handled here
    {"bool", "boolean"},    {"char", "char8"},      {"wchar_t", "char16"}, {"uint8_t", "uint8"},       {"int8_t", "int8"},
    {"int16_t", "int16"},   {"uint16_t", "uint16"}, {"int32_t", "int32"},  {"uint32_t", "uint32"},     {"int64_t", "int64"},
    {"uint64_t", "uint64"}, {"float", "float32"},   {"double", "float64"}, {"long double", "float128"}};

const static std::unordered_map<std::string, std::string> string_typename_map{
    {"std::string", "string"},
    {"std::wstring", "wstring"},
};

static bool is_string_type_kind(const DynamicType& type) {
  return type.kind() == TypeKind::STRING_TYPE || type.kind() == TypeKind::STRING16_TYPE ||
         type.kind() == TypeKind::WSTRING_TYPE;
}

static bool is_basic_type(const DynamicType& type) {
  return type.is_primitive_type() || is_string_type_kind(type);
}

static void update_member_type(pugi::xml_node& member_node, const DynamicType& type) {
  if (type.is_primitive_type()) {
    if (primitive_typename_map.count(type.name())) {
      member_node.append_attribute("type") = primitive_typename_map.at(type.name());
    } else {
      member_node.append_attribute("type") = type.name();
    }
  } else if (is_string_type_kind(type)) {
    auto type_and_length = split(type.name(), "_");
    assert(type_and_length.size() > 0);
    std::string type_name{type_and_length[0]};
    if (string_typename_map.count(type_name)) {
      member_node.append_attribute("type") = string_typename_map.at(type_name);
    } else {
      member_node.append_attribute("type") = type.name();
    }
  } else if (type.kind() == TypeKind::SEQUENCE_TYPE) {
    auto sequence_type = static_cast<const SequenceType&>(type);
    const auto& content_type = sequence_type.content_type();
    update_member_type(member_node, content_type);
  } else if (type.kind() == TypeKind::ARRAY_TYPE) {
    const DynamicType* p_content_type = &type;
    while (p_content_type->kind() == TypeKind::ARRAY_TYPE) {
      p_content_type = &(static_cast<const ArrayType*>(p_content_type)->content_type());
    }
    update_member_type(member_node, *p_content_type);
  } else {
    member_node.append_attribute("type") = "nonBasic";
    member_node.append_attribute("nonBasicTypeName") = type.name();
  }
}

static void update_primitive_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  update_member_type(member_node, member.type());
}

static void update_string_member_attributes(pugi::xml_node& member_node, const Member& member) {
  uint32_t bounds = 0;
  if (member.type().kind() == TypeKind::STRING_TYPE) {
    const auto& string_type = static_cast<const StringType&>(member.type());
    bounds = string_type.bounds();
  } else if (member.type().kind() == TypeKind::WSTRING_TYPE) {
    const auto& wstring_type = static_cast<const WStringType&>(member.type());
    bounds = wstring_type.bounds();
  } else if (member.type().kind() == TypeKind::STRING16_TYPE) {
    const auto& string16_type = static_cast<const String16Type&>(member.type());
    bounds = string16_type.bounds();
  } else {
    return;
  }
  member_node.append_attribute("name") = member.name();
  update_member_type(member_node, member.type());
  if (bounds != 0) {
    member_node.append_attribute("stringMaxLength") = bounds;
  } else {
    // If bounds is 0, it means that no bounds were specified.
  }
}

static void update_enumeration_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  update_member_type(member_node, member.type());
}

static void update_alias_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  update_member_type(member_node, member.type());
}

static void update_sequence_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  const auto& type = static_cast<const SequenceType&>(member.type());
  update_member_type(member_node, member.type());
  const auto bounds = type.bounds();
  if (bounds == 0) {
    member_node.append_attribute("sequenceMaxLength") = -1;
  } else {
    member_node.append_attribute("sequenceMaxLength") = type.bounds();
  }
}

static void update_array_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  const auto& array_type = static_cast<const ArrayType&>(member.type());
  std::vector<uint32_t> dims;
  const DynamicType* p_content_type{nullptr};

  auto get_array_type_info = [&dims, &p_content_type](const ArrayType& array_type) {
    const ArrayType* current_array_type = &array_type;
    const DynamicType* current_content_type = &array_type.content_type();

    dims.push_back(current_array_type->dimension());

    auto type_kind_value = current_content_type->kind();
    while (type_kind_value == TypeKind::ARRAY_TYPE) {
      current_array_type = &static_cast<const ArrayType&>(*current_content_type);
      current_content_type = &current_array_type->content_type();
      dims.push_back(current_array_type->dimension());
      type_kind_value = current_content_type->kind();
    }
    p_content_type = current_content_type;
  };

  get_array_type_info(array_type);

  update_member_type(member_node, member.type());
  member_node.append_attribute("arrayDimensions") = join(dims, ",");
}

static void update_map_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  const auto& map_type = static_cast<const MapType&>(member.type());
  auto bounds = map_type.bounds();
  const auto& pair_type = static_cast<const PairType&>(map_type.content_type());
  const auto& key_type = pair_type.first();
  const auto& value_type = pair_type.second();

  update_member_type(member_node, value_type);

  if (is_basic_type(key_type)) {
    if (key_type.is_primitive_type()) {
      assert(primitive_typename_map.count(key_type.name()));
      member_node.append_attribute("key_type") = primitive_typename_map.at(key_type.name());
    } else if (is_string_type_kind(key_type)) {
      assert(string_typename_map.count(key_type.name()));
      member_node.append_attribute("key_type") = string_typename_map.at(key_type.name());
    } else {
      // pass
    }
  } else {
    member_node.append_attribute("key_type") = key_type.name();
  }

  if (bounds == 0) {
    member_node.append_attribute("mapMaxLength") = -1;
  } else {
    member_node.append_attribute("mapMaxLength") = bounds;
  }
}

static void update_structure_member_attributes(pugi::xml_node& member_node, const Member& member) {
  member_node.append_attribute("name") = member.name();
  update_member_type(member_node, member.type());
}

static void save_member_to_xml_node(pugi::xml_node& member_node, const Member& member) {
  auto member_kind = member.type().kind();
  if (static_cast<std::underlying_type<TypeKind>::type>(member_kind & TypeKind::PRIMITIVE_TYPE)) {
    update_primitive_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::STRING_TYPE || member_kind == TypeKind::WSTRING_TYPE ||
             member_kind == TypeKind::STRING16_TYPE) {
    update_string_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::ENUMERATION_TYPE) {
    update_enumeration_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::ALIAS_TYPE) {
    update_alias_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::SEQUENCE_TYPE) {
    update_sequence_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::ARRAY_TYPE) {
    update_array_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::MAP_TYPE) {
    update_map_member_attributes(member_node, member);
  } else if (member_kind == TypeKind::STRUCTURE_TYPE) {
    update_structure_member_attributes(member_node, member);
  } else {
    // nothing to do for other types
    // UnionType, BitSetType,BitMaskType is not supported yet.
  }
}

static void save_enum_to_xml_node(pugi::xml_node& node, const EnumerationType<int32_t>& enum_type) {
  auto type_definition_node = node.append_child("enum");
  type_definition_node.append_attribute("name") = enum_type.name();
  for (const auto& enumerator : enum_type.enumerators()) {
    auto enumerator_node = type_definition_node.append_child("enumerator");
    enumerator_node.append_attribute("name") = enumerator.first.c_str();
    enumerator_node.append_attribute("value") = enumerator.second;
  }
}

static void save_struct_to_xml_node(pugi::xml_node& node, const StructType& struct_type) {
  auto type_definition_node = node.append_child("struct");
  type_definition_node.append_attribute("name") = struct_type.name();
  for (const auto member : struct_type.members()) {
    auto member_node = type_definition_node.append_child("member");
    save_member_to_xml_node(member_node, member);
  }
}

static void save_alias_to_xml_node(pugi::xml_node& node, const AliasType& alias_type) {
  auto type_definition_node = node.append_child("typedef");
  type_definition_node.append_attribute("name") = alias_type.name();
  const auto& aliased_type = alias_type.get();
  if (is_basic_type(aliased_type)) {
    const auto& type_name = aliased_type.name();
    if (aliased_type.is_primitive_type()) {
      type_definition_node.append_attribute("type") = primitive_typename_map.at(type_name);
    } else if (is_string_type_kind(aliased_type)) {
      auto type_and_length = split(type_name, "_");
      assert(type_and_length.size() >= 1);
      std::string actual_typename{type_and_length[0]};
      assert(string_typename_map.count(actual_typename));
      type_definition_node.append_attribute("type") = string_typename_map.at(actual_typename);
    } else {
      // pass, should not come here
    }
  } else {
    type_definition_node.append_attribute("type") = "nonBasic";
    type_definition_node.append_attribute("nonBasicTypeName") = aliased_type.name();
  }
}

static void save_type_to_xml_node(pugi::xml_node& node, std::set<std::string>& saved_types,
                                  const DynamicType::TypeNode& type_node) {
  const auto& type = type_node.type();

  if (saved_types.count(type.name())) {
    return;
  } else {
    saved_types.insert(type.name());
  }

  switch (type.kind()) {
    case TypeKind::STRUCTURE_TYPE: {
      const auto& struct_type = static_cast<const StructType&>(type);
      save_struct_to_xml_node(node, struct_type);
      break;
    }
    case TypeKind::ENUMERATION_TYPE: {
      const auto& enum_type = static_cast<const EnumerationType<int32_t>&>(type);
      save_enum_to_xml_node(node, enum_type);
      break;
    }
    case TypeKind::ALIAS_TYPE: {
      const auto& alias_type = static_cast<const AliasType&>(type);
      save_alias_to_xml_node(node, alias_type);
      break;
    }
    default:
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
  auto type = context.module().type(type_name);
  if (type.get() == nullptr) {
    return std::nullopt;
  } else {
    auto doc = get_xml_from_dynamic_type(*type);
    std::stringstream sstm;
    doc.save(sstm);
    return sstm.str();
  }
}

}  // namespace idl2xml
