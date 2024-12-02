#include <iostream>
#include <fstream>

#include "dynamic_type_xml.hpp"
#include "xtypes/xtypes.hpp"
#include "xtypes/idl/idl.hpp"

using namespace eprosima::xtypes;

int main() {
    std::string idl_spec =
            R"(
module A{
        struct InnerType
        {
            uint32 im1;
            float im2;
        };
};
    )";

    idl::Context context = idl::parse(idl_spec);
    StructType inner = context.module().structure("A::InnerType");

    AliasType abool(primitive_type<bool>(), "bool");
    context.module().add_alias(abool);

    StructType outer("OuterType");
    outer.add_member(Member("om1", primitive_type<double>()).id(2));
    outer.add_member("om2", inner);
    outer.add_member("om3", StringType());
    outer.add_member("om4", WStringType(100));
    outer.add_member("om5", SequenceType(primitive_type<uint32_t>(), 5));
    outer.add_member("om6", SequenceType(inner));
    outer.add_member("om7", ArrayType(primitive_type<uint32_t>(), 4));
    outer.add_member("om8", ArrayType(inner, 4));
    outer.add_member("om9", SequenceType(SequenceType(primitive_type<uint32_t>(), 5), 3));
    outer.add_member("om10", ArrayType(ArrayType(primitive_type<uint32_t>(), 2), 3));
    outer.add_member("om11", abool);
    outer.add_member("om12", MapType(StringType(), inner));
    context.module().structure(outer);

    {
        std::ofstream f("outer_type.idl");
        f << idl::generate(context.module());
    }
    {
        std::stringstream sstm;
        {
            std::ifstream f("outer_type.idl");
            sstm << f.rdbuf();
        }
        auto xml_content = idl2xml::get_xml_from_idl(sstm.str(), "OuterType");
        if (xml_content.has_value()) {
            std::ofstream f("outer_type.xml");
            f << xml_content.value();
        }
    }


    return 0;
}
