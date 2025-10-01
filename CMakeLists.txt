cmake_minimum_required(VERSION 3.22)
project(sdl3_cimgui_opengl3)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set C standard (since you're using .c files)
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch raylib
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.22
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(sdl3)

# Fetch cimgui
FetchContent_Declare(
    cimgui
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    GIT_TAG master 
    # GIT_TAG 1.92.1
    EXCLUDE_FROM_ALL #this disable build? since customize for sdl3 and opengl.
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(cimgui)

#================================================
# cimgui custom
#================================================

## Define cimgui sources
set(IMGUI_SOURCES
    ${cimgui_SOURCE_DIR}/cimgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_sdl3.cpp
    ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

## Build cimgui as a static library with a custom name
add_library(custom_cimgui STATIC ${IMGUI_SOURCES})

## Set include directories for custom_cimgui
target_include_directories(custom_cimgui PUBLIC
    ${cimgui_SOURCE_DIR}                    # cimgui
    ${cimgui_SOURCE_DIR}/imgui              # cimgui/imgui
    ${cimgui_SOURCE_DIR}/imgui/backends     # imgui/backends
    ${SDL3_SOURCE_DIR}/include              # SDL 3.2
)

## Set compile definitions for custom_cimgui (no CIMGUI_DEFINE_ENUMS_AND_STRUCTS)
target_compile_definitions(custom_cimgui PUBLIC
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
    IMGUI_IMPL_API=extern\ \"C\"
)

## Link custom_cimgui with GLFW and OpenGL
target_link_libraries(custom_cimgui PUBLIC 
    OpenGL::GL 
    SDL3::SDL3
)

#================================================
# Lua
#================================================
FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/lua/lua.git
    GIT_TAG v5.4.8
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(lua)
# Lua doesn't have a CMake build, so we manually compile it
# List Lua source files (core and libraries, excluding lua.c and luac.c)
set(LUA_SRC
    ${lua_SOURCE_DIR}/lapi.c
    ${lua_SOURCE_DIR}/lauxlib.c
    ${lua_SOURCE_DIR}/lbaselib.c
    ${lua_SOURCE_DIR}/lcode.c
    ${lua_SOURCE_DIR}/lcorolib.c
    ${lua_SOURCE_DIR}/lctype.c
    ${lua_SOURCE_DIR}/ldblib.c
    ${lua_SOURCE_DIR}/ldebug.c
    ${lua_SOURCE_DIR}/ldo.c
    ${lua_SOURCE_DIR}/ldump.c
    ${lua_SOURCE_DIR}/lfunc.c
    ${lua_SOURCE_DIR}/lgc.c
    ${lua_SOURCE_DIR}/linit.c
    ${lua_SOURCE_DIR}/liolib.c
    ${lua_SOURCE_DIR}/llex.c
    ${lua_SOURCE_DIR}/lmathlib.c
    ${lua_SOURCE_DIR}/lmem.c
    ${lua_SOURCE_DIR}/loadlib.c
    ${lua_SOURCE_DIR}/lobject.c
    ${lua_SOURCE_DIR}/lopcodes.c
    ${lua_SOURCE_DIR}/loslib.c
    ${lua_SOURCE_DIR}/lparser.c
    ${lua_SOURCE_DIR}/lstate.c
    ${lua_SOURCE_DIR}/lstring.c
    ${lua_SOURCE_DIR}/lstrlib.c
    ${lua_SOURCE_DIR}/ltable.c
    ${lua_SOURCE_DIR}/ltablib.c
    ${lua_SOURCE_DIR}/ltm.c
    ${lua_SOURCE_DIR}/lundump.c
    ${lua_SOURCE_DIR}/lvm.c
    ${lua_SOURCE_DIR}/lzio.c
    ${lua_SOURCE_DIR}/lutf8lib.c
)
# Create a static Lua library
add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC ${lua_SOURCE_DIR})
# ================================================
# flecs
# ================================================
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG v4.1.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(flecs)

#================================================
# ENET
#================================================
# FetchContent_Declare(
#     enet
#     GIT_REPOSITORY https://github.com/zpl-c/enet.git
#     GIT_TAG v2.6.5
#     GIT_SHALLOW TRUE
# )
# set(ENET_TEST  OFF CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(enet)


FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    EXCLUDE_FROM_ALL #this disable build?
    GIT_SHALLOW TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG v0.9.6
    GIT_PROGRESS   TRUE
)
set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cglm)

#================================================
# APP
#================================================

# Application NAME
set(APP_NAME sdl3_test)

# Source files (add more if needed)
set(SRC_FILES
    # Add other .c files here if necessary
    src/gl.c                    # glad 2.0.8
    src/module_font.c           # font
    src/module_cube.c           # font
    src/module_lua.c
    src/module_flecs.c
)

message(STATUS "cimgui_SOURCE_DIR: >> ${cimgui_SOURCE_DIR}")

# Create executable
add_executable(${APP_NAME}
    ${SRC_FILES}
    # src/main.c
    # src/sdl3_window.c
    # src/sdl3_cimgui.c
    # src/sdl3_cimgui_font.c


    # src/sdl3_glad_cimgui_transformer3d.c
    # src/sdl3_glad_cimgui_transformer3d02.c
    # src/sdl3_glad_cimgui_transformer3d03.c
    # src/sdl3_glad_cimgui_transformer3d04.c
    # src/sdl3_glad_cimgui_transformer3d05.c
    # src/sdl3_glad_cimgui_transformer3d07.c # working
    # src/sdl3_glad_cimgui_transformer3d08.c
    # src/sdl3_glad_cimgui_transformer3d09.c
    # src/sdl3_glad_cimgui_transformer3d10.c
    # src/sdl3_glad_cimgui_transformer3d12.c
    src/sdl3_glad_cimgui_transformer3d13.c
    # src/main.c

    #opengl
    
    # examples/sdl3_glad_font.c
    # examples/sdl3_glad_font02.c
    # examples/sdl3_glad_font_cube01.c
    # examples/sdl3_glad_font_cube02.c
    # examples/sdl3_glad_font_cube03.c
    # src/sdl3_glad_font_cube04.c
    # src/sdl3_glad_cimgui_font.c
    # src/sdl3_glad_cimgui.c
    # src/sdl3_glad_cimgui02.c
)

# SDL3 
target_link_libraries(${APP_NAME} PUBLIC SDL3::SDL3) # SDL 3.2.22
target_link_libraries(${APP_NAME} PRIVATE custom_cimgui) # custom cimgui
target_link_libraries(${APP_NAME} PRIVATE lua) # lua
target_link_libraries(${APP_NAME} PRIVATE flecs) # flecs

# Include directories
target_include_directories(${APP_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include             # root project
    ${cglm_SOURCE_DIR}                      # cglm
    ${cimgui_SOURCE_DIR}                    # cimgui
    ${cimgui_SOURCE_DIR}/imgui              # imgui
    ${cimgui_SOURCE_DIR}/imgui/backends     # imgui/backends
    ${SDL3_SOURCE_DIR}/include              # SDL 3.2.22
    ${stb_SOURCE_DIR}                       # stb
    ${lua_SOURCE_DIR}                       # lua
)

# for c #define
target_compile_definitions(
	${APP_NAME}
	PUBLIC
    #CIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
	CIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
	CIMGUI_USE_SDL3=1
	CIMGUI_USE_OPENGL3=1
)

# Platform-specific settings for Windows (MinGW/MSYS2)
if (WIN32)
    # Link necessary Windows libraries for raylib
    target_link_libraries(${APP_NAME} PRIVATE
        opengl32
        gdi32
        winmm
        cglm
    )

    # Static linking for libgcc and libstdc++
    target_link_options(${APP_NAME} PRIVATE
        -static-libgcc
        -static-libstdc++  # Uncomment if C++ code is used
        -static           # Avoid full static linking to prevent issues with system libraries
    )
endif()


# Define the source and destination directories
set(RESOURCE_SRC_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCE_DEST_DIR "${CMAKE_BINARY_DIR}/resources")

# Create the resources directory in the build folder
file(MAKE_DIRECTORY ${RESOURCE_DEST_DIR})

# Copy the resources folder to the build directory
file(COPY ${RESOURCE_SRC_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# Optional: Add a custom target to ensure copying happens during build
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_SRC_DIR} ${RESOURCE_DEST_DIR}
    COMMENT "Copying resources to build directory"
)
