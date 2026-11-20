cmake_minimum_required(VERSION 3.16)

project(GazeboOpenXR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
)

find_package(gz-gui8 REQUIRED)
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(OpenXR REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(gz-rendering8 REQUIRED)

# Explicit Qt MOC generation
qt5_wrap_cpp(MOC_SOURCES
    include/GazeboOpenXR.hh
)

# Explicit Qt resource generation
qt5_add_resources(QRC_SOURCES
    GazeboOpenXR.qrc
)

add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    src/XRRuntime.cc
    src/XRCamera.cc
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)

target_include_directories(GazeboOpenXR PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GazeboOpenXR PRIVATE
    gz-gui8::gz-gui8
    gz-plugin2::register
    OpenXR::openxr_loader
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    OpenGL::GL
    OpenGL::GLX
    X11::X11
    gz-rendering8::gz-rendering8
)

target_compile_options(GazeboOpenXR PRIVATE
    -frtti
)

target_link_options(GazeboOpenXR PRIVATE
    "-Wl,--no-undefined"
)

set_target_properties(GazeboOpenXR PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

