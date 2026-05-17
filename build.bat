@echo off
setlocal

set SRC_DIR=src
set INC_DIR=include
set OUT=gravity_sim.exe
set CXX=g++
set CXXFLAGS=-std=c++14 -Wall -Wextra -O2 -I%INC_DIR%

%CXX% %CXXFLAGS% ^
  %SRC_DIR%\Vector3.cpp ^
  %SRC_DIR%\Body.cpp ^
  %SRC_DIR%\GravitySystem.cpp ^
  %SRC_DIR%\Integrator.cpp ^
  %SRC_DIR%\PhysicsWorld.cpp ^
  %SRC_DIR%\main.cpp ^
  -o %OUT%

if %ERRORLEVEL% NEQ 0 (
    echo BUILD FAILED
    exit /b 1
)
echo Build successful: %OUT%
