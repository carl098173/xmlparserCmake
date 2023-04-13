# CMakeLists.txt : CMake project for xmlparserCmake, include source and define
# project specific logic here.
#

# Include source files
add_executable(xmlparserCmakeApp xmlparserCmake.cpp py.cpp)

# Find Python and link
find_package(Python COMPONENTS Interpreter Development REQUIRED)
target_link_libraries(xmlparserCmakeApp PRIVATE Python::Python)

# Set C++ standard
set_target_properties(xmlparserCmakeApp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
