@echo off

if "%~1"=="" (
    echo ERROR: Path to cmake.exe not provided
    echo Usage: generate_vs.bat "C:\path\to\cmake.exe" ProjectName
    exit /b 1
)

if "%~2"=="" (
    echo ERROR: Project name not provided
    echo Usage: generate_vs.bat "C:\path\to\cmake.exe" ProjectName
    exit /b 1
)

set CMAKE_EXE=%~1
set PROJECT_NAME=%~2
set BUILD_DIR=build

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

"%CMAKE_EXE%" ^
  -S . ^
  -B %BUILD_DIR% ^
  -G "Visual Studio 17 2022" ^
  -DPROJECT_NAME=%PROJECT_NAME%

if %ERRORLEVEL% NEQ 0 (
    echo CMake generation failed
    exit /b %ERRORLEVEL%
)

echo.
echo Visual Studio project generated successfully
echo Solution: %BUILD_DIR%\%PROJECT_NAME%.sln

