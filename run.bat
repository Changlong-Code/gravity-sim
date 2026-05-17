@echo off
echo Building physics engine...
call build.bat
if %ERRORLEVEL% NEQ 0 exit /b 1

echo Launching visualiser...
python viz\visualizer.py
