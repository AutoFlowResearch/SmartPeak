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


#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/ImEntry.h>
#include <SmartPeak/ui/Help.h>
#include <unsupported/Eigen/CXX11/Tensor>
#include <SmartPeak/iface/ISequenceSegmentObserver.h>
#include <SmartPeak/iface/IFeaturesObserver.h>
#include <SmartPeak/iface/IPropertiesHandler.h>

#include <string>
#include <utility>
#include <vector>
#include <functional>

/**
Generic and base classes for Widgets
*/

namespace SmartPeak
{
  extern bool enable_quick_help;
  /**
    @brief Abstract base class for all panes, windows, and widgets
  */
  class Widget: public IPropertiesHandler
  {
  public:
    explicit Widget(std::string title = ""):
      title_(title)
    {};
    virtual ~Widget() = default;
    Widget(const Widget &&) = delete;

    /**
      IPropertiesHandler
    */
    virtual std::string getPropertiesHandlerName() const override;
    virtual std::map<std::string, CastValue::Type> getPropertiesSchema() const override;
    virtual std::optional<CastValue> getProperty(const std::string& property, const size_t row) const override;
    virtual void setProperty(const std::string& property, const CastValue& value, const size_t row) override;

    /**
      Interface to show the widget

      NOTE: free to override in inherited implmementations
    */
    virtual void draw() = 0;

    void setWindowSize(float width, float height) { width_ = width; height_ = height; };

    bool visible_ = false;
    std::string title_;
    float width_ = 0.0;
    float height_ = 0.0;
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
    typedef void(SessionHandler::* DataGetterMethod)(const SequenceHandler& sequence_handler, SessionHandler::GenericTableData& generic_table_data);
    typedef Eigen::Tensor<bool, 1>(SessionHandler::* DataFilterMethod)(const Eigen::Tensor<std::string, 2>& to_filter) const;

    /*
    * GenericTableWidget.
    * 
    * A data getter can be provided so that data will be retreive from SessionHandler automatically in the draw method.
    * if not provided, table_data_ must be filled by external mean.
    */
    GenericTableWidget(const std::string& table_id,
      const std::string title = "",
      SessionHandler* session_handler = nullptr,
      SequenceHandler* sequence_handler = nullptr,
      DataGetterMethod data_getter = nullptr,
      DataFilterMethod data_filter = nullptr)
      : Widget(title),
        table_id_(table_id),
        session_handler_(session_handler),
        sequence_handler_(sequence_handler),
        data_getter_(data_getter),
        data_filter_(data_filter)
    {};

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

    SessionHandler::GenericTableData table_data_;
    Eigen::Tensor<bool, 1> checked_rows_;

  protected:
    /*
    @brief whether a cell is editable

    @param[in] row the row of the cell
    @param[in] col the column of the cell
    @param[out] returns true if the cell is editable (onEdit will be called)
    */
    virtual bool isEditable(const size_t row, const size_t col) const { return false; };

    /*
    @brief edit cell callback. to be overriden.
    */
    virtual void onEdit() { };

    /*
    @brief drawing popups. to be overriden.
    */
    virtual void drawPopups() { };

  private:
    void selectCell(size_t row, size_t col);

  protected:
    const std::string table_id_;
    SessionHandler* session_handler_ = nullptr;
    SequenceHandler* sequence_handler_ = nullptr;
    DataGetterMethod data_getter_ = nullptr;
    DataFilterMethod data_filter_ = nullptr;
    std::vector<ImEntry> table_entries_;
    bool table_scanned_ = false;
    bool plot_all_ = false;
    bool plot_unplot_all_deactivated_ = false;
    int selected_col_ = 0;
    int plot_idx_ = -1;
    const int max_rows_ = 100;
    size_t print_until_ = 0;
    unsigned int table_entries_plot_col_ = 0;
    unsigned int checkbox_columns_plot_col_ = 0;
    std::string plot_switch_ = "";
    std::vector<const char*> cols_;
    bool data_changed_ = false;
    std::vector<std::tuple<size_t, size_t>> selected_cells_;
  };

  struct FeaturesTableWidget : public GenericTableWidget, public IFeaturesObserver
  {
    FeaturesTableWidget(const std::string& table_id,
      const std::string title = "",
      FeaturesObservable* observable = nullptr)
    : GenericTableWidget(table_id, title)
    {
      if (observable) observable->addFeaturesObserver(this);
    };

    /**
      IFeaturesObserver
    */
    virtual void onFeaturesUpdated() override
    {
      table_data_.clear();
      data_changed_ = false;
    };
  };


  struct SequenceSegmentWidget : public GenericTableWidget, public ISequenceSegmentObserver
  {
    SequenceSegmentWidget(const std::string& table_id,
      const std::string title = "",
      SessionHandler* session_handler = nullptr,
      SequenceHandler* sequence_handler = nullptr,
      GenericTableWidget::DataGetterMethod data_getter = nullptr,
      GenericTableWidget::DataFilterMethod data_filter = nullptr,
      SequenceSegmentObservable* observable = nullptr)
      : GenericTableWidget(table_id, title, session_handler, sequence_handler, data_getter, data_filter)
    {
      if (observable) observable->addSequenceSegmentObserver(this);
    };

    /**
      ISequenceSegmentObserver
    */
    virtual void onQuantitationMethodsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onStandardsConcentrationsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureFiltersComponentsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureFiltersComponentGroupsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureQCComponentsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureQCComponentGroupsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureRSDFilterComponentsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureRSDFilterComponentGroupsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureRSDQCComponentsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureRSDQCComponentGroupsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureBackgroundFilterComponentsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureBackgroundFilterComponentGroupsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureBackgroundQCComponentsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
    virtual void onFeatureBackgroundQCComponentGroupsUpdated() override
    {
      table_data_.clear();
      data_changed_ = true;
    };
  };

  /**
    @brief Base class for graphics including plots and layouts
  */
  class GenericGraphicWidget : public Widget
  {
  public:
    GenericGraphicWidget(const std::string title = "")
      : Widget(title)
    {};

    void draw() override;

    /**
     @brief some data may have unexpected too much high value - or even infinite. data higher than this constant may be not be displayed.
    */
    static const double high_value_threeshold_;
  };

  /**
    @brief Class for plotting 2D line plots

    NOTE: series data are assumed to be aligned column wise (i.e., each column is a series or x_data_.dimension(1) == series_names_.size())

  */
  class LinePlot2DWidget : public GenericGraphicWidget
  {
  public:
    LinePlot2DWidget(const std::string title = "") : GenericGraphicWidget(title)  {};
    void setValues(const Eigen::Tensor<float, 2>& x_data,
      const Eigen::Tensor<float, 2>& y_data,
      const Eigen::Tensor<std::string, 1>* x_labels,
      const Eigen::Tensor<std::string, 1>* series_names,
      const std::string& x_axis_title, 
      const std::string& y_axis_title,
      const float& x_min,
      const float& x_max,
      const float& y_min,
      const float& y_max,
      const std::string& plot_title)
    {
        x_data_ = x_data;
        y_data_ = y_data;
        x_labels_ = x_labels;
        series_names_ = series_names;
        x_axis_title_ = x_axis_title;
        y_axis_title_ = y_axis_title;
        x_min_ = x_min;
        x_max_ = x_max;
        y_min_ = y_min;
        y_max_ = y_max;
        plot_title_ = plot_title;
    }
    void draw() override;
  protected:
    Eigen::Tensor<float, 2> x_data_;
    Eigen::Tensor<float, 2> y_data_;
    const Eigen::Tensor<std::string, 1>* x_labels_;
    const Eigen::Tensor<std::string, 1>* series_names_;
    std::string x_axis_title_;
    std::string y_axis_title_;
    float x_min_;
    float x_max_;
    float y_min_;
    float y_max_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
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
   @brief Shows Quick Help tooltip when ui_element_name is present in tooltip_info (Help.h).
   
   @param[in,out] ui_element_name such as table_id_.
  */
  static void showQuickHelpToolTip(const std::string& ui_element_name)
  {
    if (ImGui::IsItemHovered() && enable_quick_help && tooltip_info.find(ui_element_name) != tooltip_info.end()) {
      ImGui::SetTooltip("%s", tooltip_info.find(ui_element_name)->second.c_str());
    }
  }
}
