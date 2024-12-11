cmake_minimum_required(VERSION 3.10)
project(idl2xml)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(thirdparty/xtypes)
add_subdirectory(thirdparty/pugixml)

add_library(dynamic_type_xml STATIC
    src/dynamic_type_xml.cpp
    src/utils.cpp
    )

set_target_properties(dynamic_type_xml PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(dynamic_type_xml PUBLIC Threads::Threads PRIVATE eprosima::xtypes)
target_link_libraries(dynamic_type_xml PRIVATE pugixml-static)
target_include_directories(dynamic_type_xml INTERFACE src)

add_executable(idl2xml src/cmd/idl2xml.cpp)
target_link_libraries(idl2xml PRIVATE dynamic_type_xml)
target_include_directories(idl2xml PRIVATE thirdparty/CLI11/include)

add_subdirectory(thirdparty/pybind11)

pybind11_add_module(pyidl2xml src/python/pyidl2xml.cpp)
target_link_libraries(pyidl2xml PRIVATE dynamic_type_xml)
target_compile_definitions(pyidl2xml PRIVATE VERSION_INFO=${PYIDL2XML_VERSION_INFO})

install(TARGETS idl2xml DESTINATION bin)
install(TARGETS pyidl2xml DESTINATION lib)
