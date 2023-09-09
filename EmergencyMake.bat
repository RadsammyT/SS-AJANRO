mkdir emergencyBuild
mkdir emergencyBuild/Release
mkdir emergencyBuild/Debug
c++ src/main.cpp -o emergencyBuild/Release/ajanro.exe -O3 -DNDEBUG
c++ src/main.cpp -o emergencyBuild/Debug/ajanro.exe -Og -DDEBUG
