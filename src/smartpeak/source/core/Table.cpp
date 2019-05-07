#include <SmartPeak/core/Table.h>

namespace SmartPeak
{
  std::vector<std::string> Table::getHeaders() const
  {
    return headers_;
  }

  CastValue& Table::get(const size_t r, const size_t c)
  {
    return get(c)[indices_.at(r)];
  }

  const CastValue& Table::get(const size_t r, const size_t c) const
  {
    return get(c).at(indices_.at(r));
  }

  CastValue& Table::get(const size_t r, const std::string& header)
  {
    return get(r, findIndex(header));
  }

  const CastValue& Table::get(const size_t r, const std::string& header) const
  {
    return get(r, findIndex(header));
  }

  Column& Table::get(const size_t c)
  {
    return columns_[c];
  }

  const Column& Table::get(const size_t c) const
  {
    return columns_.at(c);
  }

  Column& Table::get(const std::string& header)
  {
    return get(findIndex(header));
  }

  const Column& Table::get(const std::string& header) const
  {
    return get(findIndex(header));
  }

  size_t Table::size() const
  {
    if (columns_.empty())
      return 0;
    return columns_[0].size();
  }

  void Table::clear()
  {
    headers_.clear();
    indices_.clear();
    columns_.clear();
  }

  size_t Table::addColumn(const std::string& column_header)
  {
    headers_.push_back(column_header);
    const size_t i = findIndex(column_header);
    columns_.insert(columns_.cbegin() + i, Column());
    return i;
  }

  void Table::sort(
    const size_t column_idx,
    const bool asc,
    const bool case_sensitive
  )
  {
    Column& c = get(column_idx);

    if (c.size() != indices_.size())
      throw "can't sort: rows and indices have different sizes";

    std::sort(
      indices_.begin(),
      indices_.end(),
      [&c, asc, case_sensitive](const size_t l, const size_t r)
      {
        const bool b = c[l].is_less_than(c[r], case_sensitive);
        return asc ? b : !b;
      }
    );
  }

  void Table::sort(
    const std::string& column_header,
    const bool asc,
    const bool case_sensitive
  )
  {
    sort(findIndex(column_header), asc, case_sensitive);
  }

  size_t Table::findIndex(const std::string& header) const
  {
    const long int i = std::distance(
      headers_.begin(),
      std::find(headers_.begin(), headers_.end(), header)
    );

    if (i < 0 || static_cast<size_t>(i) >= headers_.size()) {
      throw std::out_of_range("Table: header/column not found");
    }

    return static_cast<size_t>(i);
  }
}
