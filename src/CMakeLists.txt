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
# $Maintainer: Douglas McCloskey, Ahmed Khalil $
# $Authors: Douglas McCloskey $
# --------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeak_src")

#------------------------------------------------------------------------------
# SmartPeak core lib
#------------------------------------------------------------------------------
add_subdirectory(smartpeak)

#------------------------------------------------------------------------------
# SmartPeak Widgets
#------------------------------------------------------------------------------
add_subdirectory(widgets)

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------
option (BUILD_TESTS "Whether or not build unit tests" ON)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Examples
#------------------------------------------------------------------------------
option (BUILD_EXAMPLES "Whether or not build the examples" ON)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
