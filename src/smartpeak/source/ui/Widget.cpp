#include <SmartPeak/ui/Widget.h>
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <imgui.h>
#include <implot.h>

namespace SmartPeak
{
  void Widget::FilterPopup(
    const char* popuop_id,
    ImGuiTextFilter& filter,
    const std::vector<std::string>& column,
    bool* checked,
    const std::vector<std::pair<std::string, std::vector<size_t>>>& values_indices) {
    if (false == ImGui::BeginPopup(popuop_id))
      return;

    //// Sorting
    //if (ImGui::Button("Asc"))
    //{
    //  // TODO: will need to rethink how this function is called to implement sorting on the entire table
    //  //std::sort(std::begin(column), std::end(column), [](std::string a, std::string b) {return a > b; });
    //  //std::sort(std::begin(values_indices), std::end(values_indices), [](std::pair<std::string, std::vector<int>> a, std::pair<std::string, std::vector<int>> b) {return a.first > b.first; });
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Desc"))
    //{
    //  // TODO: will need to rethink how this function is called to implement sorting on the entire table
    //  //std::sort(std::begin(column), std::end(column), [](std::string a, std::string b) {return a < b; });
    //  //std::sort(std::begin(values_indices), std::end(values_indices), [](std::pair<std::string, std::vector<int>> a, std::pair<std::string, std::vector<int>> b) {return a.first < b.first; });
    //}
    //ImGui::Separator();

    // Filtering and selecting
    filter.Draw();

    if (ImGui::Button("check all")) {
      for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
        for (const size_t i: indexes.second) {
          checked[i] = 1;
        }
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("uncheck all")) {
      for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
        for (const size_t i : indexes.second) {
          checked[i] = 0;
        }
      }
    }

    for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
      if (filter.PassFilter(indexes.first.c_str())) {
        ImGui::Checkbox(indexes.first.c_str(), &checked[indexes.second.front()]);
      }
    }

    // Update checked rows not explicitly shown
    for (const std::pair<std::string, std::vector<size_t>>& indexes : values_indices) {
      for (const size_t i : indexes.second) {
        checked[i] = checked[indexes.second.front()];
      }
    }

    // NOTE: Appears to apply the filter immediately so this is not needed
    // // Apply filters
    // ImGui::Separator();
    // if (ImGui::Button("Accept"))
    // {
    //   //TODO: apply filter and exit
    // }
    // ImGui::SameLine();
    // if (ImGui::Button("Cancel"))
    // {
    //   //TODO: exit without applying filter
    // }

    ImGui::EndPopup();
  }

  void Widget::makeFilters(
    const std::vector<std::string>& headers,
    const std::vector<std::vector<std::string>>& columns,
    std::vector<std::vector<std::pair<std::string, std::vector<std::size_t>>>>& columns_indices,
    std::vector<ImGuiTextFilter>& filter) {
    for (std::size_t col = 0; col < headers.size(); ++col)
    {
      // Extract out unique columns and replicate indices
      std::map<std::string, std::vector<std::size_t>> value_indices;
      for (std::size_t row = 0; row < columns[col].size(); ++row) {
        std::vector<std::size_t> index = { row };
        auto found = value_indices.emplace(columns[col][row], index);
        if (!found.second) {
          value_indices.at(columns[col][row]).push_back(row);
        }
      }

      // Copy into a vector of pairs (used for downstream sorting)
      std::vector<std::pair<std::string, std::vector<std::size_t>>> value_indices_v;
      std::copy(
        value_indices.begin(),
        value_indices.end(),
        std::back_inserter(value_indices_v)
      );
      columns_indices.push_back(value_indices_v);

      // Initialize the filters
      static ImGuiTextFilter filter0;
      filter.push_back(filter0);
    }
  }
  void Widget::makeCheckedRows(const std::size_t& n_rows, bool* checked_rows)
  {
    for (std::size_t i=0;i<n_rows;++i)
      checked_rows[i] = true;
  }

  void GenericTableWidget::draw()
  {
    if (headers_.empty())
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable |
      //ImGuiTableFlags_Sortable | ImGuiTableFlags_Hideable |
      ImGuiTableFlags_Borders | ImGuiTableFlags_Scroll | ImGuiTableFlags_ScrollFreezeTopRow | ImGuiTableFlags_ScrollFreezeLeftColumn;

    if (ImGui::BeginTable("Table", headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < headers_.size(); col++) {
        ImGui::TableSetupColumn(headers_.at(col).c_str());
      }
      ImGui::TableAutoHeaders();

      // Second row to end body
      //static size_t selected = -1;
      if (columns_.size() > 0) {
        for (size_t row = 0; row < columns_[0].size(); ++row) {
          bool pass_all_columns = checked_rows_[row];
          if (pass_all_columns) {
            ImGui::TableNextRow();

            //// TODO: Bug in row highlighting
            //char label[32];
            //sprintf(label, "%lu", row);
            //if (ImGui::Selectable(label, selected == row, ImGuiSelectableFlags_SpanAllColumns))
            //  selected = row;
            //// bool hovered = ImGui::IsItemHovered();
            for (size_t col = 0; col < headers_.size(); ++col) {
              ImGui::TableSetColumnIndex(col);
              ImGui::Text("%s", columns_[col][row].c_str());

              //// TODO: Testing random fixes to get input text to work
              //char buf[512];
              //sprintf(buf, columns_[col][row].c_str());
              //ImGui::InputText("", buf, IM_ARRAYSIZE(buf));
              //if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
              //  ImGui::SetMouseCursor(1);
            }
          }
        }
      }
      ImGui::EndTable();
    }
  }

  void ExplorerWidget::draw()
  {
    if (headers_.empty())
      return;

    // headers
    const ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable |
      ImGuiTableFlags_Scroll | ImGuiTableFlags_ScrollFreezeTopRow;

    if (ImGui::BeginTable("Explorer", headers_.size(), table_flags)) {
      // First row headers
      for (int col = 0; col < headers_.size(); col++) {
        ImGui::TableSetupColumn(headers_.at(col).c_str());
      }
      ImGui::TableAutoHeaders();

      // Second row to end body
      //static size_t selected = -1;
      if (columns_.size() > 0) {
        for (size_t row = 0; row < columns_[0].size(); ++row) {
          bool pass_all_columns = checked_rows_;
          if (pass_all_columns) {
            ImGui::TableNextRow();

            //// TODO: Bug in row highlighting
            //char label[32];
            //sprintf(label, "%lu", row);
            //if (ImGui::Selectable(label, selected == row, ImGuiSelectableFlags_SpanAllColumns))
            //  selected = row;
            //// bool hovered = ImGui::IsItemHovered();
            for (size_t col = 0; col < headers_.size(); ++col) {
              if (col == headers_.size() - 2) {
                ImGui::TableSetColumnIndex(col);
                ImGui::Checkbox("", &checked_rows_1_[row]);
              }
              else if (col == headers_.size() - 1) {
                ImGui::TableSetColumnIndex(col);
                ImGui::Checkbox("", &checked_rows_2_[row]);
              }
              else {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("%s", columns_[col][row].c_str());
              }
            }
          }
        }
      }
      ImGui::EndTable();
    }
  }

  void GenericGraphicWidget::draw()
  {
    // Dummy data
    std::vector<std::string> headers = { "sample_name", "sample_type", "component_name" }; // feature or sample columns to filter on
    std::vector<std::string> sample_name_col = {"S1", "S1", "S2", "S2", "S3", "S3", "S4", "S4"};
    std::vector<std::string> sample_type_col = { "A", "A", "A", "A", "B", "B", "B" ,"B"};
    std::vector<std::string> component_name_col = { "C1", "C2", "C1", "C2", "C1", "C2", "C1", "C2" };
    std::vector<std::vector<std::string>> columns = { sample_name_col, sample_type_col, component_name_col };
    bool checked_rows[] = { true, true, true, true, true, true, true, true };

    // row filters
    static std::vector<ImGuiTextFilter> filter;
    std::vector<std::vector<std::pair<std::string, std::vector<size_t>>>> columns_indices;
    this->makeFilters(headers, columns, columns_indices, filter);

    // Search and filtering options for the plot
    for (size_t col = 0; col < headers.size(); ++col)
    {
      char s[128];
      if (ImGui::Button(headers[col].c_str())) {
        sprintf(s, "%lu", col);
        ImGui::OpenPopup(s);
      }
      sprintf(s, "%lu", col);
      this->FilterPopup(s, filter[col], columns[col], checked_rows, columns_indices[col]);
      ImGui::SameLine();
    }
    ImGui::NewLine();
    ImGui::TextWrapped("TODO: scatter plots for each feature (red, selected; green, not selected)");
  }

  void LinePlot2DWidget::draw()
  {

    // Main graphic
    if (ImPlot::BeginPlot("Line Plot", x_axis_title_.c_str(), y_axis_title_.c_str())) {
      for (int i = 0; i < x_data_.size(); ++i) {
        assert(x_data_.at(i).size() == y_data_.at(i).size());
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, ImPlot::GetStyle().LineWeight);
        ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
        ImPlot::PlotLine(series_names_.at(i).c_str(), x_data_.at(i).data(), y_data_.at(i).data(), x_data_.at(i).size());
      }
      ImPlot::EndPlot();
    }
  }

  void GenericTreeWidget::draw()
  {
    // left

    // TODO: search by sample name (or sample_group or sequence_segment)
    // TODO: switch between samples, sample_groups, and sequence_segments
    // TODO: check for samples included in the "current" analysis
    // TODO: tooltip with status information for each sample
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (int i = 0; i < 100; ++i)
    {
      // TODO: change to button with color codes for the following
      // - pass = green
      // - warn = yellow
      // - fail = red
      // add progress bar to indicate completed
      char label[128];
      sprintf(label, "MyObject %d", i); // TODO: update for use with sequence list
      if (ImGui::Selectable(label, selected == i))
        selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // right

    // TODO add ability to search and filter by
    // - sample_name
    // - sequence_group_name
    // - sample_group_name
    // - sample_type
    // - sample_status (i.e., pass, warn, fail)
    // - completion status
    // TODO change row column to/from sample, sample_group, sequence_group depending on selected in left panel
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 line below us
    ImGui::Text("MyObject: %d", selected); //TODO: update for use with sequence list
    ImGui::Separator();

    // TODO: anlaysis output
    ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");

    // TODO: add "tag" dropdown
    // TODO: row of buttons for sequence_group

    // TODO: add "tag" dropdown
    // TODO: row of buttons for sample_group

    // TODO: add "tag" dropdown
    // TODO: drop down for sample type

    // NOTE: each button should have an "x" that deletes the tag
    ImGui::EndChild();
    ImGui::EndGroup();

    // TODO: add selected samples to plot widget
    // TODO: add selected samples to workflow widget
  }

  void WorkflowWidget::draw()
  {
    // Top
    ImGui::BeginGroup();
    ImGui::Text("Add processing step:");
    ImGui::SameLine();
    const char* sample_ps[] = { "load_mzML" }; // TODO
    if (ImGui::Button("Sample"))
      ImGui::OpenPopup("Sample");
    if (ImGui::BeginPopup("Sample"))
    {
      static ImGuiTextFilter sample_filter;
      sample_filter.Draw();
      for (int i = 0; i < IM_ARRAYSIZE(sample_ps); ++i)
      {
        if (sample_filter.PassFilter(sample_ps[i]))
        {
          ImGui::Text("%s", sample_ps[i]);
          if (ImGui::IsItemClicked())
          {
            //TODO:  add table row
          }
        }
      }
      ImGui::EndPopup();
    }
    ImGui::SameLine();
    const char* sequence_segment_ps[] = { "calibrate" }; // TODO
    if (ImGui::Button("Sequence Segment"))
      ImGui::OpenPopup("Sequence Segment");
    if (ImGui::BeginPopup("Sequence Segment"))
    {
      static ImGuiTextFilter sequence_segment_filter;
      sequence_segment_filter.Draw();
      for (int i = 0; i < IM_ARRAYSIZE(sequence_segment_ps); ++i)
      {
        if (sequence_segment_filter.PassFilter(sequence_segment_ps[i]))
        {
          ImGui::Text("%s", sequence_segment_ps[i]);
          if (ImGui::IsItemClicked())
          {
            //TODO:  add table row
          }
        }
      }
      ImGui::EndPopup();
    }
    const char* sample_group_ps[] = { "normalize" }; // TODO
    ImGui::SameLine();
    if (ImGui::Button("Sample Group"))
      ImGui::OpenPopup("Sample Group");
    if (ImGui::BeginPopup("Sample Group"))
    {
      static ImGuiTextFilter sample_group_filter;
      sample_group_filter.Draw();
      for (int i = 0; i < IM_ARRAYSIZE(sample_group_ps); ++i)
      {
        if (sample_group_filter.PassFilter(sample_group_ps[i]))
        {
          ImGui::Text("%s", sample_group_ps[i]);
          if (ImGui::IsItemClicked())
          {
            //TODO:  add table row
          }
        }
      }
      ImGui::EndPopup();
    }
    ImGui::EndGroup();

    // Middle:
    // Table Filters
    static std::vector<ImGuiTextFilter> filter;
    static ImGuiTextFilter filter0, filter1, filter2, filter3;
    filter.push_back(filter0); filter.push_back(filter1);
    filter.push_back(filter2); filter.push_back(filter3);

    // Table Data
    const std::vector<std::string> column0 = { "PS1", "PS2", "PS3" };
    const std::vector<std::string> column1 = { "Sample", "SequenceSegment", "Sample" };
    static bool checked_rows[] = { true, true, true };
    static bool rows_enabled[] = { true, true, true };
    std::vector<std::vector<std::string>> columns;
    columns.push_back(column0);
    columns.push_back(column1);

    // Header
    ImGui::BeginChild("Workflow table", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
    const char* headers[] = { "Processing Step", "Processing Type" };
    ImGui::Columns(IM_ARRAYSIZE(headers) + 2, "mycolumns", true);
    ImGui::Text("#");
    ImGui::NextColumn();
    for (int col = 0; col < IM_ARRAYSIZE(headers); ++col)
    {
      if (ImGui::Button(headers[col]))
        ImGui::OpenPopup(headers[col]);
      //FilterPopup(headers[col], filter[col], columns[col], checked_rows);
      ImGui::NextColumn();
    }
    ImGui::Text("Enabled");
    ImGui::NextColumn();

    // Body
    // TODO: 3 columns with order, process step, checkmark (enabled, disabled)
    ImGui::Separator();
    static size_t selected = -1;
    for (size_t i = 0; i < columns[0].size(); ++i)
    {
      if (checked_rows[i])
      {
        char label[32];
        sprintf(label, "%lu", i);
        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
          selected = i;
        // bool hovered = ImGui::IsItemHovered();
        ImGui::NextColumn();
        for (int j = 0; j < IM_ARRAYSIZE(headers); ++j)
        {
          ImGui::Text("%s", columns[j][i].c_str());
          // TODO: tooltip description of each processing step
          ImGui::NextColumn();
        }
        ImGui::Checkbox("", &rows_enabled[i]);
        ImGui::NextColumn();
      }
    }
    ImGui::Dummy(ImVec2(0, 0));
    ImGui::EndChild();

    // Bottom:
    ImGui::BeginGroup();
    if (ImGui::Button("Run/Resume"))
    {
      // TODO execute the workflow
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
      // TODO pause the workflow
    }
    ImGui::SameLine();
    if (ImGui::Button("Restart"))
    {
      // TODO restart the workflow
    }
    ImGui::EndGroup();
  }

  void GenericTextWidget::draw()
  {
    for (const std::string& line : text_lines) {
      ImGui::TextWrapped("%s", line.c_str());
    }
  }
}