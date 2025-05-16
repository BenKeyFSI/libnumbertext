cmake_minimum_required(VERSION 3.10)
project(NumberText)

set(NUMBERTEXT_MAJOR_VERSION 1)
set(NUMBERTEXT_MINOR_VERSION 0)
set(NUMBERTEXT_MICRO_VERSION 12)
set(NUMBERTEXT_VERSION "${NUMBERTEXT_MAJOR_VERSION}.${NUMBERTEXT_MINOR_VERSION}.${NUMBERTEXT_MICRO_VERSION}")

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

if (MSVC)
    # warning level 4
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1 _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING=1)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
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
if (MSVC)
  target_compile_definitions(libnumbertext PRIVATE _NUMBER_TEXT_EXPORTS=1)
endif()

# Create executable
add_executable(spellout "${CMAKE_SOURCE_DIR}/src/spellout.cxx")
target_include_directories(spellout PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}/src")
target_link_libraries(spellout libnumbertext)
if(USE_BOOST)
  target_include_directories(spellout PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(spellout ${Boost_LIBRARIES})
endif()
if (MSVC)
  target_compile_definitions(spellout PRIVATE DATADIR="/mingw64/share" _NUMBER_TEXT_IMPORTS=1)
else()
  target_compile_definitions(spellout PRIVATE DATADIR="/usr/local/share")
endif()
