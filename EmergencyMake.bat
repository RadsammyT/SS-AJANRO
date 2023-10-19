mkdir emergencyBuild
mkdir emergencyBuild/Release
mkdir emergencyBuild/Debug
c++ src/*.cpp -o emergencyBuild/Release/ajanro.exe -O3 -DNDEBUG
c++ src/*.cpp -o emergencyBuild/Debug/ajanro.exe -Og -DDEBUG
