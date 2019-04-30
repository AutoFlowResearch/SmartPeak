#pragma once

#include <string>
#include <vector>
// #include <iostream>
#include <SmartPeak/core/CastValue.h>

namespace SmartPeak
{

  class Column {
    std::vector<size_t>* indices = nullptr; // TODO: try using references? it'd be hard to create copies of columns in different tables
    std::vector<CastValue> rows;
  public:
    Column(std::vector<size_t>* indices, const std::string& header) :
      indices(indices), header(header) {}

    Column(std::vector<size_t>* indices) : indices(indices) {}

    std::string header;

    template<typename T>
    void push_back(T&& value)
    {
      if (!indices)
        throw "Table: indices ptr is null";
      const size_t n = rows.size();
      rows.insert(rows.cbegin() + n, std::forward<T>(value));
      if (indices->size() < rows.size()) {
        indices->insert(indices->cbegin() + n, n);
        // printf("inserting index %lu\n", n);
      }
      // std::cout << "inserting value " << value << std::endl;
    }

    void setIndices(std::vector<size_t>& indices);

    CastValue& get(const size_t i);
    const CastValue& get(const size_t i) const;

    size_t size() const;

    void sort(const bool asc, const bool case_sensitive);
  };

  class Table {
    std::vector<size_t> indices;
    std::vector<Column> columns;
  public:
    std::vector<std::string> getHeaders() const;

    CastValue& get(const size_t i, const size_t j);
    const CastValue& get(const size_t i, const size_t j) const;

    CastValue& get(const size_t i, const std::string& header);
    const CastValue& get(const size_t i, const std::string& header) const;

    Column& get(const size_t j);
    const Column& get(const size_t j) const;

    Column& get(const std::string& header);
    const Column& get(const std::string& header) const;

    size_t size() const;
    void clear();
    Column& addColumn(const std::string& column_header);

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
  };

}
