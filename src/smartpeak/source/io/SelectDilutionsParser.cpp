// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/io/SelectDilutionsParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <plog/Log.h>

namespace SmartPeak
{
  const std::string s_component_name{ "component_name" };
  const std::string s_dilution_factor{ "dilution_factor" };

  void SelectDilutionsParser::read(
    const std::string& filename,
    std::map<std::string,int>& dilution_map
  )
  {
    io::CSVReader<
      2,
      io::trim_chars<' ', '\t'>,
      io::double_quote_escape<',', '\"'>, // io::no_quote_escape<','>, // io::double_quote_escape<',', '\"'>,
      io::no_comment // io::single_line_comment<'#'>
    > in(filename);

    in.read_header(
      io::ignore_extra_column | io::ignore_missing_column,
      s_component_name,
      s_dilution_factor
    );

    // check for required columns
    static const std::vector<std::string>
      required_column{ s_component_name, s_dilution_factor };
    for (const auto& column : required_column)
    {
      if (!in.has_column(column))
      {
        std::ostringstream os;
        os << "Missing required column \'" << column << "\'";
        throw std::invalid_argument(os.str());
      }
    }

    std::string component;
    std::string dilution_factor;
    unsigned int row = 1; // we have read the header, and we will start line 1 as much text editors do
    while (in.read_row(component, dilution_factor)) {
      row++;
      std::map<std::string, std::string> properties =
      {
        {s_component_name, component},
        {s_dilution_factor, dilution_factor}
      };
      try
      {
        dilution_map.insert_or_assign(component, std::stoi(dilution_factor));
      }
      catch (const std::exception& e)
      {
        LOG_ERROR << filename << ", Error line " << row << ": " << e.what();
        throw;
      }
    }
  }
}
