@echo off
pushd .

set SOURCE_DIR=%~dp0\..
cd %SOURCE_DIR%
md _conan
cd _conan
conan install ..

cd ..
md _build_vs2019
cd _build_vs2019
cmake .. -G "Visual Studio 16 2019" -A x64




popd

pause