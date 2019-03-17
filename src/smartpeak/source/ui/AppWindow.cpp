#include <SmartPeak/ui/AppWindow.h>
#include <SmartPeak/ui/Widget.h>

namespace SmartPeak
{
  void AppWindow::showApp() {
    // Show the main window
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    static ImVec2 display_size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    static bool show_main_window = true;
    if (ImGui::Begin("Sequence list", &show_main_window, window_flags))
    {
      // Main menu bar
      MainMenu main_menu;
      main_menu.showMainMenuBar();
      
      // left (icon pain, set w ~1/20 window size) e.g., injections, components
      ImGui::BeginChild("Icon pane", ImVec2(50, 0), true);
      ImGui::Text("TODO: icons");
      ImGui::EndChild();
      ImGui::SameLine();

      // left (explorer pain, 1/5 w window size)
      ImGui::BeginChild("Explorer pane", ImVec2(display_size.x*0.2, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
      ImGui::Text("TODO: search and sort bar");
      ImGui::Text("TODO: list or tree");
      ImGui::EndChild();
      ImGui::SameLine();

      // right top (main pain 3/5 w 5/6 l window size)
      //ImGui::BeginChild("Main pane", ImVec2(0, 0), true);
      ImGui::BeginChild("Main pane", ImVec2(0, display_size.y*0.8), true);
      ImGui::Text("TODO: main graphics");
      ImGui::EndChild();

      //// right bottom (log/output pain, 3/5 w 1/6 l window size)
      //ImGui::SetNextWindowPos(ImVec2(display_size.x*0.2 + 50, display_size.y*0.8));
      //ImGui::BeginChild("Info pane", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar);
      //ImGui::Text("TODO: output and info");
      //ImGui::EndChild();
    }
    ImGui::End();
  }

  void MainMenu::showMainMenuBar() {
    // Show the widgets
    //SequenceWidget sequenceWidget;
    //if (show_sequence_) sequenceWidget.show(&show_sequence_);
    //if (show_generic_table) TableWidget(&show_generic_table);
    //if (show_file_browser) FileBrowserWidget(&show_file_browser);
    //if (show_plot) PlotWidget(&show_plot);
    //if (show_workflow) WorkflowWidget(&show_workflow);

    // Show the main menu bar
    if (ImGui::BeginMenuBar())
    // if (ImGui::BeginMainMenuBar())
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
      ImGui::EndMenuBar();
      //ImGui::EndMainMenuBar();
    }
  }

  void MainMenu::showMenuFile() {
    ImGui::MenuItem("File", NULL, false, false);
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
    ImGui::MenuItem("Edit", NULL, false, false);
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
    ImGui::MenuItem("View", NULL, false, false);
    ImGui::MenuItem("Sequence", NULL, &show_sequence_);
    if (ImGui::MenuItem("Sequence segments")) {}
    if (ImGui::MenuItem("Sample groups")) {}
    if (ImGui::MenuItem("Workflow wizard")) {}
    if (ImGui::BeginMenu("Review plots"))
    {
      if (ImGui::MenuItem("Feature plot")) {}
      if (ImGui::MenuItem("Metric plot")) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Reports"))
    {
      if (ImGui::MenuItem("Feature summary")) {}
      if (ImGui::MenuItem("Sequence summary")) {}
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Log")) {}
  }

  void MainMenu::showMenuAction() {
    ImGui::MenuItem("Action", NULL, false, false);
    if (ImGui::MenuItem("Run command")) {}
    if (ImGui::MenuItem("Run workflow")) {}
    if (ImGui::MenuItem("Sample groups")) {}
    if (ImGui::MenuItem("Workflow wizard")) {}
    if (ImGui::BeginMenu("Quick info"))
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
    if (ImGui::BeginMenu("Integrity checks"))
    {  // TODO: bug
      if (ImGui::MenuItem("Sample consistency")) {}
      if (ImGui::MenuItem("Comp consistency")) {}
      if (ImGui::MenuItem("Comp Group consistency")) {}
      if (ImGui::MenuItem("IS consistency")) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Report"))
    {  // TODO: bug
      if (ImGui::MenuItem("Feature summary")) {}
      if (ImGui::MenuItem("Sequence summary")) {}
      ImGui::EndMenu();
    }
  }

  void MainMenu::showMenuHelp() {
    ImGui::MenuItem("Main menu", NULL, false, false);
    if (ImGui::MenuItem("About")) {
      ImGui::OpenPopup("about");
      if (ImGui::BeginPopupModal("about"))
      {
        //AboutWidget aboutWidget;
        //bool show_about = true;
        //aboutWidget.show(&show_about);
        ImGui::Text("About SmartPeak");
        ImGui::Text("SmartPeak %s", 1.0); //TODO: define version function
        ImGui::Separator();
        ImGui::Text("By the hardworking SmartPeak developers.");
        ImGui::EndPopup();
      }
    };

    if (ImGui::MenuItem("Documentation")) {}
    if (ImGui::MenuItem("Version")) {}


  }
}