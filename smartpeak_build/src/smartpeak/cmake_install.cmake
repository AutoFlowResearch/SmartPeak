# Install script for directory: C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibraryx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE STATIC_LIBRARY FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/lib/Debug/SmartPeakd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE STATIC_LIBRARY FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/lib/Release/SmartPeak.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE STATIC_LIBRARY FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/lib/MinSizeRel/SmartPeak.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE STATIC_LIBRARY FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/lib/RelWithDebInfo/SmartPeak.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/ApplicationHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/ApplicationProcessor.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/CastValue.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/Filenames.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/Helloworld.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/FeatureMetadata.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/MetaDataHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/RawDataHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/RawDataProcessor.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SampleGroupHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SampleGroupProcessor.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/InjectionHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SampleType.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SequenceHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SequenceProcessor.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SequenceSegmentHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SequenceSegmentProcessor.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SessionHandler.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/SharedProcessors.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/Utilities.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/core" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/core/WorkflowManager.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/algorithm" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/algorithm/MRMFeatureValidator.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/io" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/io/csv.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/io" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/io/CSVWriter.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/io" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/io/FileReader.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/io" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/io/SequenceParser.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/io" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/io/InputDataValidation.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/FIAMS_FullScan_Unknown_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/GCMS_SIM_Unknown_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/HPLC_UV_Standards_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/HPLC_UV_Unknown_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/LCMS_MRM_Standards_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/LCMS_MRM_Unknown_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/pipelines" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/pipelines/LCMS_MRM_Validation_example.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/ui" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/ui/FilePicker.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/ui" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/ui/GuiAppender.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/ui" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/ui/Report.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/ui" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/ui/Widget.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/ui" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/ui/WindowSizesAndPositions.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak/ui" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/smartpeak/include/SmartPeak/ui/Workflow.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/src/smartpeak/include/SmartPeak/config.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/src/smartpeak/include/SmartPeak/smartpeak_package_version.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/src/smartpeak/include/SmartPeak/smartpeak_data_path.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSmartPeak_headersx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SmartPeak" TYPE FILE FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/smartpeak_build/src/smartpeak/include/SmartPeak/SmartPeakConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xsharex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SmartPeak" TYPE DIRECTORY PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ DIR_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ FILES "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/share/SmartPeak/" REGEX "^\\..*" EXCLUDE REGEX ".*\\/\\..*" EXCLUDE)
endif()

