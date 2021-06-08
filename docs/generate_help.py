# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation
# Center for Biosustainability, Technical University of Denmark 2018-2021.
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
# $Maintainer: Ahmed Khalil $
# $Authors: Ahmed Khalil $
# --------------------------------------------------------------------------

import sys, datetime, os, os.path

file_name   = 'Help'
destination = '../src/smartpeak/include/SmartPeak/ui'
header_path = os.path.join(destination, file_name + '.hh') #TODO

# writing to file with indentation
def write_to_file(file_name, indent_count, source_str):
    while (indent_count > 0):
        source_str = '  ' + source_str
        indent_count -= 1
    file_name.write(source_str)

# writing copyright notice
def include_copyright_notice(file_name, filename):
    file_name.write(
    '// -----------------------------------------------------------------------------\n//\
      SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing\n//\
    --------------------------------------------------------------------------\n//\
    Copyright The SmartPeak Team -- Novo Nordisk Foundation\n//\
    Center for Biosustainability, Technical University of Denmark 2018-{current_year}.\n//\
    \n//\
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"\n//\
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n//\
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n//\
    ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING\n//\
    INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n//\
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n//\
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;\n//\
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,\n//\
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR\n//\
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF\n//\
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n//\
    \n//\
    --------------------------------------------------------------------------\n//\
    THIS FILE IS AUTO GENERATED.\n//\
    --------------------------------------------------------------------------\n\n'
    .format(current_year=str(datetime.datetime.now().year)))


print ('Writing ' + header_path)
with open(header_path, 'w') as header_file:
    t = 0
    # writing copyright notice
    include_copyright_notice(header_file, file_name + '.h')

    # writing directives/includes
    header_file.write('#pragma once\n')
    header_file.write('#include <string>\n')
    header_file.write('#include <unordered_map>\n\n')
    
    # writing help
    write_to_file(header_file, t, 'namespace SmartPeak\n')
    write_to_file(header_file, t, '{\n')
    write_to_file(header_file, t + 1, '/**\n')
    write_to_file(header_file, t + 2, '@brief Documentation/Help for SmartPeakGUI\n')
    write_to_file(header_file, t + 1, '*/\n')
    write_to_file(header_file, t + 1, 'static std::unordered_map<std::string, std::string> tooltip_info\n')
    write_to_file(header_file, t + 1, '{\n')

    #TODO
    write_to_file(header_file, t + 2, '{')
    write_to_file(header_file, t + 1, '}')
    write_to_file(header_file, t, ',\n')

    write_to_file(header_file, t + 1, '};\n')
    write_to_file(header_file, t, '}\n')
