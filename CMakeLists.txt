cmake_minimum_required(VERSION 3.20)

project(ProtDSL CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  gflags
  GIT_REPOSITORY https://github.com/gflags/gflags.git
  GIT_TAG v2.2.2
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gflags)

set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
if(NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake"
    "${CPM_DOWNLOAD_LOCATION}"
  )
endif()
include("${CPM_DOWNLOAD_LOCATION}")

CPMAddPackage(
  NAME ELFIO
  GITHUB_REPOSITORY serge1/ELFIO
  GIT_TAG Release_3.12
)

add_executable(sim src/main.cpp)

target_include_directories(sim PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Common
  ${CMAKE_CURRENT_SOURCE_DIR}/generated
  ${elfio_SOURCE_DIR}
)

target_link_libraries(sim PRIVATE gflags)
