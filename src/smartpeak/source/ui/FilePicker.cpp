#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/core/Utilities.h>
#include <future>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void FilePicker::draw()
  {
    if (!ImGui::BeginPopupModal("Pick a pathname", NULL, ImGuiWindowFlags_NoResize)) {
      return;
    }

    static int selected_entry = -1;

    if (ImGui::Button("Up"))
    {
      const std::string parent = Utilities::getParentPathname(current_pathname_);
      if (parent.size()) {
        current_pathname_ = parent;
      }
      pathname_content_ = Utilities::getPathnameContent(current_pathname_);
      selected_entry = -1;
    }
    ImGui::SameLine();
    ImGui::Text("Path: %s", current_pathname_.c_str());

    static char new_pathname[4096];
    if (ImGui::Button("Change dir"))
    {
      ImGui::OpenPopup("Change directory");
      std::strncpy(new_pathname, current_pathname_.c_str(), 4096);
    }

    if (ImGui::BeginPopupModal("Change directory", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::InputText("Pathname", new_pathname, 4096);
      if (ImGui::Button("Set") || ImGui::IsKeyPressedMap(ImGuiKey_Enter))
      {
        current_pathname_.assign(new_pathname);
        pathname_content_ = Utilities::getPathnameContent(current_pathname_);
        selected_entry = -1;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    static ImGuiTextFilter filter;
    filter.Draw("Filter filename (inc,-exc)");

    // File type filter
    static int selected_extension = 0; // If the selection isn't within 0..count, Combo won't display a preview
    static const char* extensions[] = { "All", "csv", "featureXML", "mzML" };
    ImGui::Combo("File type", &selected_extension, extensions, IM_ARRAYSIZE(extensions));

    static char selected_filename[256] = {0};
    // Folder content / Navigation
    ImGui::BeginChild("Content", ImVec2(1024, 400));
    
    const int column_count = 4;
    const char* column_names[column_count] = { "Name", "Size", "Type", "Date Modified" };
    static ImGuiTableColumnFlags column_flags[column_count] = { ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_DefaultHide,
      ImGuiTableColumnFlags_DefaultSort
    };
    
    static ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable;
        
    ImVec2 size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 2);
    if (ImGui::BeginTable("Content", column_count, table_flags, size))
    {
      for (int column = 0; column < column_count; column++){
        ImGui::TableSetupColumn(column_names[column], column_flags[column]);
      }
      ImGui::TableHeadersRow();

      const ImGuiWindowFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
      for (int row = 0; row < pathname_content_[0].size(); row++)
      {
        if (!filter.PassFilter(pathname_content_[0][row].c_str()))
        {
          continue; // continue if it does not pass the filter
        }
        
        if (selected_extension > 0 && !Utilities::endsWith(pathname_content_[0][row], "." + std::string(extensions[selected_extension]), false))
        {
          continue; // continue if the file type is not desired
        }
                
        ImGui::TableNextRow();
        for (int column = 0; column < column_count; column++)
        {
          char buf[256];
          sprintf(buf, "%s", pathname_content_[column][row].c_str());
          const bool is_selected = (selected_entry == row);
          ImGui::TableSetColumnIndex(column);
          if(ImGui::Selectable(buf, is_selected, selectable_flags))
          {
            selected_entry = row;
            if (ImGui::IsMouseDoubleClicked(0) && pathname_content_[2][selected_entry] == "Directory") // TODO: error prone to rely on strings
            {
              if (current_pathname_.back() != '/') // do not insert "/" if current_pathname_ == root dir, i.e. avoid "//home"
              {
                current_pathname_.append("/");
              }
              current_pathname_.append(pathname_content_[0][selected_entry]);
              pathname_content_ = Utilities::getPathnameContent(current_pathname_);
              filter.Clear();
              selected_entry = -1;
              selected_filename[0] = '\0';
              break; // IMPORTANT: because the following lines in the loop assume accessing old/previous pathname_content_'s data
            }
          }
        }
      }
      ImGui::EndTable();
    }
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::InputTextWithHint("", "File name", selected_filename, IM_ARRAYSIZE(selected_filename));
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    if (ImGui::Button("Open"))
    {
      picked_pathname_ = current_pathname_;
      if (selected_entry >= 0)
      {
        if (picked_pathname_.back() != '/') // do not insert "/" if current_pathname_ == root dir, i.e. avoid "//home"
        {
          picked_pathname_.append("/");
        }
        picked_pathname_.append(pathname_content_[0][selected_entry]);
      }
      LOGI << "Picked pathname: " << picked_pathname_;
      runProcessor();
      clearProcessor();
      selected_entry = -1;
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    if (ImGui::Button("Cancel"))
    {
      picked_pathname_.clear();
      selected_entry = -1;
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopItemWidth();

    ImGui::EndPopup();
  }

  std::string FilePicker::getPickedPathname() const
  {
    return picked_pathname_;
  }

  void FilePicker::setProcessor(FilePickerProcessor& processor)
  {
    LOGD << "Setting processor: " << (&processor);
    processor_ = &processor;
    processor_name_ = processor.getName();
    error_loading_file_ = false;
    file_was_loaded_ = false;
  }

  void FilePicker::runProcessor()
  {
    LOGD << "Running processor (ptr): " << processor_;
    if (!processor_)
      return;
    loading_is_done_ = false;
    run_and_join(processor_, picked_pathname_, loading_is_done_, file_was_loaded_);
  }

  void FilePicker::clearProcessor()
  {
    LOGD << "Clearing processor";
    processor_ = nullptr;
  }

  void FilePicker::run_and_join(
    FilePickerProcessor* processor,
    const std::string& pathname,
    bool& loading_is_done,
    bool& file_was_loaded
  )
  {
    processor->pathname_ = pathname;
    std::future<bool> f = std::async(
      std::launch::async, 
      [processor](){ return processor->process(); }
    );

    LOGN << "File is being loaded...";
    f.wait();

    try {
      file_was_loaded = f.get();
      if (file_was_loaded) {
        LOGN << "File has been loaded.";
      }
      else {
        error_loading_file_ = true;
        LOGN << "File has not been loaded.";
      }
    } catch (const std::exception& e) {
      error_loading_file_ = true;
      LOGE << e.what();
    }

    loading_is_done = true;    
  }
}
