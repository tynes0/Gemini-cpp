cmake_minimum_required(VERSION 3.20)
project(gemini-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

option(GEMINI_BUILD_EXAMPLES "Build example applications" ON)

file(GLOB_RECURSE GEMINI_SOURCES CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.cpp"
)
file(GLOB_RECURSE GEMINI_HEADERS CONFIGURE_DEPENDS 
    "include/*.h"
)

# If the project is added to someone else's project, let's set the examples to OFF by default.
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(GEMINI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif()

# --- DEPENDENCIES ---
include(FetchContent)

# 1. CPR (HTTP Request Library)
message(STATUS "Downloading CPR Library...")
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.10.5
)

set(CPR_USE_SYSTEM_CURL OFF CACHE BOOL "" FORCE)
set(BUILD_CPR_TESTS OFF CACHE BOOL "" FORCE)
set(CPR_ENABLE_SSL ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cpr)

# 2. Nlohmann JSON
message(STATUS "Downloading JSON Library...")
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

# --- LIBRARIES (Internal) ---

# 1. dtlog
message(STATUS "Adding dtlog Library...")
add_library(dtlog STATIC 
    libs/dtlog/dtlog.cpp 
    libs/dtlog/dtlog.h
)

target_include_directories(dtlog PUBLIC libs/dtlog)

# 2. Frenum
message(STATUS "Adding Frenum Library...")
add_library(frenum INTERFACE)
target_include_directories(frenum INTERFACE libs/frenum)

# --- MAIN LIBRARY (CORE) ---
add_library(gemini-core STATIC 
    ${GEMINI_SOURCES}
    ${GEMINI_HEADERS}
)

add_library(gemini-cpp ALIAS gemini-core)

target_include_directories(gemini-core PUBLIC include)

target_link_libraries(gemini-core 
PUBLIC
    nlohmann_json::nlohmann_json
    dtlog 
    frenum

PRIVATE 
    cpr::cpr 
)

# --- EXAMPLES (Compiles only if GEMINI_BUILD_EXAMPLES is enabled) ---

if(GEMINI_BUILD_EXAMPLES)
    message(STATUS "Gemini Examples are being built...")    
    # demo.cpp
    add_executable(gemini-demo examples/demo.cpp)
    target_link_libraries(gemini-demo PRIVATE gemini-core)
endif()