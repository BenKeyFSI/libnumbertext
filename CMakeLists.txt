cmake_minimum_required(VERSION 3.10)

set(NUMBERTEXT_MAJOR_VERSION 1)
set(NUMBERTEXT_MINOR_VERSION 0)
set(NUMBERTEXT_MICRO_VERSION 12)
set(NUMBERTEXT_COMPANYNAME "László Németh et al.")
set(NUMBERTEXT_COPYRIGHT "Copyright (C) 2009 - 2025 László Németh et al.")
set(NUMBERTEXT_DESCRIPTION "Numbertext is a library for converting numbers to words in many languages.")
set(NUMBERTEXT_FILE_VERSION "${NUMBERTEXT_MAJOR_VERSION},${NUMBERTEXT_MINOR_VERSION},${NUMBERTEXT_MICRO_VERSION}")
set(NUMBERTEXT_PRODUCTNAME "Numbertext")
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

add_compile_definitions(_GNU_SOURCE=1)
add_compile_definitions(_THREAD_SAFE=1)
add_compile_definitions(VERBOSE=1)
if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
    add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING=1)
    add_compile_definitions(VER_COMPANYNAME_STR="${NUMBERTEXT_COMPANYNAME}")
    add_compile_definitions(VER_FILEDESCRIPTION_STR="${NUMBERTEXT_DESCRIPTION}")
    add_compile_definitions(VER_FILEVERSION=${NUMBERTEXT_FILE_VERSION})
    add_compile_definitions(VER_FILEVERSION_STR="${NUMBERTEXT_VERSION}")
    add_compile_definitions(VER_LEGALCOPYRIGHT_STR="${NUMBERTEXT_COPYRIGHT}")
    add_compile_definitions(VER_PRODUCTNAME_STR="${NUMBERTEXT_PRODUCTNAME}")
    add_compile_definitions(VER_PRODUCTVERSION=${NUMBERTEXT_FILE_VERSION})
    add_compile_definitions(VER_PRODUCTVERSION_STR="${NUMBERTEXT_VERSION}")
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
if(MSVC)
  target_compile_definitions(libnumbertext PRIVATE _NUMBER_TEXT_EXPORTS=1)
  target_compile_definitions(libnumbertext PRIVATE VER_INTERNALNAME_STR="libnumbertext-${NUMBERTEXT_VERSION}.dll")
  target_compile_definitions(libnumbertext PRIVATE VER_ORIGINALFILENAME_STR="libnumbertext-${NUMBERTEXT_VERSION}.dll")
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
if(MSVC)
  target_compile_definitions(spellout PRIVATE _NUMBER_TEXT_IMPORTS=1)
  target_compile_definitions(spellout PRIVATE DATADIR="${CMAKE_INSTALL_PREFIX}/share")
  target_compile_definitions(spellout PRIVATE VER_INTERNALNAME_STR="spellout.exe")
  target_compile_definitions(spellout PRIVATE VER_ORIGINALFILENAME_STR="spellout.exe")
  target_sources(spellout PRIVATE "${CMAKE_SOURCE_DIR}/src/version.rc")
else()
  target_compile_definitions(spellout PRIVATE DATADIR="${CMAKE_INSTALL_PREFIX}/share")
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
