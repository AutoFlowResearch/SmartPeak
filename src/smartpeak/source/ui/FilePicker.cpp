#include <SmartPeak/core/AppStateProcessor.h> // TODO: maybe forward declaration could do it
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/core/Utilities.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void FilePicker::draw()
  {
    ImGui::OpenPopup(title_.c_str());

    // File picker modal
    if (!ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_NoResize)) {
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
    // Start of Columns - content_columns
    ImGui::Columns(4, "content_columns");
    ImGui::Separator();
    ImGui::Text("Name"); ImGui::NextColumn();
    ImGui::Text("Size"); ImGui::NextColumn();
    ImGui::Text("Type"); ImGui::NextColumn();
    ImGui::Text("Date Modified"); ImGui::NextColumn();
    ImGui::Separator();

    for (int i = 0; static_cast<size_t>(i) < pathname_content_[0].size(); ++i)
    {
      if (!filter.PassFilter(pathname_content_[0][i].c_str()))
      {
        continue; // continue if it does not pass the filter
      }

      if (selected_extension > 0 &&
          !Utilities::endsWith(pathname_content_[0][i], "." + std::string(extensions[selected_extension]), false))
      {
        continue; // continue if the file type is not desired
      }

      const ImGuiWindowFlags flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;

      if (ImGui::Selectable(pathname_content_[0][i].c_str(), selected_entry == i, flags))
      {
        selected_entry = i;
        sprintf(selected_filename, "%s", pathname_content_[0][selected_entry].c_str());
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

      ImGui::NextColumn();
      ImGui::Text("%s", pathname_content_[1][i].c_str()); ImGui::NextColumn();
      ImGui::Text("%s", pathname_content_[2][i].c_str()); ImGui::NextColumn();
      ImGui::Text("%s", pathname_content_[3][i].c_str()); ImGui::NextColumn();
    }
    ImGui::Columns(1);
    // End of Columns - content_columns
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.9f);
    ImGui::InputTextWithHint("", "File name", selected_filename, IM_ARRAYSIZE(selected_filename));

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
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
      show_file_picker_ = false;
      selected_entry = -1;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    if (ImGui::Button("Cancel"))
    {
      picked_pathname_.clear();
      show_file_picker_ = false;
      selected_entry = -1;
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  std::string FilePicker::getPickedPathname() const
  {
    return picked_pathname_;
  }

  void FilePicker::setTitle(const std::string& title)
  {
    title_ = title;
  }

  void FilePicker::setProcessor(AppStateProcessor& processor)
  {
    LOGD << "Setting processor: " << (&processor);
    processor_ = &processor;
  }

  void FilePicker::runProcessor()
  {
    LOGD << "Running processor (ptr): " << processor_;
    if (!processor_)
      return;
    (*processor_)(picked_pathname_.c_str());
  }

  void FilePicker::clearProcessor()
  {
    LOGD << "Clearing processor";
    processor_ = nullptr;
  }
}
