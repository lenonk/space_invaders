@echo off
cmake -G "Visual Studio 17 2022" -B build-vs2022
if %errorlevel% neq 0 (
    echo CMake failed with error code %errorlevel%
    pause
    exit /b %errorlevel%
)
echo CMake completed successfully

echo You may now open ".\build-vs2022\space_invaders.sln" in Visual Studio 2022.
echo If you want to run the project from within VS, don't forget to set the working directory to the top level space_invaders directory.
echo You'll probably also need to change the default startup project to "space_invaders".

pause