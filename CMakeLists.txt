cmake_minimum_required(VERSION 3.28)
project(GameBoy_Emulator LANGUAGES C)

find_package(OpenGL REQUIRED)

# Add library subfolders
add_subdirectory(libs/glfw)

# Recursively find all source files
file(GLOB_RECURSE sources src/*.c include/*.h)

# Add sources to target
add_executable(GameBoy_Emulator ${sources})
#[[target_sources(GameBoy_Emulator PUBLIC
	libs/ImGui/imgui.cpp
	libs/ImGui/imgui_demo.cpp
	libs/ImGui/imgui_draw.cpp
	libs/ImGui/backends/imgui_impl_glfw.cpp
	libs/ImGui/backends/imgui_impl_opengl3.cpp
	libs/ImGui/imgui_tables.cpp
	libs/ImGui/imgui_widgets.cpp
)]]

# Add header files
target_include_directories(GameBoy_Emulator PUBLIC
	include
	include/KHR
	#include/ImGui
	libs)
	#libs/ImGui)

# Link all required libraries
target_link_libraries(GameBoy_Emulator PUBLIC
	glfw
	OpenGL::GL)

# No console on MSVC release builds
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

file(COPY roms DESTINATION ${GameBoy_Emulator_BINARY_DIR})
file(COPY imgui.ini DESTINATION ${GameBoy_Emulator_BINARY_DIR})