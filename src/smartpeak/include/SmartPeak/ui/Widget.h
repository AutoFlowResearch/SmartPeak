#pragma once

#include <imgui.h>
#include <string>
#include <vector>

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
    */
    virtual void show(bool* p_open) = 0;

    /**
      Helper method to filter a popup

      @param[in] popup_id Sequence handler
      @param[in] filter Text filters
      @param[in] colum Column of text items to filter
      @param[in] checked Vector of boolean values indicating if the column is filtered or not
    */
    static void tableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked);
  };

  /**
    @brief Base class for all text output boxes
  */
  class GenericTextWidget : public Widget
  {
  public:
    void show(bool* p_open);
  };

  /**
    @brief Base class for all tables
  */
  class GenericTableWidget : public Widget
  {
  public:
    void show(bool* p_open);
  };

  /**
    @brief Base class for graphics including plots and layouts
  */
  class GenericGraphicWidget : public Widget
  {
  public:
    void show(bool* p_open);
  };

  /**
    @brief Base class for all tree layouts
  */
  class GenericTreeWidget : public Widget
  {
  public:
    void show(bool* p_open);
  };
  
  /**
    @brief Base class used for all file browsing used in
       loading and storing files
  */
  class FileBrowserWidget : public Widget
  {
  public:
    void show(bool* p_open);
  };

  /**
    @brief Class for configuring a workflow
  */
  class WorkflowWidget : public GenericGraphicWidget
  {
  public:
    void show(bool* p_open);
  };
}