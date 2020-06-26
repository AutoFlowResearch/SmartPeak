#.rst:
# Find ImPlot
# -------------
#
# Finds the ImPlot library. This module defines:
#
#  ImPlot_FOUND                - True if ImPlot is found
#  ImPlot::ImPlot               - ImPlot interface target
#  ImPlot::Examples            - ImPlot example target
#  ImPlot::Sources             - ImPlot source target for core functionality
#  ImPlot::SourcesMiscCpp      - ImPlot source target for misc/cpp
#
# Additionally these variables are defined for internal usage:
#
#  IMPLOT_INCLUDE_DIR          - Include dir 
#
# The find module tries to find ``implot`` the C++ sources.
# You can supply their location via an ``IMPLOT_DIR`` variable. Once found, the
# ``ImPlot::ImPlot`` target contains just the header file, while
# ``ImPlot::Sources`` contains the source files in ``INTERFACE_SOURCES``.
#
# The ``ImPlot::SourcesMiscCpp`` component, if requested, is always searched for
# in the form of C++ sources.
#
# The desired usage that covers both cases is to link ``ImPlot::Sources``
# ``PRIVATE``\ ly to a *single* target, which will then contain either the
# sources or be linked to the implot library; and linking
# ``ImPlot::ImPlot`` to this target ``PUBLIC``\ ly.

find_path(IMPLOT_INCLUDE_DIR 
	NAMES implot.h
	HINTS ${IMPLOT_DIR}
	NO_CMAKE_FIND_ROOT_PATH
)
mark_as_advanced(IMPLOT_INCLUDE_DIR)
if(NOT TARGET ImPlot::ImPlot)
	add_library(ImPlot::ImPlot INTERFACE IMPORTED)
	set_property(TARGET ImPlot::ImPlot APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${IMPLOT_INCLUDE_DIR})
	message(STATUS "ImPlot::ImPlot interface updated")
endif()

macro(_implot_setup_source_file source_var)
    ## Handle export and import of implot symbols via IMPLOT_API

    # Hide warnings from implot source files

    # GCC- and Clang-specific flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?Clang"
        AND NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC") OR CORRADE_TARGET_EMSCRIPTEN)
        set_property(SOURCE ${${source_var}} APPEND_STRING PROPERTY COMPILE_FLAGS
            " -Wno-old-style-cast -Wno-zero-as-null-pointer-constant")
    endif()

    # GCC-specific flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set_property(SOURCE ${${source_var}} APPEND_STRING PROPERTY COMPILE_FLAGS
            " -Wno-double-promotion")
    endif()

    mark_as_advanced(${source_var})
endmacro()

# Find components
foreach(_component IN LISTS ImPlot_FIND_COMPONENTS)
    if(_component STREQUAL "Sources")
        if(NOT TARGET ImPlot::Sources)
            set(ImPlot_Sources_FOUND TRUE)
            set(ImPlot_SOURCES )

            foreach(_file implot)
                # Disable the find root path here, it overrides the
                # CMAKE_FIND_ROOT_PATH_MODE_INCLUDE setting potentially set in
                # toolchains.
                find_file(ImPlot_${_file}_SOURCE NAMES ${_file}.cpp
                    HINTS ${IMPLOT_DIR} NO_CMAKE_FIND_ROOT_PATH)
                list(APPEND ImPlot_SOURCES ${ImPlot_${_file}_SOURCE})

                if(NOT ImPlot_${_file}_SOURCE)
                    set(ImPlot_Sources_FOUND FALSE)
                    break()
                endif()

                _implot_setup_source_file(ImPlot_${_file}_SOURCE)
            endforeach()

            add_library(ImPlot::Sources INTERFACE IMPORTED)
            set_property(TARGET ImPlot::Sources APPEND PROPERTY
                INTERFACE_SOURCES "${ImPlot_SOURCES}")
            set_property(TARGET ImPlot::Sources APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ImPlot::ImPlot)
        else()
            set(ImPlot_Sources_FOUND TRUE)
        endif()
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ImPlot REQUIRED_VARS IMPLOT_INCLUDE_DIR HANDLE_COMPONENTS)