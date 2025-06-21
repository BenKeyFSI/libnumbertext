cmake_minimum_required(VERSION 3.10)

string(TIMESTAMP CURRENT_YEAR "%Y")
message(STATUS "The current year is: ${CURRENT_YEAR}")

set(NUMBERTEXT_MAJOR_VERSION 1)
set(NUMBERTEXT_MINOR_VERSION 0)
set(NUMBERTEXT_MICRO_VERSION 12)
set(NUMBERTEXT_FILE_VERSION "${NUMBERTEXT_MAJOR_VERSION},${NUMBERTEXT_MINOR_VERSION},${NUMBERTEXT_MICRO_VERSION}")
set(NUMBERTEXT_VERSION "${NUMBERTEXT_MAJOR_VERSION}.${NUMBERTEXT_MINOR_VERSION}.${NUMBERTEXT_MICRO_VERSION}")

set(HOME "$ENV{HOME}")
set(LOCALAPPDATA "$ENV{LOCALAPPDATA}")
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_TARGET_TRIPLET}")
function(set_install_prefix)
  if(VCPKG_ROOT AND VCPKG_TARGET_TRIPLET)
    set(CMAKE_INSTALL_PREFIX "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}" CACHE PATH "CMake install prefix" FORCE)
  elseif(WIN32)
    set(CMAKE_INSTALL_PREFIX "${LOCALAPPDATA}/Programs/libnumbertext/${NUMBERTEXT_VERSION}" CACHE PATH "CMake install prefix" FORCE)
  elseif(HOME)
    set(CMAKE_INSTALL_PREFIX "${HOME}/libnumbertext/${NUMBERTEXT_VERSION}" CACHE PATH "CMake install prefix" FORCE)
  else()
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "CMake install prefix" FORCE)
  endif()
  message(STATUS "CMAKE_INSTALL_PREFIX set to: ${CMAKE_INSTALL_PREFIX}")
endfunction()
set_install_prefix()

project(NumberText)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckIncludeFileCXX)

# Test for codecvt header
check_include_file_cxx(codecvt codecvt_FOUND)
if(codecvt_FOUND)
  set(BOOST_REQUIRED 0)
else()
  set(BOOST_REQUIRED 1)
endif()

if(${BOOST_REQUIRED} EQUAL 0)
  # Test for regex header
  check_include_file_cxx(regex regex_FOUND)
  if(regex_FOUND)
    set(BOOST_REQUIRED 0)
  else()
    set(BOOST_REQUIRED 1)
  endif()
endif()

if(BOOST_REQUIRED)
  find_package(Boost 1.80.0 REQUIRED COMPONENTS locale regex)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(USE_BOOST 1)
  else()
    set(USE_BOOST 0)
  endif()
else()
  set(USE_BOOST 0)
endif()

if(USE_BOOST)
  set(NUMBERTEXT_BOOST "#define NUMBERTEXT_BOOST 1")
else()
  set(NUMBERTEXT_BOOST "")
endif()

configure_file(
  "${CMAKE_SOURCE_DIR}/src/numbertext-version.h.in"
  "${CMAKE_BINARY_DIR}/src/numbertext-version.h")

configure_file(
  "${CMAKE_SOURCE_DIR}/src/version-info-defs.h.in"
  "${CMAKE_BINARY_DIR}/src/version-info-defs.h")

add_compile_definitions(_GNU_SOURCE=1)
add_compile_definitions(_THREAD_SAFE=1)
add_compile_definitions(VERBOSE=1)
if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
    add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING=1)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -fextended-identifiers)
endif()

# Create library
add_library(
  libnumbertext SHARED
  "${CMAKE_SOURCE_DIR}/src/Numbertext.cxx"
  "${CMAKE_SOURCE_DIR}/src/Numbertext.hxx"
  "${CMAKE_SOURCE_DIR}/src/NumberTextAPI.hxx"
  "${CMAKE_SOURCE_DIR}/src/Soros.cxx"
  "${CMAKE_SOURCE_DIR}/src/Soros.hxx")
target_include_directories(libnumbertext PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}/src")
if(USE_BOOST)
  target_include_directories(libnumbertext PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(libnumbertext ${Boost_LIBRARIES})
endif()
target_compile_definitions(libnumbertext PRIVATE LibNumberText_Resources=1)
if(MSVC)
  target_compile_definitions(libnumbertext PRIVATE _NUMBER_TEXT_EXPORTS=1)
  target_sources(libnumbertext PRIVATE "${CMAKE_SOURCE_DIR}/src/version.rc")
endif()
set_target_properties(libnumbertext PROPERTIES
  VERSION "${NUMBERTEXT_VERSION}"
  SOVERSION "${NUMBERTEXT_MAJOR_VERSION}"
  OUTPUT_NAME "libnumbertext-${NUMBERTEXT_VERSION}")

# Create executable
add_executable(spellout "${CMAKE_SOURCE_DIR}/src/spellout.cxx")
target_include_directories(spellout PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}/src")
target_link_libraries(spellout libnumbertext)
if(USE_BOOST)
  target_include_directories(spellout PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(spellout ${Boost_LIBRARIES})
endif()
target_compile_definitions(spellout PRIVATE DATADIR="${CMAKE_INSTALL_PREFIX}/share")
target_compile_definitions(spellout PRIVATE SpellOut_Resources=1)
if(MSVC)
  target_compile_definitions(spellout PRIVATE _NUMBER_TEXT_IMPORTS=1)
  target_sources(spellout PRIVATE "${CMAKE_SOURCE_DIR}/src/version.rc")
endif()

install(TARGETS libnumbertext
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(TARGETS spellout DESTINATION bin)
install(FILES
  "${CMAKE_BINARY_DIR}/src/numbertext-version.h"
  "${CMAKE_SOURCE_DIR}/src/Numbertext.hxx"
  "${CMAKE_SOURCE_DIR}/src/NumberTextAPI.hxx"
  "${CMAKE_SOURCE_DIR}/src/Soros.hxx"
  DESTINATION include) 
install(
  DIRECTORY "${CMAKE_SOURCE_DIR}/data"
  DESTINATION .
  FILES_MATCHING
    PATTERN "*.sor")
