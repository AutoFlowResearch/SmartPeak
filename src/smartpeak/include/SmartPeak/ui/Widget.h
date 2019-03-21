#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <map>

namespace SmartPeak
{
  /**
    @brief Abstract base class for all panes, windows, and widgets
  */
  class Widget
  {
  public:
    Widget() = default;
    ~Widget() = default;

    /**
      Interface to show the widget

      NOTE: free to override in inherited implmementations
    */
    virtual void show() {};

    /**
      Helper method to filter a popup

      @param[in] popup_id Sequence handler
      @param[in] filter Text filters
      @param[in] colum Column of text items to filter
      @param[in] checked Vector of boolean values indicating if the column is filtered or not
    */
    static void tableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked);
    static void tableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked, std::map<std::string, std::vector<int>>& values_indices);
  };

  /**
    @brief Base class for all text output boxes
  */
  class GenericTextWidget : public Widget
  {
  public:
    void show(const std::vector<std::string>& text_lines);
  };

  /**
    @brief Base class for all tables

    TODO: potential refactors
    - Extract out methods for making the headers and columns
    - Extract out method for maing the filters (unit testable)
    - Add unit tests for `makeCheckedRows`
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
    void show(const std::vector<std::string>& headers,
      std::vector<std::vector<std::string>>& columns,
    bool* checked_rows);

    /*
    @brief Helper method to create the checked_rows param

    @example
    static bool checked_rows[n_rows];
    makeCheckedRows(n_rows, checked_rows);

    @param[in] n_rows The number of rows
    @param[in,out] checked_rows What rows are checked/filtered
    */
    static void makeCheckedRows(const int& n_rows, bool* checked_rows);
  };

  /**
    @brief Base class for graphics including plots and layouts
  */
  class GenericGraphicWidget : public Widget
  {
  public:
    void show();
  };

  /**
    @brief Base class for all tree layouts
  */
  class GenericTreeWidget : public Widget
  {
  public:
    void show();
  };
  
  /**
    @brief Base class used for all file browsing used in
       loading and storing files
  */
  class FileBrowserWidget : public Widget
  {
  public:
    void show();
  };

  /**
    @brief Class for configuring a workflow
  */
  class WorkflowWidget : public GenericGraphicWidget
  {
  public:
    void show();
  };
}