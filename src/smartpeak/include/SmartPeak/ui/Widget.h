#pragma once

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{
  /**
    @brief Abstract base class for all panes, windows, and widgets

    TODO: potential refactors
    - Add unit tests for `makeCheckedRows`
  */
  class Widget
  {
  public:
    Widget() = default;
    virtual ~Widget() = default;

    /**
      Interface to show the widget

      NOTE: free to override in inherited implmementations
    */
    virtual void draw() = 0;

    /**
      Method to make a filter and search popup

      @param[in] popup_id Sequence handler
      @param[in] filter Text filters
      @param[in] colum Column of text items to filter
      @param[in, out] checked Vector of boolean values indicating if the column is filtered or not
      @param[in] values_indices Map containing unique row entries and their duplicate indices
    */
    static void FilterPopup(const char* popuop_id, ImGuiTextFilter& filter, const std::vector<std::string>& column, bool* checked,
      const std::vector<std::pair<std::string, std::vector<size_t>>>& values_indices);

    /**
      Method to make a sort button

      @param[in] button_id button ID
      @param[in] headers Table headers
      @param[in, out] columns Table columns
      @param[in, out] checked Vector of boolean values indicating if the column is filtered or not
      @param[in, out] columns_indices A vector of maps containing unique row entries and their duplicate indices
      @param[in] sort_asc Whether to sort in ascending order or descending order
    */
    static void SortButton(const char* button_id, const std::vector<std::string>& headers, 
      std::vector<std::vector<std::string>>& columns,
      const int n_col,
      bool* checked,
      std::vector<std::vector<std::pair<std::string, std::vector<size_t>>>>& columns_indices,
      bool sort_asc = true);

    /**
      Helper method to make the filters and value_indices needed for `FilterPopup`

      @param[in] headers Table headers
      @param[in] columns Table columns
      @param[out] columns_indices A vector of maps containing unique row entries and their duplicate indices
      @param[out] filter Vector of ImGuiTextFilters
    */
    static void makeFilters(const std::vector<std::string>& headers,
      const std::vector<std::vector<std::string>>& columns, 
      std::vector<std::vector<std::pair<std::string, std::vector<size_t>>>>& columns_indices,
      std::vector<ImGuiTextFilter>& filter);

    /*
      @brief Helper method to create the checked_rows param

      @example
      static bool checked_rows[n_rows];
      makeCheckedRows(n_rows, checked_rows);

      @param[in] n_rows The number of rows
      @param[in,out] checked_rows What rows are checked/filtered
    */
    static void makeCheckedRows(const std::size_t& n_rows, bool* checked_rows);
  };

  /**
    @brief Base class for all text output boxes
  */
  class GenericTextWidget : public Widget
  {
  public:
    void draw() override;
    std::vector<std::string> text_lines;
  };

  /**
    @brief Base class for all tables

    TODO: features
    - row highlighting on focus
    - sorting
    - filtering
  */
  class GenericTableWidget : public Widget
  {
  public:
    /*
    @brief Show the table

    @param[in] headers Column header names
    @param[in,out] columns Columns where the inner vector<string> are individual columns [TODO: refactor to use other types besides strings]
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> columns_;
    bool is_columnar_ = true;
    bool* checked_rows_;
    std::string table_id_;
  };

  /**
    @brief Base class for all tables

    TODO: features
    - row highlighting on focus
    - searching
    - color coding of rows by status
  */
  class ExplorerWidget : public GenericTableWidget
  {
  public:
    /*
    @brief Show the explorer

    @param[in] headers Column header names
    @param[in,out] columns Columns where the inner vector<string> are individual columns [TODO: refactor to use other types besides strings]
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;
    bool* checked_rows_1_;
    bool* checked_rows_2_;
  };

  /**
    @brief Base class for graphics including plots and layouts
  */
  class GenericGraphicWidget : public Widget
  {
  public:
    void draw() override;
  };

  /**
    @brief Class for plotting 2D line plots
  */
  class LinePlot2DWidget : public GenericGraphicWidget
  {
  public:
    void draw() override;
    std::vector<std::vector<float>> x_data_;
    std::vector<std::vector<float>> y_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string x_axis_title_;
    std::string y_axis_title_;
    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    float plot_width_;
    float plot_height_;
    std::vector<std::string> series_names_;
  };

  /**
    @brief Class for plotting 2D line plots
  */
  class ScatterPlot2DWidget : public GenericGraphicWidget
  {
  public:
    void draw() override;
    std::vector<std::vector<float>> x_data_;
    std::vector<std::vector<float>> y_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string x_axis_title_;
    std::string y_axis_title_;
    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    float plot_width_;
    float plot_height_;
    std::vector<std::string> series_names_;
  };

  /**
    @brief Class for plotting 2D line plots
  */
  class CalibratorsPlotWidget : public GenericGraphicWidget
  {
  public:
    void draw() override;
    std::vector<std::vector<float>> x_fit_data_;
    std::vector<std::vector<float>> y_fit_data_;
    std::vector<std::vector<float>> x_raw_data_;
    std::vector<std::vector<float>> y_raw_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string x_axis_title_;
    std::string y_axis_title_;
    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    float plot_width_;
    float plot_height_;
    std::vector<std::string> series_names_;
  };

  /**
    @brief Class for plotting heatmaps
  */
  class Heatmap2DWidget : public GenericGraphicWidget
  {
  public:
    void draw() override;
    std::vector<float> data_; // Row major ordering
    std::vector<std::string> columns_;
    std::vector<std::string> rows_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string x_axis_title_;
    std::string y_axis_title_;
    float data_min_;
    float data_max_;
    float plot_width_;
    float plot_height_;
  };

  /**
    @brief Base class for all tree layouts
  */
  class GenericTreeWidget : public Widget
  {
  public:
    void draw() override;
  };

  /**
    @brief Class for configuring a workflow
  */
  class WorkflowWidget : public GenericGraphicWidget
  {
  public:
    void draw() override;
  };
}