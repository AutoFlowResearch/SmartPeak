#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/core/Utilities.h>
#include <future>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace SmartPeak
{
  enum ContentColumnID
  {
    ContentColumnID_ID,
    ContentColumnID_Name,
    ContentColumnID_Size,
    ContentColumnID_Type,
    ContentColumnID_DateModified
  };

  struct ImDirectoryEntry
  {
    int         ID;
    const char* Name;
    const char* Size;
    const char* Type;
    const char* DateModified;
    
    static const ImGuiTableSortSpecs* s_current_sort_specs;

    static int IMGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
      const ImDirectoryEntry* a = (const ImDirectoryEntry*)lhs;
      const ImDirectoryEntry* b = (const ImDirectoryEntry*)rhs;
      for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
      {
        const ImGuiTableSortSpecsColumn* sort_spec = &s_current_sort_specs->Specs[n];
        int delta = 0;
        switch (sort_spec->ColumnUserID)
        {
          case ContentColumnID_ID:
            delta = (a->ID - b->ID);
            break;
            
          case ContentColumnID_Name:
            {
              std::string a_str, b_str;
              a_str = a->Name;
              b_str = b->Name;
              std::string a_str_lowercased(a_str.length(),' ');
              std::string b_str_lowercased(b_str.length(),' ');
              std::transform(a_str.begin(), a_str.end(), a_str_lowercased.begin(), tolower);
              std::transform(b_str.begin(), b_str.end(), b_str_lowercased.begin(), tolower);

              if  (!a_str_lowercased.empty() && !b_str_lowercased.empty() && a_str_lowercased.front()==b_str_lowercased.front()) {
                delta = a_str.compare(b_str);
              }
              else
              {
                // case-insensitive lexicographic sort
                delta = a_str_lowercased.compare(b_str_lowercased);
              }
            }
            break;
            
          case ContentColumnID_Size:
            delta = ((std::stol(a->Size) - std::stol(b->Size)));
            break;
            
          case ContentColumnID_Type:
            delta = (strcmp(a->Type, b->Type));
            break;
            
          case ContentColumnID_DateModified:
            delta = (strcmp(a->DateModified, b->DateModified));
            break;
            
          default: IM_ASSERT(0); break;
        }
        if (delta > 0)
          return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
        if (delta < 0)
          return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
      }
      return (strcmp(a->Name, b->Name));
    }
  };

  const ImGuiTableSortSpecs* ImDirectoryEntry::s_current_sort_specs = NULL;

  void FilePicker::update_contents(ImVector<ImDirectoryEntry>& Im_directory_entries)
  {
    if (!files_scanned)
    {
      pathname_content_ = Utilities::getPathnameContent(current_pathname_);
      Im_directory_entries.resize(pathname_content_[0].size());
      for (int row = 0; row < pathname_content_[0].size(); row++)
      {
        ImDirectoryEntry& ImDirectoryEntry  = Im_directory_entries[row];
        ImDirectoryEntry.Name               = pathname_content_[0][row].c_str();
        ImDirectoryEntry.Size               = pathname_content_[1][row].c_str();
        ImDirectoryEntry.Type               = pathname_content_[2][row].c_str();
        ImDirectoryEntry.DateModified       = pathname_content_[3][row].c_str();
      }
      files_scanned = true;
    }
  }

  void FilePicker::draw()
  {
    if (!ImGui::BeginPopupModal("Pick a pathname", NULL, ImGuiWindowFlags_NoResize)) {
      return;
    }

    static int selected_entry = -1;
    static char selected_filename[256] = {0};

    if (ImGui::Button("Up"))
    {
      const std::string parent = Utilities::getParentPathname(current_pathname_);
      if (parent.size()) {
        current_pathname_ = parent;
      }
      pathname_content_ = Utilities::getPathnameContent(current_pathname_);
      files_scanned = false;
      memset(selected_filename, 0, sizeof selected_filename);
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
        memset(selected_filename, 0, sizeof selected_filename);
        files_scanned = false;
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

    ImGui::BeginChild("ImDirectoryEntry", ImVec2(1024, 400));
    
    const int column_count = 4;
    const char* column_names[column_count] = { "Name", "Size [bytes]", "Type", "Date Modified" };
    static ImGuiTableColumnFlags column_flags[column_count] = { ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide, ImGuiTableColumnFlags_NoHide, ImGuiTableColumnFlags_NoHide, ImGuiTableColumnFlags_NoHide
    };
    static ContentColumnID content_column_ID[column_count] = { ContentColumnID_Name, ContentColumnID_Size, ContentColumnID_Type, ContentColumnID_DateModified };
    
    static ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Sortable | ImGuiTableFlags_MultiSortable | ImGuiTableFlags_NoBordersInBody;
    
    static ImVector<ImDirectoryEntry> Im_directory_entries;
    static ImVector<int> selection;
    update_contents(Im_directory_entries);
    
    ImVec2 size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 2);
    if (ImGui::BeginTable("ImDirectoryEntry", column_count, table_flags, size))
    {
      for (int column = 0; column < column_count; column++){
        ImGui::TableSetupColumn(column_names[column], column_flags[column], -1.0f, content_column_ID[column]);
      }
      ImGui::TableSetupScrollFreeze(column_count, 1);
      ImGui::TableHeadersRow();
      
      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        if (sorts_specs->SpecsDirty)
        {
          ImDirectoryEntry::s_current_sort_specs = sorts_specs;
          if (Im_directory_entries.Size > 1)
              qsort(&Im_directory_entries[0], (size_t)Im_directory_entries.Size, sizeof(Im_directory_entries[0]), ImDirectoryEntry::CompareWithSortSpecs);
          ImDirectoryEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
        }
      }

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
          ImDirectoryEntry* item = &Im_directory_entries[row];
          char text_buffer[256];
          if (column == 0)
          {
            sprintf(text_buffer, "%s", item->Name);
          }
          else if (column == 1)
          {
            sprintf(text_buffer, "%s", item->Size);
          }
          else if (column == 2)
          {
            sprintf(text_buffer, "%s", item->Type);
          }
          else if (column == 3)
          {
            sprintf(text_buffer, "%s", item->DateModified);
          }
          
          char buf[256];
          const bool is_selected = (selected_entry == row);
          ImGui::TableSetColumnIndex(column);
          if(ImGui::Selectable(text_buffer, is_selected, selectable_flags))
          {
            selected_entry = row;
            std::strcpy(selected_filename, Im_directory_entries[selected_entry].Name);
            if (ImGui::IsMouseDoubleClicked(0) && !std::strcmp(item->Type , "Directory") )
            {
              if (current_pathname_.back() != '/') // do not insert "/" if current_pathname_ == root dir, i.e. avoid "//home"
              {
                current_pathname_.append("/");
              }
              
              current_pathname_.append(item->Name);
              memset(selected_filename, 0, sizeof selected_filename);
              files_scanned = false;
              update_contents(Im_directory_entries);
              filter.Clear();
              selected_entry = -1;
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
    if (ImGui::IsMouseClicked(0))
      ImGui::InputTextWithHint("", "File name", selected_filename, IM_ARRAYSIZE(selected_filename));
    else
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
