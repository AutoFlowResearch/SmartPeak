include (ExternalProject)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)

set (CONTRIB_VERSION 8eb2d4ab51)
message (STATUS "Adding CONTRIB ${CONTRIB_VERSION} as an external project.")

ExternalProject_Add(contrib
    GIT_REPOSITORY "https://github.com/OpenMS/contrib.git"
    GIT_TAG ${CONTRIB_VERSION}
    CMAKE_ARGS -DBUILD_TYPE=ALL -DNUMBER_OF_JOBS=4
    INSTALL_COMMAND ""
    GIT_PROGRESS 1
    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
)

set (OPENMS_VERSION develop)
message (STATUS "Adding OPENMS ${OPENMS_VERSION} as an external project.")

ExternalProject_Add(openms
    GIT_REPOSITORY "https://github.com/OpenMS/OpenMS.git"
    GIT_TAG ${OPENMS_VERSION}
	GIT_SUBMODULE "THIRDPARTY"
    CMAKE_ARGS 
        -DPYOPENMS=OFF
        -DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/contrib/build
        -DBOOST_USE_STATIC=OFF
        -DHAS_XSERVER=OFF
        -DWITH_GUI=OFF
    INSTALL_COMMAND ""
    GIT_PROGRESS 1
    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
)

ExternalProject_Add_StepDependencies(openms build contrib)