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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <unsupported/Eigen/CXX11/Tensor>

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
    Widget(const Widget &&) = delete;

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
    static void FilterPopup(const char* popuop_id, ImGuiTextFilter& filter, const Eigen::Tensor<std::string,1>& column, bool* checked,
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
    static void SortButton(const char* button_id, const Eigen::Tensor<std::string,1>& headers, 
      Eigen::Tensor<std::string,2>& columns,
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
    static void makeFilters(const Eigen::Tensor<std::string,1>& headers,
      const Eigen::Tensor<std::string,2>& columns, 
      std::vector<std::vector<std::pair<std::string, std::vector<size_t>>>>& columns_indices,
      std::vector<ImGuiTextFilter>& filter);
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
    GenericTableWidget(const Eigen::Tensor<std::string, 1>&headers, const Eigen::Tensor<std::string, 2>&columns, const Eigen::Tensor<bool, 1>&checked_rows, const std::string&table_id)
      : headers_(headers), columns_(columns), checked_rows_(checked_rows), table_id_(table_id) {};
    /*
    @brief Show the table

    @param[in] headers Column header names
    @param[in,out] columns Table body or matrix
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;
    const Eigen::Tensor<std::string,1>& headers_; // keep these `const` and references so that the data is not copied on each call!
    const Eigen::Tensor<std::string,2>& columns_;
    const Eigen::Tensor<bool, 1>& checked_rows_;
    const std::string table_id_; // keep this `const` and non-reference so that the table is not built de-novo on each call!
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
    ExplorerWidget(const Eigen::Tensor<std::string, 1>&headers, const Eigen::Tensor<std::string, 2>&columns, const Eigen::Tensor<bool, 1>&checked_rows, const std::string&table_id, const Eigen::Tensor<std::string, 1>&checkbox_headers, Eigen::Tensor<bool, 2>&checkbox_columns)
      :GenericTableWidget(headers, columns, checked_rows, table_id), checkbox_headers_(checkbox_headers), checkbox_columns_(checkbox_columns) {};
    /*
    @brief Show the explorer

    @param[in] headers Column header names
    @param[in,out] columns Table body or matrix
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;
    const Eigen::Tensor<std::string, 1>& checkbox_headers_;
    Eigen::Tensor<bool,2>& checkbox_columns_;
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

    NOTE: series data are assumed to be aligned column wise (i.e., each column is a series or x_data_.dimension(1) == series_names_.size())

  */
  class LinePlot2DWidget : public GenericGraphicWidget
  {
  public:
    LinePlot2DWidget(const Eigen::Tensor<float, 2>&x_data, const Eigen::Tensor<float, 2>&y_data, const Eigen::Tensor<std::string, 1>&series_names,
      const std::string& x_axis_title, const std::string& y_axis_title, const float& x_min, const float& x_max, const float& y_min, const float& y_max,
      const float& plot_width, const float& plot_height, const std::string& plot_title) :
      x_data_(x_data), y_data_(y_data), series_names_(series_names), x_axis_title_(x_axis_title), y_axis_title_(y_axis_title),
      x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max), plot_width_(plot_width), plot_height_(plot_height), plot_title_(plot_title) {};
    void draw() override;
    const Eigen::Tensor<float, 2>& x_data_;
    const Eigen::Tensor<float, 2>& y_data_;
    const Eigen::Tensor<std::string, 1>& series_names_;
    const std::string& x_axis_title_;
    const std::string& y_axis_title_;
    const float& x_min_;
    const float& x_max_;
    const float& y_min_;
    const float& y_max_;
    const float& plot_width_;
    const float& plot_height_;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
  };

  /**
    @brief Class for plotting 2D line plots
  */
  class ChromatogramPlotWidget : public GenericGraphicWidget
  {
  public:
    ChromatogramPlotWidget(SessionHandler& session_handler, 
                            SequenceHandler& sequence_handler, 
                            const std::string& title) :
      session_handler_(session_handler), 
      sequence_handler_(sequence_handler),
      plot_title_(title) {};
    void setWindowSize(float width, float height) { plot_width_ = width; plot_height_ = height; };
    void setRefreshNeeded() { refresh_needed_ = true; };
    void draw() override;
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    float plot_width_ = 0.0f;
    float plot_height_ = 0.0f;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    bool show_legend_ = true;
    bool compact_view_ = true;
    SessionHandler::ChromatogramScatterPlot chrom_;
    bool refresh_needed_ = false;
    std::pair<float, float> slider_min_max_ = { 0.0f, 0.0f };
    std::pair<float, float> current_range_ = { 0.0f, 0.0f };
    // input used to create the graph
    std::pair<float, float> input_range_ = { 0.0f, 0.0f };
    std::set<std::string> input_sample_names_;
    std::set<std::string> input_component_names_;
  };

  /**
    @brief Class for plotting 2D line plots
  */
  class CalibratorsPlotWidget : public GenericGraphicWidget
  {
  public:
    CalibratorsPlotWidget(const std::vector<std::vector<float>>&x_fit_data, const std::vector<std::vector<float>>&y_fit_data,
      const std::vector<std::vector<float>>&x_raw_data, const std::vector<std::vector<float>>&y_raw_data, const std::vector<std::string>&series_names,
      const std::string& x_axis_title, const std::string& y_axis_title, const float& x_min, const float& x_max, const float& y_min, const float& y_max,
      const float& plot_width, const float& plot_height, const std::string& plot_title) :
      x_fit_data_(x_fit_data), y_fit_data_(y_fit_data), x_raw_data_(x_raw_data), y_raw_data_(y_raw_data), series_names_(series_names), x_axis_title_(x_axis_title), y_axis_title_(y_axis_title),
      x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max), plot_width_(plot_width), plot_height_(plot_height), plot_title_(plot_title) {};
    void draw() override;
    const std::vector<std::vector<float>>& x_fit_data_;
    const std::vector<std::vector<float>>& y_fit_data_;
    const std::vector<std::vector<float>>& x_raw_data_;
    const std::vector<std::vector<float>>& y_raw_data_;
    const std::vector<std::string>& series_names_;
    const std::string& x_axis_title_;
    const std::string& y_axis_title_;
    const float& x_min_;
    const float& x_max_;
    const float& y_min_;
    const float& y_max_;
    const float& plot_width_;
    const float& plot_height_;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
  };

  /**
    @brief Class for plotting heatmaps
  */
  class Heatmap2DWidget : public GenericGraphicWidget
  {
  public:
    Heatmap2DWidget(const Eigen::Tensor<float, 2, Eigen::RowMajor>& data, const Eigen::Tensor<std::string, 1>& columns, const Eigen::Tensor<std::string, 1>& rows,
      const std::string& x_axis_title, const std::string& y_axis_title, const float& data_min, const float& data_max,
      const float& plot_width, const float& plot_height, const std::string& plot_title)
      :data_(data), columns_(columns), rows_(rows), x_axis_title_(x_axis_title), y_axis_title_(y_axis_title), data_min_(data_min), data_max_(data_max),
      plot_width_(plot_width), plot_height_(plot_height), plot_title_(plot_title){};
    void draw() override;
    const Eigen::Tensor<float, 2, Eigen::RowMajor>& data_; // Row major ordering
    const Eigen::Tensor<std::string,1>& columns_;
    const Eigen::Tensor<std::string,1>& rows_;
    const std::string& x_axis_title_;
    const std::string& y_axis_title_;
    const float& data_min_;
    const float& data_max_;
    const float& plot_width_;
    const float& plot_height_;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
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