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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/io/ParametersParser.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/io/CSVWriter.h>

namespace SmartPeak
{
  const std::string s_function{ "function" };
  const std::string s_name{ "name" };
  const std::string s_value{ "value" };
  const std::string s_used{ "used_" };
  const std::string s_type{ "type" };
  const std::string s_tags{ "tags" };
  const std::string s_description{ "description" };
  const std::string s_comment{ "comment_" };

  void ParametersParser::read(
    const std::string& filename,
    ParameterSet& parameters
  )
  {
    parameters.clear();

    io::CSVReader<
      8,
      io::trim_chars<' ', '\t'>,
      io::double_quote_escape<',', '\"'>, // io::no_quote_escape<','>, // io::double_quote_escape<',', '\"'>,
      io::no_comment // io::single_line_comment<'#'>
    > in(filename);

    in.read_header(
      io::ignore_extra_column | io::ignore_missing_column,
      s_function,
      s_name,
      s_value,
      s_used,
      s_type,        // optional
      s_tags,        // optional
      s_description, // optional
      s_comment      // optional
    );

    // check for required columns
    static const std::vector<std::string>
      required_column{ s_used , s_function, s_name, s_value };
    for (const auto& column : required_column)
    {
      if (!in.has_column(column))
      {
        std::ostringstream os;
        os << "Missing required column \'" << column  << "\'";
        throw std::invalid_argument(os.str());
      }
    }

    const bool has_type = in.has_column(s_type);
    const bool has_tags = in.has_column(s_tags);
    const bool has_description = in.has_column(s_description);
    const bool has_comment = in.has_column(s_comment);
    std::string function;
    std::string name;
    std::string value;
    std::string used;
    std::string type;
    std::string tags;
    std::string description;
    std::string comment;
    unsigned int row = 1; // we have read the header, and we will start line 1 as much text editors do
    while (in.read_row(function, name, value, used, type, tags, description, comment)) {
      row++;
      std::transform(used.begin(), used.end(), used.begin(), ::tolower);
      if (used == "false")
        continue;
      std::map<std::string, std::string> properties =
      {
        {"name", name},
        {"value", value},
        {"used", used},
        {"type", type},
        {"tags", tags},
        {"description", description},
        {"comment", comment}
      };
      try
      {
        auto p = Parameter(properties);
        parameters.addParameter(function, p);
      }
      catch (const std::exception& e)
      {
        LOG_ERROR << filename << ", Error line " << row << ": " << e.what();
        throw;
      }
    }
  }

  void ParametersParser::write(
    const std::string& filename,
    const ParameterSet& parameter_set
  )
  {
    std::vector<std::string> headers = { 
      s_function,
      s_name,
      s_value,
      s_used,
      s_type,        // optional
      s_tags,        // optional
      s_description, // optional
      s_comment 
    };

    CSVWriter writer(filename, ",");
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    std::vector<std::vector<std::string>> rows;
    for (const auto& parameter_function : parameter_set)
    {
      const auto parameter_function_name = parameter_function.first;
      for (const auto& parameter : parameter_function.second)
      {
        std::vector<std::string> row;
        row.push_back(parameter_function_name);
        row.push_back(parameter.getName());
        if ((parameter.getType() == "string")     || 
            (parameter.getType() == "bool_list")  ||
            (parameter.getType() == "float_list") ||
            (parameter.getType() == "int_list")   ||
            (parameter.getType() == "string_list"))
        {
          row.push_back(decorateString(parameter.getValueAsString()));
        }
        else
        {
          row.push_back(parameter.getValueAsString());
        }
        row.push_back("true"); // Parameters do not have the concept of used
        if ((parameter.getType() == "bool_list") ||
            (parameter.getType() == "float_list") ||
            (parameter.getType() == "int_list") ||
            (parameter.getType() == "string_list"))
        {
          row.push_back("list");
        }
        else
        {
          row.push_back(parameter.getType());
        }
        const auto tags = parameter.getTags();
        std::ostringstream tags_as_string;
        if (!tags.empty())
        {
          tags_as_string << "\"[";
          std::string separator = "";
          for (const auto& tag : tags)
          {
            tags_as_string << separator << "'" << tag << "'";
            separator = ",";
          }
          tags_as_string << "]\"";
        }
        row.push_back(tags_as_string.str());
        row.push_back(decorateString(parameter.getDescription()));
        row.push_back(""); // Parameters do not have the concept of comment
        rows.push_back(row);
      }
    }
    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }
  }

  std::string ParametersParser::decorateString(const std::string& string_in)
  {
    return std::string("\"") + string_in + std::string("\"");
  }
}
