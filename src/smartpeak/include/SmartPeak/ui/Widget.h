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
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/ImEntry.h>
#include <unsupported/Eigen/CXX11/Tensor>

/**
Generic and base classes for Widgets
*/

namespace SmartPeak
{
  /**
    @brief Abstract base class for all panes, windows, and widgets

    TODO: potential refactors
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
  };

  class GenericTextWidget : public Widget
  {
  public:
    void draw() override;
    std::vector<std::string> text_lines;
  };

  /**
    @brief Base class for default tables

    TODO: features
    - row highlighting on focus
    - sorting
    - filtering
  */
  class GenericTableWidget : public Widget
  {
  public:
    GenericTableWidget(const std::string& table_id)
      : table_id_(table_id) {};
    /*
    @brief Show the table

    @param[in] headers Column header names
    @param[in,out] columns Table body or matrix
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;

    /*
    @brief Search across table entries

    @param[in] Im_table_entries vector of ImTableEntry
    @param[in] selected_entry Index of the column to search in starting from 1 as 0 is reserved for `All`
    @param[in] filter Filter of type `ImGuiTextFilter`
    @param[in] row Current row index
    @param[out] returns true if entry is found (to be used in conjuction with continue)
    */
    bool searcher(const std::vector<ImEntry>& Im_table_entries, const int& selected_entry,
      const ImGuiTextFilter& filter, const size_t row) const;

    /*
    @brief Update table contents with text table entries and checkboxes

    @param[in,out] Im_table_entries vector of ImTableEntry
    @param[in] is_scanned true if `columns_` and `checkbox_columns_` are in sync with `Im_table_entries`
    @param[in] columns columns' entries
    @param[in] checkbox_columns checkboxes' entries
    */
    void updateTableContents(std::vector<ImEntry>& Im_table_entries, bool& is_scanned,
      const Eigen::Tensor<std::string, 2>& columns, const Eigen::Tensor<bool, 2>& checkbox_columns);

    /*
    @brief Perform sorting on a given `vector` of `ImTableEntry` elements

    @param[in,out] Im_table_entries vector of ImTableEntry
    @param[in] sorts_specs with sort specs of current table
    @param[in] is_scanned true if `columns_` and `checkbox_columns_` are in sync with `Im_table_entries`
    */
    void sorter(std::vector<ImEntry>& Im_table_entries, ImGuiTableSortSpecs* sorts_specs, const bool& is_scanned);

    Eigen::Tensor<std::string, 1> headers_;
    Eigen::Tensor<std::string, 2> columns_;
    Eigen::Tensor<bool, 1> checked_rows_;
    const std::string table_id_;
    std::vector<ImEntry> table_entries_;
    bool table_scanned_;
    int selected_col_ = 0;
    unsigned int plot_idx_ = 0;
    unsigned int table_entries_plot_col_ = 0;
    unsigned int checkbox_columns_plot_col_ = 0;
    std::string active_plot_;
    std::vector<const char*> cols_;
  };


  /**
    @brief Base class for all tables

    TODO: features
    - row highlighting on focus
    - searching
    - color coding of rows by status
  */
  class ExplorerWidget final : public GenericTableWidget
  {
  public:
    ExplorerWidget(const std::string& table_id)
      :GenericTableWidget(table_id) {};
    /*
    @brief Show the explorer

    @param[in] headers Column header names
    @param[in,out] columns Table body or matrix
    @param[in,out] checked_rows What rows are checked/filtered
    */
    void draw() override;
    Eigen::Tensor<std::string, 1> checkbox_headers_;
    Eigen::Tensor<bool, 2> *checkbox_columns_ = nullptr;
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
  @brief Base class for ScatterPlot widgets
  */
  class ScatterPlotWidget : public GenericGraphicWidget
  {
  public:
    ScatterPlotWidget(SessionHandler& session_handler,
      SequenceHandler& sequence_handler,
      const std::string& title) :
      session_handler_(session_handler),
      sequence_handler_(sequence_handler),
      plot_title_(title) {};
    void setWindowSize(float width, float height) { plot_width_ = width; plot_height_ = height; };
    void setRefreshNeeded() { refresh_needed_ = true; };
    void draw() override;
  protected:
    virtual void updateScatterPlotData() = 0;
  protected:
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    float plot_width_ = 0.0f;
    float plot_height_ = 0.0f;
    const std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    bool show_legend_ = true;
    bool compact_view_ = true;
    SessionHandler::ScatterPlotData chrom_;
    bool refresh_needed_ = false;
    std::pair<float, float> slider_min_max_ = { 0.0f, 0.0f };
    std::pair<float, float> current_range_ = { 0.0f, 0.0f };
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
