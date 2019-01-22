// TODO: Add copyright

#include <SmartPeak/io/FileReader.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/InputDataValidation.h>

namespace SmartPeak
{
  void FileReader::parseOpenMSParams(
    const std::string& filename,
    std::map<std::string,std::vector<std::map<std::string,std::string>>>& parameters
  )
  {
    const std::string s_function {"function"};
    const std::string s_name {"name"};
    const std::string s_value {"value"};
    const std::string s_used {"used_"};
    const std::string s_type {"type"};
    const std::string s_tags {"tags"};
    const std::string s_description {"description"};
    const std::string s_comment {"comment_"};

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
    if (!in.has_column(s_used)
        || !in.has_column(s_function)
        || !in.has_column(s_name)
        || !in.has_column(s_value))
    {
      throw "Missing required column.";
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
    while (in.read_row(function, name, value, used, type, tags, description, comment)) {
      std::transform(used.begin(), used.end(), used.begin(), ::tolower);
      if (used == "false")
        continue;
      std::map<std::string, std::string> m;
      m.emplace(s_name, name);
      m.emplace(s_value, value);
      if (has_type)
        m.emplace(s_type, type);
      if (has_tags)
        m.emplace(s_tags, tags);
      if (has_description)
        m.emplace(s_description, description);
      if (has_comment)
        m.emplace(s_comment, comment);
      if (!parameters.count(function))
        parameters.emplace(function, std::vector<std::map<std::string,std::string>>());
      parameters[function].push_back(m);
    }

    std::cout << InputDataValidation::getParametersInfo(parameters, false);
  }
}
