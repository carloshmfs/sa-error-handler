# Build

## CMake

Generate the build files:
```bash
cmake -S . -G "Visual Studio 17 2022" -A Win32 -B build
```

Compile the target:
```bash
cmake --build build
```

Install the target:

> **PS:** Its necessary to have ```GTA_SA_DIR``` enviroment variable on the system. This variable points to your GTA SA directory. This variable is generated from [PluginSDK](https://github.com/DK22Pac/plugin-sdk). But you can create your own.
```bash
cmake --install build --config Debug
```
