cmake_minimum_required(VERSION 3.5)
project(mgl C CXX OBJC)

add_subdirectory(subprojects)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glslang REQUIRED)
find_package(SPIRV-Tools REQUIRED)

find_library(METAL_FRAMEWORK Metal)
find_library(OPENGL_FRAMEWORK OpenGL)
find_library(FOUNDATION_FRAMEWORK Foundation)
find_library(QUARTZCORE_FRAMEWORK QuartzCore)
find_library(APPKIT_FRAMEWORK AppKit)

file(GLOB c_srcs src/*.c)
file(GLOB objc_srcs src/*.m)
set_source_files_properties(${objc_srcs} PROPERTIES COMPILE_FLAGS "-fobjc-arc")

add_library(mgl SHARED
    ${c_srcs}
    ${objc_srcs})

target_compile_definitions(mgl PUBLIC 
                                ENABLE_OPT=0 
                                SPIRV_CROSS_C_API_MSL=1
                                SPIRV_CROSS_C_API_GLSL=1
                                SPIRV_CROSS_C_API_CPP=1
                                SPIRV_CROSS_C_API_REFLECT=1
                                )

target_include_directories(mgl PUBLIC include/)
target_include_directories(mgl PUBLIC include/GL)

target_link_libraries(mgl
    glslang::glslang
    glslang::SPIRV
    glslang::glslang-default-resource-limits
    spirv-cross-core
    spirv-cross-c
    spirv-cross-cpp
    spirv-cross-msl
    spirv-cross-glsl
    spirv-cross-hlsl
    spirv-cross-reflect
    ${METAL_FRAMEWORK}
    ${FOUNDATION_FRAMEWORK}
    ${OPENGL_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${APPKIT_FRAMEWORK}
)

find_package(SDL2 REQUIRED)

add_executable(mgl_test test/main.cpp)
target_link_libraries(mgl_test mgl SDL2::SDL2)

target_compile_definitions(mgl_test PUBLIC 
                                ENABLE_OPT=0 
                                SPIRV_CROSS_C_API_MSL=1
                                SPIRV_CROSS_C_API_GLSL=1
                                SPIRV_CROSS_C_API_CPP=1
                                SPIRV_CROSS_C_API_REFLECT=1
                                )