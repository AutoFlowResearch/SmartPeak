# Install script for directory: C:/Users/dmccloskey/Documents/GitHub/SmartPeak

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SmartPeak_host")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xsharex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/install/SmartPeakConfig.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xsharex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/SmartPeakConfigVersion.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xsharex" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake/SmartPeakTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake/SmartPeakTargets.cmake"
         "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/CMakeFiles/Export/share/SmartPeak/cmake/SmartPeakTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake/SmartPeakTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake/SmartPeakTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/CMakeFiles/Export/share/SmartPeak/cmake/SmartPeakTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/CMakeFiles/Export/share/SmartPeak/cmake/SmartPeakTargets-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/CMakeFiles/Export/share/SmartPeak/cmake/SmartPeakTargets-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/CMakeFiles/Export/share/SmartPeak/cmake/SmartPeakTargets-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak/cmake" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/CMakeFiles/Export/share/SmartPeak/cmake/SmartPeakTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xapplicationsx" OR NOT CMAKE_INSTALL_COMPONENT)
  
    include(InstallRequiredSystemLibraries)
    include(BundleUtilities)
    # GET_BUNDLE_ALL_EXECUTABLES(C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/bin EXECS)
    SET(EXECS C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/bin/Release/GUI.exe)
    SET(DEST "${CMAKE_INSTALL_PREFIX}/bin/GUI.exe")
    execute_process(COMMAND C:/Program Files/CMake/bin/cmake.exe -E copy "${EXECS}" "${DEST}")
    SET(DIRS C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/lib/cmake/Qt5Core C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/lib/cmake/Qt5Network C:/lib/Qt/qt-5.12.9-dynamic-msvc2019-x86_64/lib/cmake/Qt5 C:/Users/dmccloskey/Documents/GitHub/OpenMS/openms_build/bin/Release C:\lib\boost\boost_1_73_0\lib64-msvc-14.2 C:/lib/sdl2/SDL2-2.0.12/lib/x64)
    fixup_bundle("${DEST}" "" "${DIRS}")
    
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xsharex" OR NOT CMAKE_INSTALL_COMPONENT)
  
    execute_process(COMMAND C:/Program Files/CMake/bin/cmake.exe -E copy_directory C:/Users/dmccloskey/Documents/GitHub/OpenMS/share/OpenMS "${CMAKE_INSTALL_PREFIX}/share/OpenMS")
    
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/src/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
