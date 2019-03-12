#include <SmartPeak/ui/AppWindow.h>

namespace SmartPeak
{
  void AppWindow::showApp() {
    MainMenu main_menu;
    main_menu.showMainMenuBar();
  }

  void MainMenu::showMainMenuBar() {
    // Show the widgets
    if (show_app_about_) {
      ImGui::Begin("About SmartPeak", &show_app_about_, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("SmartPeak %s", 1.0); //TODO: define version function
      ImGui::Separator();
      ImGui::Text("By SmartPeak contributors.");
      ImGui::End();
    }
    //if (show_sequence_) SequenceProgressWidget(&show_sequence_progress);
    //if (show_generic_table) TableWidget(&show_generic_table);
    //if (show_file_browser) FileBrowserWidget(&show_file_browser);
    //if (show_plot) PlotWidget(&show_plot);
    //if (show_workflow) WorkflowWidget(&show_workflow);

    // Show the main menu bar
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        showMenuFile();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit"))
      {
        showMenuEdit();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
        showMenuView();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        showMenuAction();
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help"))
      {
        showMenuHelp();
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  }

  void MainMenu::showMenuFile() {
    ImGui::MenuItem("Main menu", NULL, false, false);
    if (ImGui::MenuItem("New"))
    {
      //TODO: SQL light interface
    }
    if (ImGui::MenuItem("Open...", "Ctrl+O"))
    {
      //TODO: open file browser modal
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
      //TODO: populate with recent
      ImGui::MenuItem("fish_hat.c");
      ImGui::MenuItem("fish_hat.inl");
      ImGui::MenuItem("fish_hat.h");
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
      //TODO
    }
    if (ImGui::MenuItem("Save As.."))
    {
      //TODO: open save as File browser modal
    }
    ImGui::Separator();
    if (ImGui::BeginMenu("Import ..."))
    {
      //TODO
      // sequence, parameters, and all other tables
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Export ..."))
    {
      //TODO
      // sequence, parameters, and all other tables
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
  }

  void MainMenu::showMenuEdit() {
    ImGui::MenuItem("Main menu", NULL, false, false);
    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
    ImGui::Separator();
    if (ImGui::MenuItem("Sequence")) {}
    if (ImGui::MenuItem("Transitions")) {}
    if (ImGui::MenuItem("Quant Method")) {}
    if (ImGui::MenuItem("Standards Conc")) {}
    if (ImGui::MenuItem("Comp Filters")) {}
    if (ImGui::MenuItem("Comp Group Filters")) {}
    if (ImGui::MenuItem("Comp QCs")) {}
    if (ImGui::MenuItem("Comp Group QCs")) {}
    if (ImGui::MenuItem("Parameters")) {}
    ImGui::Separator();
    if (ImGui::MenuItem("Workflow")) {}
  }

  void MainMenu::showMenuView() {
    ImGui::MenuItem("Main menu", NULL, false, false);
    ImGui::MenuItem("Sequence", NULL, &show_sequence_);
    if (ImGui::MenuItem("Sequence segments")) {}
    if (ImGui::MenuItem("Sample groups")) {}
    if (ImGui::MenuItem("Workflow wizard")) {}
    if (ImGui::MenuItem("Review plots"))
    {
      if (ImGui::MenuItem("Feature plot")) {}
      if (ImGui::MenuItem("Metric plot")) {}
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Reports"))
    {
      if (ImGui::MenuItem("Feature summary")) {}
      if (ImGui::MenuItem("Sequence summary")) {}
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Log")) {}
  }

  void MainMenu::showMenuAction() {
    ImGui::MenuItem("Main menu", NULL, false, false);
    if (ImGui::MenuItem("Run command")) {}
    if (ImGui::MenuItem("Run workflow")) {}
    if (ImGui::MenuItem("Sample groups")) {}
    if (ImGui::MenuItem("Workflow wizard")) {}
    if (ImGui::MenuItem("Quick info"))
    { // TODO: bug
      if (ImGui::MenuItem("Sequence")) {}
      if (ImGui::MenuItem("Transitions")) {}
      if (ImGui::MenuItem("Quant Method")) {}
      if (ImGui::MenuItem("Standards Conc")) {}
      if (ImGui::MenuItem("Comp Filters")) {}
      if (ImGui::MenuItem("Comp Group Filters")) {}
      if (ImGui::MenuItem("Comp QCs")) {}
      if (ImGui::MenuItem("Comp Group QCs")) {}
      if (ImGui::MenuItem("Parameters")) {}
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Integrity checks"))
    {  // TODO: bug
      if (ImGui::MenuItem("Sample consistency")) {}
      if (ImGui::MenuItem("Comp consistency")) {}
      if (ImGui::MenuItem("Comp Group consistency")) {}
      if (ImGui::MenuItem("IS consistency")) {}
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Report"))
    {  // TODO: bug
      if (ImGui::MenuItem("Feature summary")) {}
      if (ImGui::MenuItem("Sequence summary")) {}
      ImGui::EndMenu();
    }
  }

  void MainMenu::showMenuHelp() {
    ImGui::MenuItem("Main menu", NULL, false, false);
    ImGui::MenuItem("About", NULL, &show_app_about_);
    if (ImGui::MenuItem("Documentation")) {}
    if (ImGui::MenuItem("Version")) {}
  }
}