#include <SmartPeak/core/Table.h>

namespace SmartPeak
{
  size_t Column::size() const
  {
    return rows.size();
  }

  void Column::setIndices(std::vector<size_t>& indices)
  {
    Column::indices = &indices;
  }

  Utilities::CastValue& Column::get(const size_t i)
  {
    if (!indices)
      throw "Table: indices ptr is null\n";
    return rows[(*indices)[i]];
  }

  const Utilities::CastValue& Column::get(const size_t i) const
  {
    if (!indices)
      throw "Table: indices ptr is null\n";
    return rows.at((*indices)[i]);
  }

  void Column::sort(const bool asc, const bool case_sensitive)
  {
    if (rows.size() != indices->size())
      throw "can't sort: rows and indices have different sizes";

    std::sort(
      indices->begin(),
      indices->end(),
      [this, asc, case_sensitive](const size_t l, const size_t r)
      {
        const bool b = rows[l].is_less_than(rows[r], case_sensitive);
        return asc ? b : !b;
      }
    );
  }

  std::vector<std::string> Table::getHeaders() const
  {
    std::vector<std::string> names;
    for (const Column& c : columns) {
      names.push_back(c.header);
    }
    return names;
  }

  // NOTE: if columns is a map instead of a vector
  // std::vector<std::string> Table::getHeaders() const
  // {
  //   std::vector<std::string> names;
  //   for (const std::pair<size_t, Column>& p : columns) {
  //     names.push_back(p.second.header);
  //   }
  //   return names;
  // }

  Utilities::CastValue& Table::get(const size_t r, const size_t c)
  {
    return get(c).get(r);
  }

  const Utilities::CastValue& Table::get(const size_t r, const size_t c) const
  {
    return get(c).get(r);
  }

  Utilities::CastValue& Table::get(const size_t r, const std::string& header)
  {
    return get(header).get(r);
  }

  const Utilities::CastValue& Table::get(const size_t r, const std::string& header) const
  {
    return get(header).get(r);
  }

  Column& Table::get(const size_t c)
  {
    return columns[c];
  }

  const Column& Table::get(const size_t c) const
  {
    return columns.at(c);
  }

  Column& Table::get(const std::string& header)
  {
    std::vector<Column>::iterator it = std::find_if(
      columns.begin(),
      columns.end(),
      [&header](Column& c){
        return c.header == header;
      }
    );

    if (it == columns.end())
      throw "Table: column not found: " + header + "\n";
    
    return *it;
  }

  const Column& Table::get(const std::string& header) const
  {
    std::vector<Column>::const_iterator it = std::find_if(
      columns.cbegin(),
      columns.cend(),
      [&header](const Column& c){
        return c.header == header;
      }
    );

    if (it == columns.cend())
      throw "Table: column not found: " + header + "\n";
    
    return *it;
  }

  size_t Table::size() const
  {
    if (columns.empty())
      return 0;
    return columns[0].size();
  }

  void Table::clear()
  {
    indices.clear();
    columns.clear();
  }

  Column& Table::addColumn(const std::string& column_header)
  {
    return *columns.emplace(columns.cend(), &indices, column_header);
  }

  void Table::sort(
    const size_t column_idx,
    const bool asc,
    const bool case_sensitive
  )
  {
    get(column_idx).sort(asc, case_sensitive);
  }

  void Table::sort(
    const std::string& column_header,
    const bool asc,
    const bool case_sensitive
  )
  {
    get(column_header).sort(asc, case_sensitive);
  }
}
