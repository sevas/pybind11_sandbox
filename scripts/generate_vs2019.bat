@echo off
pushd .

set SOURCE_DIR=%~dp0\..
set BUILD_ROOT=%SOURCE_DIR%/.build

if not exist %BUILD_ROOT% md "%BUILD_ROOT%"

cd %SOURCE_DIR%

md "%BUILD_ROOT%/_conan_vs2019"
cd "%BUILD_ROOT%/_conan_vs2019"

conan install "%SOURCE_DIR%"


cd "%SOURCE_DIR%"
md "%BUILD_ROOT%/_build_vs2019"
cd "%BUILD_ROOT%/_build_vs2019"

cmake %SOURCE_DIR% -G "Visual Studio 16 2019" -A x64 -DCONAN_BUILD_DIR=.build/_conan_vs2019




popd

pause
