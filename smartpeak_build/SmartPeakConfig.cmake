### CMake SmartPeak config file for external code
### configured by the SmartPeak build system from <SmartPeak>/cmake/SmartPeakConfig.cmake.in

# we need this to reference the target file
get_filename_component(SMARTPEAK_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# include directories for targets
set(SmartPeak_INCLUDE_DIRECTORIES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include;C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/src/smartpeak/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/src/openms/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/openms_build/src/openms/include;~/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/src/openms/thirdparty/evergreen/src;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include/sqlite;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include/eigen3;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include/WildMagic;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include;C:/lib/boost/boost_1_73_0;C:/Users/dmccloskey/Documents/GitHub/OpenMS/src/openswathalgo/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/openms_build/src/openswathalgo/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/src/openswathalgo/thirdparty/MIToolbox/include;C:/Users/dmccloskey/Documents/GitHub/OpenMS/src/openswathalgo/thirdparty/MIToolbox/src;C:/lib/boost/boost_1_73_0;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/include/;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/include/QtCore;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/.//mkspecs/win32-msvc;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/include/;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/include/QtNetwork;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/include/QtCore;C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/.//mkspecs/win32-msvc;C:/lib/sdl2/SDL2-2.0.12/include;C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/imgui;C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/implot;C:/Users/dmccloskey/Documents/GitHub/OpenMS/contrib_build/include/eigen3;C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/plog/include")



set(SMARTPEAK_ADDCXX_FLAGS "")

## The targets file
include("${SMARTPEAK_CMAKE_DIR}/SmartPeakTargets.cmake")
