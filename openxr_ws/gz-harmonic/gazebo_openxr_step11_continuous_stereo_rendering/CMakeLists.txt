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
find_package(gz-rendering8 REQUIRED)
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(OpenXR REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

qt5_wrap_cpp(MOC_SOURCES
    include/GazeboOpenXR.hh
)

qt5_add_resources(QRC_SOURCES
    GazeboOpenXR.qrc
)

add_library(GazeboOpenXR SHARED
    src/GazeboOpenXR.cc
    src/XRRuntime.cc
    src/XRCamera.cc
    src/TextureValidation.cc
    src/TextureBlit.cc
    ${MOC_SOURCES}
    ${QRC_SOURCES}
)

target_include_directories(GazeboOpenXR PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GazeboOpenXR PRIVATE
    gz-gui8::gz-gui8
    gz-rendering8::gz-rendering8
    gz-plugin2::register
    OpenXR::openxr_loader
    OpenGL::GL
    OpenGL::GLX
    X11::X11
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

target_link_options(GazeboOpenXR PRIVATE
    "-Wl,--no-undefined"
)

set_target_properties(GazeboOpenXR PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_executable(TextureRenderingDiagnostic
    src/TextureRenderingDiagnostic.cc
    src/XRCamera.cc
    src/TextureValidation.cc
    src/TextureBlit.cc
)
target_include_directories(TextureRenderingDiagnostic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(TextureRenderingDiagnostic PRIVATE
    gz-rendering8::gz-rendering8
    OpenGL::GL
    OpenGL::GLX
    X11::X11
)

include(CTest)
if(BUILD_TESTING)
    add_executable(XRRuntimeSwapchainTest
        tests/XRRuntimeSwapchainTest.cc
        src/XRRuntime.cc
    )
    target_include_directories(XRRuntimeSwapchainTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(XRRuntimeSwapchainTest PRIVATE
        OpenXR::openxr_loader
        OpenGL::GL
        OpenGL::GLX
        X11::X11
    )
    add_test(NAME XRRuntimeSwapchainTest COMMAND XRRuntimeSwapchainTest)

    add_executable(GuiLayoutTest tests/GuiLayoutTest.cc)
    target_link_libraries(GuiLayoutTest PRIVATE gz-gui8::gz-gui8 Qt5::Quick)
    add_dependencies(GuiLayoutTest GazeboOpenXR)
    add_test(NAME GuiLayoutTest COMMAND GuiLayoutTest)
    set_tests_properties(GuiLayoutTest PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software;GZ_GUI_PLUGIN_PATH=$<TARGET_FILE_DIR:GazeboOpenXR>"
    )

    add_test(NAME TextureRenderingDiagnostic COMMAND TextureRenderingDiagnostic)
    set_tests_properties(TextureRenderingDiagnostic PROPERTIES SKIP_RETURN_CODE 77)

    add_executable(CoordinatedRenderGuiTest tests/CoordinatedRenderGuiTest.cc)
    target_link_libraries(CoordinatedRenderGuiTest PRIVATE
        gz-gui8::gz-gui8 gz-rendering8::gz-rendering8 Qt5::Quick
        OpenGL::GL OpenGL::GLX X11::X11
    )
    add_dependencies(CoordinatedRenderGuiTest GazeboOpenXR)
    add_test(NAME CoordinatedRenderGuiTest COMMAND CoordinatedRenderGuiTest)
    set_tests_properties(CoordinatedRenderGuiTest PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=xcb;GZ_GUI_PLUGIN_PATH=$<TARGET_FILE_DIR:GazeboOpenXR>"
        SKIP_RETURN_CODE 77
        TIMEOUT 45
    )

    add_executable(TextureValidationTest
        tests/TextureValidationTest.cc
        src/TextureValidation.cc
        src/TextureBlit.cc
    )
    target_include_directories(TextureValidationTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(TextureValidationTest PRIVATE OpenGL::GL OpenGL::GLX X11::X11)
    add_test(NAME TextureValidationTest COMMAND TextureValidationTest)
    set_tests_properties(TextureValidationTest PROPERTIES SKIP_RETURN_CODE 77)
endif()
