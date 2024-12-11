import argparse
from pyidl2xml import get_xml_from_idl, get_typenames_from_idl

def convert_idl_to_xml(idl_file: str, xml_file: str, typename: str):
    # Read the IDL file
    with open(idl_file, 'r') as f:
        idl_content = f.read()

    # Convert the IDL content to XML
    xml_content = get_xml_from_idl(idl_content, typename)

    with open(xml_file, 'w') as f:
        f.write(xml_content)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert IDL to XML')
    parser.add_argument("-i", dest="idl_file", type=str, required=True, help="Path to the IDL file")
    parser.add_argument("-o", dest="xml_file", type=str, required=True, help="Path to the XML file")
    parser.add_argument("-t", dest="typename", type=str, required=True, help="Typename to convert")
    args = parser.parse_args()

    with open(args.idl_file, 'r') as f:
        idl_content = f.read()
        typenames = get_typenames_from_idl(idl_content)
        for typename in typenames:
            print(typename)

    convert_idl_to_xml(args.idl_file, args.xml_file, args.typename)