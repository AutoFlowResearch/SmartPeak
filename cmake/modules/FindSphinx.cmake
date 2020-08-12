#
# FindSphinx
# Locate Sphinx-build executable
#
# This module will set the following variables when 
# find_package(Sphinx) is called :
# 
# SPHINX_EXECUTABLE : Path to the sphinx-build executable

find_program(SPHINX_EXECUTABLE
            NAMES sphinx-build
            DOC "Path to the sphinx-build executable") 
            
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Sphinx
                                "Failed to find 'sphinx-build'"
                                SPHINX_EXECUTABLE)
