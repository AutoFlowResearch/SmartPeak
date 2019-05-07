#pragma once

#include <SmartPeak/core/CastValue.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace SmartPeak
{
  typedef std::vector<CastValue> Column;

  class Table {
    std::vector<size_t>      indices_;
    std::vector<Column>      columns_;
    std::vector<std::string> headers_;
    Column& get(const size_t j);
    Column& get(const std::string& header);

  public:
    template<typename T>
    void push(const std::string& header, T&& value)
    {
      push(findIndex(header), value);
    }

    template<typename T>
    void push(const size_t column_idx, T&& value)
    {
      Column& c = get(column_idx);
      const size_t n = c.size();
      c.insert(c.cbegin() + n, std::forward<T>(value));
      if (indices_.size() < c.size()) {
        indices_.insert(indices_.cbegin() + n, n);
        // printf("inserting index %lu\n", n);
      }
      // std::cout << "inserting value " << value << std::endl;
    }

    std::vector<std::string> getHeaders() const;

    CastValue& get(const size_t i, const size_t j);
    const CastValue& get(const size_t i, const size_t j) const;

    CastValue& get(const size_t i, const std::string& header);
    const CastValue& get(const size_t i, const std::string& header) const;

    const Column& get(const size_t j) const;

    const Column& get(const std::string& header) const;

    size_t size() const;
    void clear();
    size_t addColumn(const std::string& column_header);

    void sort(
      const size_t column_idx,
      const bool asc = true,
      const bool case_sensitive = false
    );

    void sort(
      const std::string& column_header,
      const bool asc = true,
      const bool case_sensitive = false
    );

    size_t findIndex(const std::string& header) const;
  };
}
