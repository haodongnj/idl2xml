#include <fstream>
#include <iostream>
#include <sstream>

#include "CLI/CLI11.hpp"
#include "dynamic_type_xml.hpp"

int main(int argc, char** argv) {
  CLI::App app("idl2xml");
  std::vector<std::string> idl_files;
  std::string xml_file;

  std::string type_name;
  auto convert = app.add_subcommand("convert", "convert idl file to xml representation");
  convert->add_option("-i,--input", idl_files, "idl file")->required(true)->check(CLI::ExistingFile);
  convert->add_option("-o, --output", xml_file, "xml file");
  convert->add_option("-t, --typename", type_name, "type name to convert to xml representation")->required(true);
  convert->callback([&idl_files, &xml_file, &type_name]() {
    std::stringstream sstm;
    {
      for (const auto& idl_file : idl_files) {
        std::ifstream f(idl_file);
        sstm << f.rdbuf();
      }
    }
    {
      auto type_names = idl2xml::get_typenames_from_idl(sstm.str());
      if (type_names.count(type_name) == 0) {
        std::cout << "No type named [" << type_name << "] found in parsed IDL file " << std::endl;
      }
    }
    auto xml_content = idl2xml::get_xml_from_idl(sstm.str(), type_name);
    if (xml_content.has_value()) {
      if (xml_file.empty()) {
        std::cout << xml_content->c_str();
      } else {
        std::ofstream f(xml_file);
        f << xml_content->c_str();
      }
    } else {
      std::cout << "No content converted from IDL file." << std::endl;
    }
  });

  auto show = app.add_subcommand("show", "show types defined in idl file");
  show->add_option("idl_file", idl_files, "idl file")->check(CLI::ExistingFile);
  show->callback([&idl_files]() {
    std::stringstream sstm;
    for (const auto& idl_file : idl_files) {
      std::cout << "idl file: " << idl_file << std::endl;
      std::ifstream f(idl_file);
      sstm << f.rdbuf();
    }
    auto type_names = idl2xml::get_typenames_from_idl(sstm.str());
    std::cout << "types resolved:" << std::endl;
    for (const auto& type_name : type_names) {
      std::cout << "\t" << type_name << std::endl;
    }
  });

  const std::string help_msg = "Usage: idl2xml <command> [options]";
  app.usage(help_msg);
  app.set_help_flag();
  app.set_help_all_flag("-h, --help");

  CLI11_PARSE(app, argc, argv);
  return 0;
}