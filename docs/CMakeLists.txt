# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation 
# Center for Biosustainability, Technical University of Denmark 2018-2022.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# --------------------------------------------------------------------------
# $Maintainer: Ahmed Khalil, Douglas McCloskey $
# $Authors: Ahmed Khalil $
# --------------------------------------------------------------------------
# doc generation for SmartPeak lib
# with doxygen (XML) and sphinx for (RtD)

# doxygen
find_package(Doxygen REQUIRED)

set(SMARTPEAK_PUBLIC_HEADER_DIR ${PROJECT_SOURCE_DIR}/../src/smartpeak/include/SmartPeak)
get_target_property(${SMARTPEAK_PUBLIC_HEADER_DIR} SmartPeak INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE SMARTPEAK_PUBLIC_HEADERS LIST_DIRECTORIES true ${SMARTPEAK_PUBLIC_HEADER_DIR}/*.h)


set(DOXYGEN_INPUT_DIR   ${PROJECT_SOURCE_DIR}/../src/smartpeak/)
set(DOXYGEN_OUTPUT_DIR  ${CMAKE_CURRENT_BINARY_DIR}/docs)
set(DOXYGEN_INDEX_FILE  ${DOXYGEN_OUTPUT_DIR}/html/index.html )

set(DOXYFILE_IN   ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})


add_custom_command( OUTPUT            ${DOXYGEN_INDEX_FILE}
                    DEPENDS           ${SMARTPEAK_PUBLIC_HEADERS}
                    COMMAND           ${DOXYGEN_EXECUTABLE}         ${DOXYFILE_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    MAIN_DEPENDENCY   ${DOXYFILE_IN}                ${DOXYFILE_OUT}
                    COMMENT           "Generating docs for SmartPeak lib")


add_custom_target(libdoc-doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})


#sphinx
find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE     ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD      ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/../images  
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/images)

add_custom_command( OUTPUT  ${SPHINX_INDEX_FILE} 
                    COMMAND ${SPHINX_EXECUTABLE} -b html
                    -Dbreathe_projects.SmartPeak=${DOXYGEN_OUTPUT_DIR}/xml
                    ${SPHINX_SOURCE}  ${SPHINX_BUILD}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS           ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                                      ${DOXYGEN_INDEX_FILE}
                    MAIN_DEPENDENCY   ${SPHINX_SOURCE}/conf.py
                    COMMENT           "Generating sphinx docs for SmartPeak lib")

add_custom_target(docs ALL DEPENDS ${SPHINX_INDEX_FILE})

