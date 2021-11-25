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
// $Authors: Douglas McCloskey, Ahmed Khalil, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/core/Utilities.h>
#include <future>
#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void FilePicker::updateContents(std::vector<ImEntry>& Im_directory_entries)
  {
    if (!files_scanned_)
    {
      pathname_content_ = Utilities::getFolderContents(current_pathname_, (mode_ == Mode::EDirectory));
      Im_directory_entries.resize(pathname_content_[0].size(), ImEntry());
      for (int row = 0; row < pathname_content_[0].size(); row++)
      {
        Im_directory_entries[row].entry_contents.resize(4, "");
        Im_directory_entries[row].ID = row;
        for (int col = 0; col < 4; col++)
        {
          Im_directory_entries[row].entry_contents[col] = pathname_content_[col][row].c_str();
        }
      }
      files_scanned_ = true;
    }
  }

  void FilePicker::draw()
  {
    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_NoResize)) {
      return;
    }

    if (ImGui::Button("Up"))
    {
      const std::filesystem::path parent = Utilities::getParentPath(current_pathname_);
      if (parent.string().size()) {
        current_pathname_ = parent;
      }
      pathname_content_ = Utilities::getFolderContents(current_pathname_, (mode_ == Mode::EDirectory));
      files_scanned_ = false;
      if (mode_ != Mode::EFileCreate)
      {
        selected_filename_.clear();
      }
      selected_entry_ = -1;
    }
    ImGui::SameLine();
    ImGui::Text("Path: %s", current_pathname_.string().c_str());

    static char new_pathname[4096];
    if (ImGui::Button("Change dir"))
    {
      ImGui::OpenPopup("Change directory");
      std::strncpy(new_pathname, current_pathname_.string().c_str(), 4096);
    }

    if (ImGui::BeginPopupModal("Change directory", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::InputText("Pathname", new_pathname, 4096);
      if (ImGui::Button("Set") || ImGui::IsKeyPressedMap(ImGuiKey_Enter))
      {
        current_pathname_.assign(new_pathname);
        pathname_content_ = Utilities::getFolderContents(current_pathname_, (mode_ == Mode::EDirectory));
        if (mode_ != Mode::EFileCreate)
        {
          selected_filename_.clear();
        }
        files_scanned_ = false;
        selected_entry_ = -1;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    static bool show_confirmation_popup = false;
    drawConfirmationPopup();

    filter_.Draw("Filter filename (inc,-exc)");

    // File type filter
    static int selected_extension = 0; // If the selection isn't within 0..count, Combo won't display a preview
    static const char* extensions[] = { "All", "csv", "featureXML", "mzML" };
    ImGui::Combo("File type", &selected_extension, extensions, IM_ARRAYSIZE(extensions));

    ImGui::BeginChild("Content", ImVec2(1024, 400));

    const int column_count = 4;
    const char* column_names[column_count] = { "Name", "Size", "Type", "Date Modified" };
    static ImGuiTableColumnFlags column_flags[column_count] = {
      ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide, ImGuiTableColumnFlags_NoHide,
      ImGuiTableColumnFlags_NoHide, ImGuiTableColumnFlags_NoHide
    };

    static ImGuiTableFlags table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_NoBordersInBody;

    static std::vector<ImEntry> Im_directory_entries;
    static ImVector<int> selection;
    updateContents(Im_directory_entries);

    ImVec2 size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 2);
    if (ImGui::BeginTable("FileBrowser", column_count, table_flags, size))
    {
      for (int column = 0; column < column_count; column++) {
        ImGui::TableSetupColumn(column_names[column], column_flags[column], -1.0f);
      }
      ImGui::TableSetupScrollFreeze(column_count, 1);
      ImGui::TableHeadersRow();

      if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
      {
        if (sorts_specs->SpecsDirty)
        {
          ImEntry::s_current_sort_specs = sorts_specs;
          if (Im_directory_entries.size() > 1)
            qsort(&Im_directory_entries[0], (size_t)Im_directory_entries.size(), sizeof(Im_directory_entries[0]), ImEntry::CompareWithSortSpecs);
          ImEntry::s_current_sort_specs = NULL;
          sorts_specs->SpecsDirty = false;
          selected_entry_ = -1;
        }
      }

      const ImGuiWindowFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;

      for (int row = 0; row < pathname_content_[0].size(); row++)
      {
        if (!filter_.PassFilter(pathname_content_[0][row].c_str()))
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
          ImEntry item = Im_directory_entries[row];
          Utilities::makeHumanReadable(item);
          char text_buffer[256];
          std::snprintf(text_buffer, sizeof text_buffer, "%s", item.entry_contents[column].c_str());

          const bool is_selected = (selected_entry_ == row);
          ImGui::TableSetColumnIndex(column);
          if (ImGui::Selectable(text_buffer, is_selected, selectable_flags))
          {
            selected_entry_ = row;
            bool is_dir = !std::strcmp(item.entry_contents[2].c_str(), "Directory");
            if ((is_dir && (mode_ == Mode::EDirectory)) ||
              (!is_dir && (mode_ != Mode::EDirectory)))
            {
              selected_filename_ = Im_directory_entries[selected_entry_].entry_contents[0];
            }
            if (ImGui::IsMouseDoubleClicked(0) && is_dir)
            {
              current_pathname_ /= item.entry_contents[0].c_str();
              if (mode_ != Mode::EFileCreate)
              {
                selected_filename_.clear();
              }
              files_scanned_ = false;
              updateContents(Im_directory_entries);
              filter_.Clear();
              selected_entry_ = -1;
              break;
            }
            else if (ImGui::IsMouseDoubleClicked(0) || ImGui::IsMouseClicked(0))
            {
              picked_pathname_ = current_pathname_.string();
              if (selected_entry_ >= 0)
              {
                if (picked_pathname_.back() != '/')
                {
                  picked_pathname_.append("/");
                }
                picked_pathname_.append(item.entry_contents[0].c_str());
              }
              if ((mode_ == Mode::EFileCreate) && (std::filesystem::exists(picked_pathname_)))
              {
                show_confirmation_popup = true;
              }
              else
              {
                doOpenFile();
              }
            }
          }
        }
      }
      ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    if (mode_ == Mode::EDirectory)
    {
      ImGui::InputTextWithHint("", "Directory name", &selected_filename_);
    }
    else
    {
      ImGui::InputTextWithHint("", "File name", &selected_filename_);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    if (ImGui::Button(open_button_text_.c_str()))
    {
      picked_pathname_ = current_pathname_.string();      if (picked_pathname_.back() != '/')
      {
        picked_pathname_.append("/");
      }
      picked_pathname_.append(selected_filename_);
      if ((mode_ == Mode::EFileCreate) && (std::filesystem::exists(picked_pathname_)))
      {
        show_confirmation_popup = true;
      }
      else
      {
        doOpenFile();
      }
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    if (ImGui::Button("Cancel"))
    {
      picked_pathname_.clear();
      selected_entry_ = -1;
      visible_ = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::PopItemWidth();

    if (show_confirmation_popup)
    {
      ImGui::OpenPopup("Overwrite confirmation");
      show_confirmation_popup = false;
    }

    ImGui::EndPopup();
  }

  void FilePicker::drawConfirmationPopup()
  {
    bool do_open_file = false;
    if (ImGui::BeginPopupModal("Overwrite confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("Overwrite file:");
      ImGui::Text(picked_pathname_.c_str());
      ImGui::SameLine();
      ImGui::Text("?");
      ImGui::Separator();
      if (ImGui::Button("Ok"))
      {
        try
        {
          std::filesystem::remove(picked_pathname_);
        }
        catch (const std::exception& e)
        {
          LOGE << e.what();
          // let's continue ...
        }
        ImGui::CloseCurrentPopup();
        do_open_file = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if (do_open_file)
    {
      doOpenFile();
    }
  }

  std::string FilePicker::getPickedPathname() const
  {
    return picked_pathname_;
  }

  void FilePicker::open(const std::string& title, 
                        std::shared_ptr<IFilePickerHandler> file_picker_handler,
                        FilePicker::Mode mode,
                        ApplicationHandler& application_handler,
                        const std::string& default_file_name)
  {
    LOGD << "Setting processor: " << (file_picker_handler.get());
    title_ = title;
    file_picker_handler_ = file_picker_handler;
    application_handler_ = &application_handler;
    error_loading_file_ = false;
    mode_ = mode;
    visible_ = true;
    selected_filename_ = default_file_name;
    files_scanned_ = false;
    switch (mode)
    {
    case SmartPeak::FilePicker::Mode::EFileRead:
      open_button_text_ = "Open";
      break;
    case SmartPeak::FilePicker::Mode::EFileCreate:
      open_button_text_ = "Save";
      break;
    case SmartPeak::FilePicker::Mode::EDirectory:
      open_button_text_ = "Select";
      break;
    default:
      open_button_text_ = "Open";
      break;
    }
    ImGui::OpenPopup(title_.c_str());
  }

  void FilePicker::runProcessor()
  {
    LOGD << "Running processor (ptr): " << file_picker_handler_;
    if (!file_picker_handler_)
      return;
    loading_is_done_ = false;
    run_and_join(file_picker_handler_.get(), picked_pathname_, loading_is_done_);
  }

  void FilePicker::clearProcessor()
  {
    LOGD << "Clearing processor";
    file_picker_handler_ = nullptr;
  }

  void FilePicker::run_and_join(
    IFilePickerHandler* file_picker_handler,
    const std::string& pathname,
    bool& loading_is_done
  )
  {
    std::future<bool> f = std::async(
      std::launch::async, 
      [this, file_picker_handler, pathname](){ return file_picker_handler->onFilePicked(pathname, application_handler_); }
    );

    try {
      LOGN << "File is being loaded...";
      if (f.get()) {
        LOGN << "File has been loaded.";
      }
      else 
      {
        error_loading_file_ = true;
        LOGN << "File has not been loaded.";
      }
    } catch (const std::exception& e) {
      error_loading_file_ = true;
      LOGE << e.what();
    }

    loading_is_done = true;    
  }

  void FilePicker::doOpenFile()
  {
    filter_.Clear();
    LOGI << "Picked file : " << picked_pathname_;
    if (isReadyToOpen(picked_pathname_))
    {
      runProcessor();
    }
    else
    {
      LOGE << "Invalid File selection: " << picked_pathname_;
    }
    clearProcessor();
    selected_entry_ = -1;
    if (mode_ != Mode::EFileCreate)
    {
      selected_filename_.clear();
    }
    visible_ = false;
    ImGui::CloseCurrentPopup();
  }

  bool FilePicker::isReadyToOpen(const std::string& full_path)
  {
    bool ready_to_open = false;
    switch (mode_)
    {
    case Mode::EFileCreate:
      ready_to_open = (!std::filesystem::is_directory(full_path));
      break;
    case Mode::EFileRead:
      ready_to_open = (std::filesystem::is_regular_file(full_path) && std::filesystem::exists(full_path));
      break;
    case Mode::EDirectory:
      ready_to_open = std::filesystem::is_directory(full_path);
      break;
    default:
      // should not happen
      LOGE << "Mode not supported";
      break;
    }
    return ready_to_open;
  }
}
