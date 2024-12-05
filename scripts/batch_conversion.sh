#!/bin/bash

EXECUTABLE=./cmake-build-debug/idl2xml
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