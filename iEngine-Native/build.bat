@echo off
echo Building iEngine Native...

REM 配置项目
cmake --preset=local-windows

REM 构建项目
cd .\build\local-windows\
cmake --build . --config Debug

echo.
echo Build complete!
pause