# Introduction

In LibNumberText version 1.0.11 and below, there was only one build system available for building LibNumberText. This build system required [GNU Autotools](https://www.gnu.org/software/automake/faq/autotools-faq.html). In LibNumberText version 1.0.12, an alternative build system was introduced that uses [CMake](https://cmake.org/).

# Limitations

Currently the CMake build system is focused on building the LibNumberText library itself and the SpellOut sample application. I makes no attempt to build any other NumberText components.

The CMake build system has been tested in the following environments.

* [Building on Microsoft Windows using Visual Studio 2022][].
* [Building on Microsoft Windows using MinGW64][].

# How to build using CMake

## Building on Microsoft Windows using Visual Studio 2022

The following commands demonstrate how to use the CMake build system to build LibNumberText on Microsoft Windows using Visual Studio 2022. The standard Windows Command Prompt (cmd.exe) was used to execute these commands.

~~~~
mkdir build
cd build
cmake  -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release --verbose
cmake --install . --prefix d:\opt\libnumbertext\1.0.12
~~~~

When attempting to build LibNumberText yourself first open the Visual Studio 2022 x64 Native Tools Command Prompt and CD into the directory containing this file. Change the prefix to meet your needs.

## Building on Microsoft Windows using MinGW64

The following commands demonstrate how to use the CMake build system to build LibNumberText on Microsoft Windows using MinGW64. Note that this does assume that you have set all the appropriate environment variables beforehand. The standard Windows Command Prompt (cmd.exe) was used to execute these commands.

~~~~
mkdir build
cd build
cmake  -G "MinGW Makefiles" ..
cmake --build . --verbose
cmake --install . --prefix f:\opt\libnumbertext\1.0.12
~~~~

When attempting to build LibNumberText yourself first open the Command Prompt, make certain all the necessary environment variables are set correctly, and CD into the directory containing this file. Change the prefix to meet your needs.

## Building on GNU/Linux

The following commands demonstrate how to use the CMake build system to build LibNumberText on GNU/Linux.

~~~~
mkdir build
cd build
cmake  -G "Unix Makefiles" ..
cmake --build . --verbose
cmake --install . --prefix ~/opt/libnumbertext\1.0.12
~~~~

