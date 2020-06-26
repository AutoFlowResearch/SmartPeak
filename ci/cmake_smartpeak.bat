REM Used in AppVeyor
mkdir C:\projects\SmartPeak2\build
cd C:\projects\SmartPeak2\build
cmake ^
  -DCMAKE_PREFIX_PATH="C:/projects/OpenMS-build;C:/Qt/5.12/msvc2017_64/lib/cmake;C:/projects/deps/SDL2-2.0.9" ^
  -G "Visual Studio 15 2017 Win64" ^
  -T host=x64 ^
  -DUSE_SUPERBUILD=OFF ^
  -DEIGEN3_INCLUDE_DIR="C:/projects/deps/contrib/include/eigen3" ^
  -DEIGEN_USE_GPU=OFF ^
  -DBoost_NO_SYSTEM_PATHS=ON ^
  -DBOOST_USE_STATIC=ON ^
  -DBOOST_ROOT="C:/Libraries/boost_1_67_0" ^
  -DBOOST_INCLUDEDIR="C:/Libraries/boost_1_67_0" ^
  -DBOOST_LIBRARYDIR="C:/Libraries/boost_1_67_0/lib64-msvc-14.1" ^
  -DPLOG_INCLUDE_DIR="C:/projects/deps/plog/include" ^
  -DIMGUI_DIR="C:/projects/deps/imgui" ^
  -DIMPLOT_DIR="C:/projects/deps/implot" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  ..
