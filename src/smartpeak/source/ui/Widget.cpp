#include <SmartPeak/ui/Widget.h>

namespace SmartPeak
{
  void Widget::TableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked) {

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

      // NOTE: Appears to apply the filter immediately
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
}