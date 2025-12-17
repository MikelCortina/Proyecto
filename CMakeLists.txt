cmake_minimum_required(VERSION 3.20)

project(${PROJECT_NAME}
    LANGUAGES CXX
)

# --------------------------------------------------
# Ensure required directories exist
# --------------------------------------------------
foreach(dir
    src
    include
    external
    build
)
    if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${dir})
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${dir})
    endif()
endforeach()

# --------------------------------------------------
# C++ configuration
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.20)

project(${PROJECT_NAME}
    LANGUAGES CXX
)

# --------------------------------------------------
# Ensure base directories exist
# --------------------------------------------------
foreach(dir
    src
    include
    external
)
    if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${dir})
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${dir})
    endif()
endforeach()

# --------------------------------------------------
# C++ configuration (C++14)
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# Output directories
# --------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --------------------------------------------------
# Project sources (recursive)
# --------------------------------------------------
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/include/*.hpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# --------------------------------------------------
# OpenGL
# --------------------------------------------------
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME}
    PRIVATE OpenGL::GL
)

# ==================================================
# GLAD (manual: glad.c + headers)
# ==================================================
set(GLAD_ROOT ${CMAKE_SOURCE_DIR}/external/glad)

if (EXISTS ${GLAD_ROOT}/src/glad.c)
    add_library(glad STATIC
        ${GLAD_ROOT}/src/glad.c
    )

    target_include_directories(glad
        PUBLIC
            ${GLAD_ROOT}/include
    )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE glad
    )
endif()

# ==================================================
# GLFW (official repo with CMake)
# ==================================================
set(GLFW_ROOT ${CMAKE_SOURCE_DIR}/external/glfw)

if (EXISTS ${GLFW_ROOT}/CMakeLists.txt)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(${GLFW_ROOT})

    target_link_libraries(${PROJECT_NAME}
        PRIVATE glfw
    )
endif()

# ==================================================
# GLM (header-only)
# ==================================================
set(GLM_ROOT ${CMAKE_SOURCE_DIR}/external/glm)

if (EXISTS ${GLM_ROOT}/glm/glm.hpp)
    target_include_directories(${PROJECT_NAME}
        PRIVATE
            ${GLM_ROOT}
    )
endif()


# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------
if (MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE /W4 /permissive-
    )
else()
    target_compile_options(${PROJECT_NAME}
        PRIVATE -Wall -Wextra -Wpedantic
    )
endif()
