# idl2xml

Convert IDL to XML representation according to [Dynamic Types profiles](https://fast-dds.docs.eprosima.com/en/3.1.x/fastdds/xml_configuration/dynamic_types.html).

## Build

How to build:
```bash
cd idl2xml
# clone submodules recursively
git submodule update --init --recursive
mkdir build && cd build
cmake .. && make
```

## Usage

Show usage message:
```bash
cd build
# show help message
./idl2xml -h
```

Help message:
```bash
idl2xml
Usage: idl2xml <command> [options]

Options:
  -h,--help                   

Subcommands:
convert
  convert idl file to xml representation
  Options:
    -i,--input TEXT:FILE REQUIRED
                                idl file
    -o,--output TEXT            xml file
    -t,--typename TEXT REQUIRED type name to convert to xml representation

show
  show types defined in idl file
  Positionals:
    idl_file TEXT:FILE          idl file
```

## Python bindings

### Build

```bash
cd idl2xml
# clone submodules recursively
git submodule update --init --recursive
pip3 install -r requirements.txt
pip3 install . 
```
