#
# Linux/MacOS
#   rm -Rf /tmp/out
#   cmake -S . -B /tmp/out -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_TOOLCHAIN_FILE="<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake" -DIMGUI_ROOT="<path/to/imgui>"
#   cmake --build /tmp/out -- -j8
#
# Windows
#   Remove-Item -path E:/TEMP/out -recurse -force
#   cmake -S . -B E:/TEMP/out -G "Ninja" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_TOOLCHAIN_FILE="<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake" -DIMGUI_ROOT="<path/to/imgui>"
#   cmake --build E:/TEMP/out -- -j8
#
cmake_minimum_required(VERSION 3.25)

project(glfw_template)

if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
  message(FATAL_ERROR "Toolchain File is not defined. Please specify the absolute path for Toolchain File")
endif()

if(NOT DEFINED ${IMGUI_ROOT})
  message(FATAL_ERROR "ImGUI is not defined. Please specify the root dir for ImGUI")
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
  find_package(OpenGL REQUIRED) # Linux only?
endif(UNIX)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(IMGUI_SRC "${IMGUI_ROOT}/imstb_rectpack.h"
              "${IMGUI_ROOT}/imstb_textedit.h"
              "${IMGUI_ROOT}/imstb_truetype.h"
              "${IMGUI_ROOT}/imgui.h"
              "${IMGUI_ROOT}/imgui_internal.h"
              "${IMGUI_ROOT}/imgui_tables.cpp"
              "${IMGUI_ROOT}/imconfig.h"
              "${IMGUI_ROOT}/imgui.cpp"
              "${IMGUI_ROOT}/imgui_draw.cpp"
              "${IMGUI_ROOT}/imgui_widgets.cpp"
              "${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
              "${IMGUI_ROOT}/backends/imgui_impl_glfw.h"
              "${IMGUI_ROOT}/backends/imgui_impl_opengl3_loader.h"
              "${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp"
              "${IMGUI_ROOT}/backends/imgui_impl_opengl3.h")

set(LIBRARIES glfw GLEW::GLEW fmt::fmt)

add_executable(main "src/main.cpp" "${IMGUI_SRC}")
target_include_directories(main PRIVATE "${IMGUI_ROOT}")

if (APPLE)
  target_link_libraries(main PRIVATE OpenGL::GL "${LIBRARIES} -framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
  set_target_properties(main PROPERTIES LINK_FLAGS "-Wl,-F/System/Library/Frameworks")
else()
  target_link_libraries(main PRIVATE ${LIBRARIES})
endif()
