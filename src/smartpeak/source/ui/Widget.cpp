#include <SmartPeak/ui/Widget.h>

namespace SmartPeak
{
  void Widget::tableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked) {

    if (ImGui::BeginPopup(popuop_id))
    {
      // Sorting
      if (ImGui::Button("Asc"))
      {
        //TODO filter ascending
      }
      ImGui::SameLine();
      if (ImGui::Button("Desc"))
      {
        //TODO filter descending
      }

      // Filtering and selecting
      ImGui::Separator();
      filter.Draw();
      // static bool check_all = true;
      if (ImGui::Button("check all"))
        for (int i = 0; i < column.size(); ++i) checked[i] = true;
      ImGui::SameLine();
      if (ImGui::Button("uncheck all"))
        for (int i = 0; i < column.size(); ++i) checked[i] = false;
      for (int i = 0; i < column.size(); ++i)
        if (filter.PassFilter(column[i].c_str()))
          ImGui::Checkbox(column[i].c_str(), &checked[i]);

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
  }
  void Widget::tableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked, std::map<std::string, std::vector<int>>& values_indices) {

    if (ImGui::BeginPopup(popuop_id))
    {
      // Sorting
      if (ImGui::Button("Asc"))
      {
        //TODO filter ascending
      }
      ImGui::SameLine();
      if (ImGui::Button("Desc"))
      {
        //TODO filter descending
      }

      // Filtering and selecting
      ImGui::Separator();
      filter.Draw();
      
      if (ImGui::Button("check all"))
        for (const auto& indexes: values_indices)
          for (const int& i: indexes.second) 
            checked[i] = true;
      ImGui::SameLine();
      if (ImGui::Button("uncheck all"))
        for (const auto& indexes : values_indices)
          for (const int& i : indexes.second)
            checked[i] = false;
      for (const auto& indexes : values_indices)
        if (filter.PassFilter(indexes.first.c_str()))
          ImGui::Checkbox(indexes.first.c_str(), &checked[indexes.second.front()]);

      // Update checked rows not explicitly shown
      for (const auto& indexes : values_indices)
        for (const int& i : indexes.second)
          checked[i] = checked[indexes.second.front()];

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
  }

  void GenericTableWidget::show(const std::vector<std::string>& headers,
    std::vector<std::vector<std::string>>& columns,
    bool* checked_rows)
  {
    // ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetFontSize() * 20), false, ImGuiWindowFlags_HorizontalScrollbar);
    //// row filters
    //static std::vector<ImGuiTextFilter> filter;
    //for (int col = 0; col < headers.size(); ++col)
    //{
    //  static ImGuiTextFilter filter0;
    //  filter.push_back(filter0);
    //}

    // row filters
    static std::vector<ImGuiTextFilter> filter;
    std::vector<std::map<std::string, std::vector<int>>> columns_indices;
    for (int col = 0; col < headers.size(); ++col)
    {
      // Extract out unique columns and replicate indices
      std::map<std::string, std::vector<int>> value_indices;
      for (int row = 0; row < columns[col].size(); ++row) {
        std::vector<int> index = { row };
        auto found = value_indices.emplace(columns[col][row], index);
        if (!found.second) {
          value_indices.at(columns[col][row]).push_back(row);
        }
      }
      columns_indices.push_back(value_indices);

      // Initialize the filters
      static ImGuiTextFilter filter0;
      filter.push_back(filter0);
    }

    // headers
    ImGui::Columns(headers.size() + 1, "mycolumns", true);
    ImGui::Separator();
    ImGui::Text("Index");
    ImGui::NextColumn();
    for (int col = 0; col < headers.size(); ++col)
    {
      if (ImGui::Button(headers[col].c_str()))
        ImGui::OpenPopup(&static_cast<char>(col));
      //tableFilterPopup(&static_cast<char>(col), filter[col], columns[col], checked_rows);
      tableFilterPopup(&static_cast<char>(col), filter[col], columns[col], checked_rows, columns_indices[col]);
      ImGui::NextColumn();
    }

    // body
    ImGui::Separator();
    static int selected = -1;
    for (int i = 0; i < columns[0].size(); ++i)
    {
      // NOTE: does not appear to be needed
      // check if all columns for the curren row pass the filter
      // bool pass_all_columns = true;
      // for (int j = 0; j < headers.size(); ++j)
      //   if (!filter[j].PassFilter(columns[j][i].c_str()))
      //     pass_all_columns = false;

      bool pass_all_columns = checked_rows[i];
      if (pass_all_columns)
      {
        char label[32];
        sprintf(label, "%d", i);
        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
          selected = i;
        bool hovered = ImGui::IsItemHovered();
        ImGui::NextColumn();
        for (int j = 0; j < headers.size(); ++j)
        {
          ImGui::Text(columns[j][i].c_str());
          ImGui::NextColumn();
        }
      }
    }
    // ImGui::EndChild();
  }

  void GenericTableWidget::makeCheckedRows(const int & n_rows, bool * checked_rows)
  {
    for (size_t i = 0; i < n_rows; ++i)
      checked_rows[i] = true;
  }

  void GenericGraphicWidget::show()
  {
    // Dummy data
    std::vector<std::string> headers = { "sample_name", "sample_type", "component_name" }; // feature or sample columns to filter on
    std::vector<std::string> sample_name_col = {"S1", "S1", "S2", "S2", "S3", "S3", "S4", "S4"};
    std::vector<std::string> sample_type_col = { "A", "A", "A", "A", "B", "B", "B" ,"B"};
    std::vector<std::string> component_name_col = { "C1", "C2", "C1", "C2", "C1", "C2", "C1", "C2" };
    std::vector<std::vector<std::string>> columns = { sample_name_col, sample_type_col, component_name_col };
    static bool checked_rows[] = { true, true, true, true, true, true, true, true };

    // row filters
    static std::vector<ImGuiTextFilter> filter;
    std::vector<std::map<std::string, std::vector<int>>> columns_indices;
    for (int col = 0; col < headers.size(); ++col)
    {
      // Extract out unique columns and replicate indices
      std::map<std::string, std::vector<int>> value_indices;
      for (int row = 0; row < columns[col].size(); ++row) {
        std::vector<int> index = { row };
        auto found = value_indices.emplace(columns[col][row], index);
        if (!found.second) {
          value_indices.at(columns[col][row]).push_back(row);
        }
      }
      columns_indices.push_back(value_indices);

      // Initialize the filters
      static ImGuiTextFilter filter0;
      filter.push_back(filter0);
    }

    // Search and filtering options for the plot
    for (int col = 0; col < headers.size(); ++col)
    {
      if (ImGui::Button(headers[col].c_str()))
        ImGui::OpenPopup(&static_cast<char>(col));
      tableFilterPopup(&static_cast<char>(col), filter[col], columns[col], checked_rows, columns_indices[col]);
      ImGui::SameLine();
    }
    ImGui::NewLine();

    //// left: filters
    //ImGui::BeginChild("Filters", ImVec2(200, -ImGui::GetFrameHeightWithSpacing()), true);
    //for (int j = 0; j < IM_ARRAYSIZE(headers); ++j)
    //{
    //  if (ImGui::Button(headers[j]))
    //  {
    //    ImGui::OpenPopup(headers[j]);
    //  }
    //  if (ImGui::BeginPopup(headers[j]))
    //  {
    //    filter[j].Draw();
    //    if (ImGui::Button("check all"))
    //      for (int i = 0; i < IM_ARRAYSIZE(columns); ++i)
    //        columns_checked[i][j] = true;
    //    ImGui::SameLine();
    //    if (ImGui::Button("uncheck all"))
    //      for (int i = 0; i < IM_ARRAYSIZE(columns); ++i)
    //        columns_checked[i][j] = false;
    //    for (int i = 0; i < IM_ARRAYSIZE(columns); ++i)
    //    {
    //      bool pass_other_filters = true;
    //      for (int k = 0; k < IM_ARRAYSIZE(headers); ++k)
    //        if (!columns_checked[i][k])
    //          bool pass_other_filters = false;
    //      if (filter[j].PassFilter(columns[i][j]) && pass_other_filters)
    //        ImGui::Checkbox(columns[i][j], &columns_checked[i][j]);
    //    }
    //    ImGui::EndPopup();
    //  }
    //  ImGui::Separator();
    //}
    //ImGui::EndChild();

    // Main graphic
    ImGui::TextWrapped("TODO: add plot");
  }

  void GenericTreeWidget::show()
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

  void FileBrowserWidget::show()
  {
    typedef char element[256];

    // starting variables
    static char buffilename[256] = "";
    static char bufdirname[256] = "path/to/current/dir";  // TODO: update with current user directory

    // top: up level, current directory string, refresh icon, search
    // ImGui::BeginChild("Top");
    if (ImGui::Button("Up"))
    {
      // TODO: move up one directory
      // TODO: update the list of
    }
    ImGui::SameLine();
    ImGui::InputText("", bufdirname, 256);
    ImGui::SameLine();
    if (ImGui::Button("Refresh"))
    {
      // Refresh right panel and list of files
    }
    ImGui::SameLine();
    static ImGuiTextFilter filter;
    filter.Draw();
    // ImGui::EndChild();
    ImGui::Separator();

    // left: current directory folders (recursive tree)
    // TODO: develop recursive directory logic
    static int selected_dir = 0;
    ImGui::BeginChild("Directories", ImVec2(120, -ImGui::GetFrameHeightWithSpacing()), true);
    element current_dirs[] = { "dir 1", "dir 2", "dir 3", "dir 4" };
    static int n_current_dirs = 4;
    for (int i = 0; i < n_current_dirs; ++i)
    {
      char label[64];
      sprintf(label, "dir %d", i);
      if (ImGui::Selectable(label, selected_dir == i))
        selected_dir = i;
      if (ImGui::IsItemClicked())
      {
        // TODO retrieve the path of the system folder...
        sprintf(bufdirname, "%s", current_dirs[i]);  // update the directory name
        // TODO update the table based on what is in the current directory
      }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // right: files/folders in highlighted directory
    ImGui::BeginChild("Current Directory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
    // headers
    const char* headers[] = { "Name", "Date Modified", "Type", "Size" };
    ImGui::Columns(IM_ARRAYSIZE(headers) + 1, "mycolumns", true); // with border
    ImGui::Separator();
    ImGui::Text("index");
    ImGui::NextColumn();
    for (int col = 0; col < IM_ARRAYSIZE(headers); ++col)
    {
      ImGui::Text(headers[col]);
      ImGui::NextColumn();
    }
    // Body
    ImGui::Separator();
    // // typedef (todo: change to std::vector/map)
    typedef element row[4];
    row columns[] = { // NOTE: this table can be all char or std::string
      {"file1", "04/15/2018 2:00 PM", ".csv", "1MB"},
      {"file2", "04/15/2018 2:00 PM", ".txt", "2MB"},
      {"file3", "04/15/2018 2:00 PM", ".mzML", "3MB"},
    };
    static int selected_file = -1;
    for (int i = 0; i < IM_ARRAYSIZE(columns); ++i)
    {
      if (filter.PassFilter(columns[i][0]))
      {
        char label[32];
        sprintf(label, "%d", i);
        if (ImGui::Selectable(label, selected_file == i, ImGuiSelectableFlags_SpanAllColumns))
          selected_file = i;
        bool hovered = ImGui::IsItemHovered();
        if (ImGui::IsItemClicked()) // update the selected file
          sprintf(buffilename, "%s", columns[i][0]);
        ImGui::NextColumn();
        for (int j = 0; j < IM_ARRAYSIZE(headers); ++j)
        {
          ImGui::Text(columns[i][j]);
          ImGui::NextColumn();
        }
      }
    }
    ImGui::EndChild();

    // bottom: selected filename, filetypes, open/cancel
    ImGui::BeginGroup();
    // Filtering and selecting
    ImGui::Separator();
    ImGui::Text("File Name:");
    // 
    ImGui::SameLine();
    ImGui::InputText("", buffilename, 256);
    ImGui::SameLine();
    const char* file_types[] = { ".csv", ".txt", ".mzML", "." };
    if (ImGui::BeginMenu("File Type"))
    {
      for (int i = 0; i < IM_ARRAYSIZE(file_types); ++i)
      {
        ImGui::MenuItem(file_types[i]);
      }
      // ImGui::SameLine(); //TODO: display selected filetype
      // ImGui::Text(file_types[i]);
      ImGui::EndMenu();
    }
    if (ImGui::Button("Accept"))
    {
      // TODO read in the file
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      // TODO close the modal
    }
    ImGui::EndGroup();
  }

  void WorkflowWidget::show()
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
          ImGui::Text(sample_ps[i]);
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
          ImGui::Text(sequence_segment_ps[i]);
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
          ImGui::Text(sample_group_ps[i]);
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
      tableFilterPopup(headers[col], filter[col], columns[col], checked_rows);
      ImGui::NextColumn();
    }
    ImGui::Text("Enabled");
    ImGui::NextColumn();

    // Body
    // TODO: 3 columns with order, process step, checkmark (enabled, disabled)
    ImGui::Separator();
    static int selected = -1;
    for (int i = 0; i < columns[0].size(); ++i)
    {
      if (checked_rows[i])
      {
        char label[32];
        sprintf(label, "%d", i);
        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
          selected = i;
        bool hovered = ImGui::IsItemHovered();
        ImGui::NextColumn();
        for (int j = 0; j < IM_ARRAYSIZE(headers); ++j)
        {
          ImGui::Text(columns[j][i].c_str());
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

  void GenericTextWidget::show(const std::vector<std::string>& text_lines)
  {
    for (const std::string& line : text_lines) {
      ImGui::Text(line.data());
    }
  }
}