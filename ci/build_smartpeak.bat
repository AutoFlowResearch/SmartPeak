call "C:/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/VC/Auxiliary/Build/vcvars64.bat"
mkdir build & cd build
cmake -DCMAKE_PREFIX_PATH="C:/OpenMS/build;C:/Qt/5.12/msvc2017_64/lib/cmake;C:/SDL2-2.0.9" ^
  -G "Visual Studio 15 2017 Win64" ^
  -T host=x64 ^
  -DUSE_SUPERBUILD=OFF ^
  -DEIGEN3_INCLUDE_DIR="C:/contrib/build/include/eigen3" ^
  -DEIGEN_USE_GPU=OFF ^
  -DBoost_NO_SYSTEM_PATHS=ON ^
  -DBOOST_USE_STATIC=ON ^
  -DBOOST_ROOT="C:/boost_1_67_0" ^
  -DBOOST_INCLUDEDIR="C:/boost_1_67_0" ^
  -DBOOST_LIBRARYDIR="C:/boost_1_67_0/lib64-msvc-14.1" ^
  -DPLOG_INCLUDE_DIR="C:/plog/include" ^
  -DIMGUI_DIR="C:/imgui" ^
  -DCMAKE_BUILD_TYPE=Debug ^
..
msbuild SmartPeak_host.sln /maxcpucount
exit /b %ERRORLEVEL%
