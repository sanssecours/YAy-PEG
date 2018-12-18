cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=integer")
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

if(CMAKE_COMPILER_IS_GNUCXX)
  # Work around error “unrecognized option '--push-state'”
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif(CMAKE_COMPILER_IS_GNUCXX)

# ===========
# = Elektra =
# ===========

find_package(Elektra REQUIRED)
include_directories(${ELEKTRA_INCLUDE_DIR})

# =========
# = PEGTL =
# =========

find_package(PEGTL REQUIRED)

# ==========
# = spdlog =
# ==========

find_path(spdlog_INCLUDE_DIR
          NAMES spdlog.h
          DOC "spdlog library header files"
          PATH_SUFFIXES spdlog)
if(spdlog_INCLUDE_DIR STREQUAL spdlog_INCLUDE_DIR-NOTFOUND)
  message(FATAL_ERROR "spdlog not found")
endif(spdlog_INCLUDE_DIR STREQUAL spdlog_INCLUDE_DIR-NOTFOUND)

# ===========
# = Yay PEG =
# ===========

set(SOURCE_DIRECTORY Source)
set(SOURCE_FILES
    ${SOURCE_DIRECTORY}/convert.hpp
    ${SOURCE_DIRECTORY}/convert.cpp
    ${SOURCE_DIRECTORY}/parser.hpp
    ${SOURCE_DIRECTORY}/context.hpp
    ${SOURCE_DIRECTORY}/context.cpp
    ${SOURCE_DIRECTORY}/yaypeg.cpp)

include_directories("${PEGTL_INCLUDE_DIRS}" "${spdlog_INCLUDE_DIR}")
add_executable(yaypeg ${SOURCE_FILES})
target_link_libraries(yaypeg elektra)
