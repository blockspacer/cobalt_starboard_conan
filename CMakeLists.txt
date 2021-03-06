﻿# see https://github.com/chromium/chromium/blob/9db0b5162b8648833c3a6b1af044cebd9d7d3855/base/BUILD.gn

cmake_minimum_required(VERSION 3.4)

# project() must be before checks, see https://stackoverflow.com/a/26437667/10904212
set(PROJECT_NAME "cobalt_starboard")
set(PROJECT_VERSION 1.0.0.1)
project(${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX C)

set(STATIC_LIB_NAME ${PROJECT_NAME}-static)

set(STARBOARD_DIR ${CMAKE_CURRENT_SOURCE_SIR}/cobalt/starboard/)

# --------------------------- conan configuration ------------------------------

# Conan package manager List of packages used by the project is container in
# conanfile.py
option(CONAN_AUTO_INSTALL "Let CMake call conan install automatically" OFF)
if (CONAN_AUTO_INSTALL)
  set(CONAN_PROFILE
      "default"
      CACHE STRING "Conan profile to use during installation")
  include(cmake/conan-auto-install.cmake)
  conan_auto_install(${CONAN_PROFILE})
endif()

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)
  conan_basic_setup(
    # prevent conan_basic_setup from resetting cmake variables
    TARGETS
    KEEP_RPATHS
    # see https://github.com/conan-io/conan/issues/6012
    NO_OUTPUT_DIRS)
else()
  message (FATAL_ERROR "must use conan")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)

find_package(cmake_platform_detection REQUIRED)
run_cmake_platform_detection()
# NOTE: perform platform checks before everything else
include(cmake/PlatformChecks)

find_package(cobalt_glm REQUIRED)
# TODO: add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

find_package(cmake_build_options REQUIRED)
setup_default_build_type(RELEASE)
setup_cmake_build_options(RELEASE DEBUG)

find_package(cobalt_starboard_headers_only REQUIRED)
if(TARGET cobalt_starboard_headers_only::cobalt_starboard_headers_only-static)
  set(cobalt_starboard_headers_only_LIB cobalt_starboard_headers_only::cobalt_starboard_headers_only-static)
else()
  message(FATAL_ERROR "not supported: using system provided cobalt_starboard_headers_only library")
endif()

set(USE_CUSTOM_ICU TRUE) # TODO
if(USE_CUSTOM_ICU)
  # chromium_icu
  if(TARGET_LINUX OR TARGET_WINDOWS OR TARGET_EMSCRIPTEN)
    find_package(chromium_icu REQUIRED)
    if(NOT TARGET chromium_icu::chromium_icu-static)
      message(FATAL_ERROR "not supported: using system provided chromium_icu library")
    endif()
  #elseif(TARGET_WINDOWS)
    # skip
  else()
    message(FATAL_ERROR "chromium_icu not found")
  endif()
  #
  list(APPEND EXTRA_STARBOARD_DEFINES USE_CUSTOM_ICU=1)
  set(CUSTOM_ICU_LIB chromium_icu::chromium_icu-static) # see icu.cmake
else()
  set(CUSTOM_ICU_LIB icu) # platform specific
  message(FATAL_ERROR "TODO: supports not only USE_CUSTOM_ICU")
endif(USE_CUSTOM_ICU)

# uses Config.cmake or a -config.cmake file
# see https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/How-to-create-a-ProjectConfig.cmake-file
find_package(chromium_build_util REQUIRED)
#
if(TARGET chromium_build_util::chromium_build_util-static)
  set(build_util_LIB chromium_build_util::chromium_build_util-static)
else()
  message(FATAL_ERROR "not supported: using system provided chromium_build_util library")
endif()

set(STARBOARD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cobalt/starboard")

include(cmake/CommonOptions)

include(cmake/ProjectFiles)

# $<INSTALL_INTERFACE:...> is exported using install(EXPORT)
# $<BUILD_INTERFACE:...> is exported using export(), or when the target is used by another target in the same buildsystem
macro(add_relative_include_dir TARGET VISIBILITY_BUILD VISIBILITY_INSTALL NEW_ELEM)
  target_include_directories(${TARGET}
    ${VISIBILITY_BUILD} "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${NEW_ELEM}>"
    ${VISIBILITY_INSTALL} "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/${NEW_ELEM}>"
  )
endmacro(add_relative_include_dir)

add_library(${STATIC_LIB_NAME} STATIC
  ${STARBOARD_SOURCES}
)

# path to starboard/export.h
add_relative_include_dir(${STATIC_LIB_NAME} PUBLIC PUBLIC "cobalt")

# add_relative_include_dir(${STATIC_LIB_NAME} INTERFACE INTERFACE "cobalt/starboard")

set_property(TARGET ${STATIC_LIB_NAME} PROPERTY CXX_STANDARD 17)

# POSITION_INDEPENDENT_CODE for -fPIC
# -fPIC may be used to link static library into dynamic one
# Position-independent code requires special support,
# and therefore works only on certain machines.
# see https://akkadia.org/drepper/dsohowto.pdf
set_property(TARGET ${STATIC_LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

if(TARGET_EMSCRIPTEN)
  # use PROPERTY CXX_STANDARD 17
else()
  target_compile_features(${STATIC_LIB_NAME}
    PUBLIC cxx_auto_type
    PRIVATE cxx_variadic_templates)
endif()

target_compile_options(${STATIC_LIB_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall>)

#if(TARGET_WINDOWS)
#  list(APPEND EXTRA_STARBOARD_DEFINES
#    STARBOARD_ATOMIC_INCLUDE="starboard/win/x64x11/atomic_public.h"
#    STARBOARD_THREAD_TYPES_INCLUDE="starboard/win/x64x11/thread_types_public.h"
#    STARBOARD_CONFIGURATION_INCLUDE="starboard/win/x64x11/configuration_public.h"
#  )
#elseif(TARGET_LINUX OR TARGET_EMSCRIPTEN)
#  list(APPEND EXTRA_STARBOARD_DEFINES
#    STARBOARD_ATOMIC_INCLUDE="starboard/linux/x64x11/atomic_public.h"
#    STARBOARD_THREAD_TYPES_INCLUDE="starboard/linux/x64x11/thread_types_public.h"
#    STARBOARD_CONFIGURATION_INCLUDE="starboard/linux/x64x11/configuration_public.h"
#  )
#else()
#  message(FATAL_ERROR "platform not supported")
#endif()
#
#target_compile_definitions(${STATIC_LIB_NAME} PUBLIC
#  ${EXTRA_STARBOARD_DEFINES}
#)

target_compile_definitions(${STATIC_LIB_NAME} PRIVATE
  # starboard/linux/shared/BUILD.gn
  STARBOARD_IMPLEMENTATION=1
  COBALT_IMPLEMENTATION=1
)

if(NOT DEFINED build_util_LIB)
  message(FATAL_ERROR "NOT DEFINED: build_util_LIB")
endif(NOT DEFINED build_util_LIB)

target_link_libraries(${STATIC_LIB_NAME} PUBLIC
  ${build_util_LIB}
  ${cobalt_starboard_headers_only_LIB}
)

if(NOT DEFINED CUSTOM_ICU_LIB)
  message(FATAL_ERROR "CUSTOM_ICU_LIB must be defined")
endif(NOT DEFINED CUSTOM_ICU_LIB)

target_link_libraries(${STATIC_LIB_NAME} PRIVATE
  ${CUSTOM_ICU_LIB}
  # NOTE: force glm from conan, otherwise we can break
  # some isolated builds (emscripten) with -system /usr/include
  CONAN_PKG::cobalt_glm
)

set(DEBUG_LIBRARY_SUFFIX "-d")
set_target_properties(${STATIC_LIB_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /lib
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" # TODO: /bin
    OUTPUT_NAME "${STATIC_LIB_NAME}$<$<CONFIG:Debug>:${DEBUG_LIBRARY_SUFFIX}>"
)

# install and export steps
include(cmake/Exports)
