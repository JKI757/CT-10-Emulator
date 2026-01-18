cmake_minimum_required(VERSION 3.20)
project(ct10_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90.4
)

FetchContent_MakeAvailable(glfw imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
)

target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_NONE)

add_library(ct10_core
  src/core/bus.cpp
  src/core/execution_engine.cpp
  src/core/instruction_decoder.cpp
  src/core/machine_state.cpp
  src/core/memory.cpp
  src/core/microcode_table.cpp
  src/core/state_io.cpp
  src/core/timing_engine.cpp
  src/core/register.cpp
)

target_include_directories(ct10_core PUBLIC src)

target_compile_features(ct10_core PUBLIC cxx_std_20)

add_library(ct10_ui
  src/ui/debug_pane.cpp
  src/ui/imgui_app.cpp
  src/ui/panel_view.cpp
)

target_include_directories(ct10_ui PUBLIC src)
target_link_libraries(ct10_ui PUBLIC ct10_core imgui)

target_compile_features(ct10_ui PUBLIC cxx_std_20)

add_executable(ct10
  src/app/main.cpp
  src/app/golden_program.cpp
  src/app/main_loop.cpp
  src/app/mode_controller.cpp
  src/app/tape_io.cpp
)

target_link_libraries(ct10 PRIVATE ct10_ui)

add_executable(ct10_headless
  src/app/headless_main.cpp
  src/app/golden_program.cpp
  src/app/tape_io.cpp
)

target_link_libraries(ct10_headless PRIVATE ct10_core)
