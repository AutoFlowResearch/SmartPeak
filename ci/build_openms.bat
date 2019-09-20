REM Used in AppVeyor
mkdir C:\projects\OpenMS-build
cd C:\projects\OpenMS-build
cmake -D OPENMS_CONTRIB_LIBS="c:/projects/deps/contrib" -D CMAKE_PREFIX_PATH="c:/Libraries/boost_1_67_0;c:/Qt/5.12/msvc2017_64/" -G "Visual Studio 15 2017 Win64" -T host=x64 -D HAS_XSERVER=Off -D ADDRESS_SANITIZER=Off -D WITH_GUI=Off -D ENABLE_TUTORIALS=Off -D GIT_TRACKING=Off -D ENABLE_UPDATE_CHECK=Off -D PYOPENMS=Off -D CMAKE_BUILD_TYPE=Debug c:\projects\OpenMS
msbuild src\openswathalgo\OpenSWATHAlgo.sln /maxcpucount /p:TrackFileAccess=false /p:CLToolExe=clcache.exe /p:CLToolPath=C:\projects\clcache\install\bin
msbuild src\openms\OpenMS.sln /maxcpucount /p:TrackFileAccess=false /p:CLToolExe=clcache.exe /p:CLToolPath=C:\projects\clcache\install\bin
