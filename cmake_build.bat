@echo off

cmake -S . -G "Visual Studio 17 2022" -A Win32 -B build

cmake --build build

cmake --install build --config Debug
