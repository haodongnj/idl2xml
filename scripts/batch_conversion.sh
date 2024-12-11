#!/bin/bash

export EXECUTABLE=./cmake-build-debug/idl2xml
if [ ! -f "$EXECUTABLE" ]; then
    EXECUTABLE=./build/idl2xml
    if [ ! -f "$EXECUTABLE" ]; then 
        echo "Error: idl2xml executable not found"
        exit 1
    fi
fi


mkdir -p output
$EXECUTABLE convert -i  resource/test.idl -o output/PrimitivesStruct.xml -t test::PrimitivesStruct
$EXECUTABLE convert -i  resource/test.idl -o output/StringsStruct.xml -t test::StringsStruct
$EXECUTABLE convert -i  resource/test.idl -o output/MyEnum.xml -t test::MyEnum
$EXECUTABLE convert -i  resource/test.idl -o output/EnumStruct.xml -t test::EnumStruct
$EXECUTABLE convert -i  resource/test.idl -o output/AliasStruct.xml -t test::AliasStruct
$EXECUTABLE convert -i  resource/test.idl -o output/SequenceStruct.xml -t test::SequenceStruct
$EXECUTABLE convert -i  resource/test.idl -o output/ArrayStruct.xml -t test::ArrayStruct
$EXECUTABLE convert -i  resource/test.idl -o output/MapStruct.xml -t test::MapStruct
$EXECUTABLE convert -i  resource/test.idl -o output/InnerStruct.xml -t test::InnerStruct
$EXECUTABLE convert -i  resource/test.idl -o output/ParentStruct.xml -t test::ParentStruct
$EXECUTABLE convert -i  resource/test.idl -o output/ComplexStruct.xml -t test::ComplexStruct

mkdir -p output/python
# setup python environment
pip3 install -r requirements.txt
# install the pyidl2xml package
pip3 install .

SCRIPT_PATH=$(dirname "$0")
PYTHON_SCRIPT_PATH=$SCRIPT_PATH/convert_idl_to_xml.py

python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/PrimitivesStruct.xml -t test::PrimitivesStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/StringsStruct.xml -t test::StringsStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/MyEnum.xml -t test::MyEnum
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/EnumStruct.xml -t test::EnumStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/AliasStruct.xml -t test::AliasStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/SequenceStruct.xml -t test::SequenceStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/ArrayStruct.xml -t test::ArrayStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/MapStruct.xml -t test::MapStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/InnerStruct.xml -t test::InnerStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/ParentStruct.xml -t test::ParentStruct
python3 $PYTHON_SCRIPT_PATH -i resource/test.idl -o output/python/ComplexStruct.xml -t test::ComplexStruct
