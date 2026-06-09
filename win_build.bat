@echo off

REM vcpkg install glew:x64-windows-static glfw3:x64-windows-static glm:x64-windows-static

if not defined VCToolsInstallDir (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" -arch=amd64
)

rmdir /s /q build


set "CMAKE_GENERATOR=Ninja"
set "CMAKE_BUILD_TYPE=Release"

echo Building with CMake generator: %CMAKE_GENERATOR%
cmake -S . -B build -DVCPKG_TARGET_TRIPLET=x64-windows-static-md
ninja -C build