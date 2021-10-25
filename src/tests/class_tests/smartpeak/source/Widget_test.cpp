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
// $Maintainer: Ahmed Khalil, Bertrand Boudaud $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <SmartPeak/ui/WorkflowWidget.h>
#include <SmartPeak/ui/SessionFilesWidget.h>
#include <SmartPeak/ui/LoadSessionWizard.h>

using namespace SmartPeak;

bool SmartPeak::enable_quick_help = false;

void getDummyTableEntries(Eigen::Tensor<std::string, 2>& rows_out)
{
  SequenceHandler sequenceHandler;
  std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");

  const std::vector<std::string> sample_names = {
    "170808_Jonathan_yeast_Sacc1_1x",
    "170808_Jonathan_yeast_Sacc2_1x",
    "170808_Jonathan_yeast_Sacc3_1x",
    "170808_Jonathan_yeast_Yarr1_1x",
    "170808_Jonathan_yeast_Yarr2_1x",
    "170808_Jonathan_yeast_Yarr3_1x"
  };

  int inj_num = 0;
  for (const std::string& sample_name : sample_names) {
    ++inj_num;
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.acq_method_name = "6";
    metaDataHandler.inj_number = inj_num;
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "FluxTest";
    metaDataHandler.scan_polarity = "negative";
    metaDataHandler.scan_mass_high = 2000;
    metaDataHandler.scan_mass_low = 60;

    Filenames filenames;
    filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH(metaDataHandler.getInjectionName() + ".featureXML"));
    RawDataHandler rawDataHandler;
    LoadFeatures loadFeatures;
    loadFeatures.process(rawDataHandler, {}, filenames);

    sequenceHandler.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
  }

  Eigen::Tensor<float, 2> data_out;
  Eigen::Tensor<std::string, 1> columns_out;

  const std::vector<std::string> meta_data = {
    "calculated_concentration",
    "leftWidth",
    "rightWidth"
  };
  const std::set<SampleType> sample_types = {SampleType::Unknown};

  SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out,
                                                         meta_data, sample_types, std::set<std::string>(),
                                                         std::set<std::string>(), std::set<std::string>());
}

TEST(Widget, widget_constructors)
{
  GenericTableWidget* generictablewidget_ptr = nullptr;
  EXPECT_NO_THROW(generictablewidget_ptr = new GenericTableWidget("empty_generic_table"));
  delete generictablewidget_ptr;
  
  ExplorerWidget* explorerwidget_ptr = nullptr;
  EXPECT_NO_THROW(explorerwidget_ptr = new ExplorerWidget("empty_explorer_table"));
  delete explorerwidget_ptr;
}

TEST(Widget, GenericTableWidget_sorter)
{
  bool is_scanned =                 false;
  std::vector<ImEntry>   Im_table_entries;
  const Eigen::Tensor<bool, 2>      checkbox_columns(false, false);
  Eigen::Tensor<std::string, 2>     rows_out;
  
  getDummyTableEntries(rows_out);
  GenericTableWidget TestTable1("TestTable1");
  TestTable1.updateTableContents(Im_table_entries, is_scanned, rows_out, checkbox_columns);
  EXPECT_TRUE(is_scanned);  // updateTableContents is successful
  
  ASSERT_TRUE(Im_table_entries.size() > 0);
  ASSERT_TRUE(Im_table_entries[0].entry_contents.size() > 0);
  
  // pre-sorting assertion
  // 1st row
  EXPECT_STREQ(Im_table_entries[0].entry_contents[0].c_str(), "23dpg.23dpg_1.Heavy");
  EXPECT_STREQ(Im_table_entries[0].entry_contents[1].c_str(), "23dpg");
  // last row
  auto last_row_idx = Im_table_entries.size() - 1;
  EXPECT_STREQ(Im_table_entries[last_row_idx].entry_contents[0].c_str(), "xan.xan_1.Light");
  EXPECT_STREQ(Im_table_entries[last_row_idx].entry_contents[1].c_str(), "xan");
  
  // sorting
  ImGuiTableSortSpecs           sorts_specs;
  ImGuiTableColumnSortSpecs     descending_1;
  descending_1.ColumnUserID   = (ImGuiID)0;
  descending_1.SortDirection  = 2;
  descending_1.ColumnIndex    = (ImS16)0;
  descending_1.SortOrder      = 2;
    
  sorts_specs.Specs           = &descending_1;
  sorts_specs.SpecsDirty      = true;
  sorts_specs.SpecsCount      = 3;
  
  //TestTable1.sorter(Im_table_entries, &sorts_specs, is_scanned);
  
  // post-sorting assertion
  // 1st row
  //EXPECT_STREQ(Im_table_entries[0].entry_contents[0], "xan.xan_1.Light");
  //EXPECT_STREQ(Im_table_entries[0].entry_contents[1], "xan");
  // last row
  //EXPECT_STREQ(Im_table_entries[last_row_idx].entry_contents[0], "2mcit.2mcit_1.Heavy");
  //EXPECT_STREQ(Im_table_entries[last_row_idx].entry_contents[1], "2mcit");
}

TEST(Widget, GenericTableWidget_searcher)
{
  bool is_scanned =                 false;
  std::vector<ImEntry>   Im_table_entries;
  const Eigen::Tensor<bool, 2>      checkbox_columns(false, false);
  Eigen::Tensor<std::string, 2>     rows_out;
  
  getDummyTableEntries(rows_out);
  GenericTableWidget TestTable1("TestTable2");
  TestTable1.updateTableContents(Im_table_entries, is_scanned, rows_out, checkbox_columns);
  EXPECT_TRUE(is_scanned); // updateTableContents is successful
  
  const int         all_selected_entry = 0;
  std::vector<uint> found_in;
  ImGuiTextFilter   filter("2mcit.2mcit_1.Heavy");
  
  for (uint row = 0; row < Im_table_entries.size(); ++row) {
    if (!TestTable1.searcher(Im_table_entries, all_selected_entry, filter, row)) {
    }
  }
  
  for (auto found_in_index : found_in) {
    EXPECT_STREQ(Im_table_entries[found_in_index].entry_contents[0].c_str(), "2mcit.2mcit_1.Heavy");
  }
}

class GraphicDataVizWidget_Test : public GraphicDataVizWidget
{
public:
  GraphicDataVizWidget_Test(
    SessionHandler& session_handler,
    ApplicationHandler& application_handler,
    const std::string& id,
    const std::string& title) :
    GraphicDataVizWidget(session_handler, application_handler, id, title)
  {};

  virtual void updateData() override
  {
//    GraphicDataVizWidget(SessionHandler& session_handler,
//      ApplicationHandler& application_handler,
//      const std::string& id,
//      const std::string& title) :
  };

public:
  // wrappers to protected methods
  void wrapper_setMarkerPosition(const std::optional<float>& marker_position)
  {
    setMarkerPosition(marker_position);
  }

  std::optional<float> wrapper_getMarkerPosition() const
  {
    return getMarkerPosition();
  }

  std::tuple<float, float, float, float> wrapper_plotLimits() const
  {
    return plotLimits();
  }

public:
  // setters for test
  void setGraphVizData(SessionHandler::GraphVizData& graph_viz_data)
  {
    graph_viz_data_ = graph_viz_data;
  }

};

TEST(GraphicDataVizWidget, markerPosition)
{
  SmartPeak::SessionHandler session_handler;
  SmartPeak::ApplicationHandler application_handler;
  std::string id = "GraphicDataVizWidget";
  std::string title = "GraphicDataVizWidget";
  GraphicDataVizWidget_Test test_graphic_data_viz(session_handler, application_handler, id, title);
  EXPECT_FALSE(test_graphic_data_viz.wrapper_getMarkerPosition());
  test_graphic_data_viz.wrapper_setMarkerPosition(42.0f);
  EXPECT_EQ(test_graphic_data_viz.wrapper_getMarkerPosition(), 42.0f);
}

TEST(GraphicDataVizWidget, plotLimits)
{
  SmartPeak::SessionHandler session_handler;
  SmartPeak::ApplicationHandler application_handler;
  std::string id = "GraphicDataVizWidget";
  std::string title = "GraphicDataVizWidget";
  GraphicDataVizWidget_Test test_graphic_data_viz(session_handler, application_handler, id, title);
  
  std::vector<float> data_x_1 = { 1.0f, 10.0f, 5.0f };
  std::vector<float> data_y_1 = { 101.0f, 110.0f, 105.0f };
  std::vector<float> data_x_2 = { 201.0f, 210.0f, 205.0f };
  std::vector<float> data_y_2 = { 301.0f, 310.0f, 305.0f };

  SessionHandler::GraphVizData graph_viz_data;
  graph_viz_data.reset("x_axis", "y_axis", "z_axis", 100);
  graph_viz_data.addData(data_x_1, data_y_1, "data1");
  graph_viz_data.addData(data_x_2, data_y_2, "data2");
  test_graphic_data_viz.setGraphVizData(graph_viz_data);

  const auto [plot_min_x, plot_max_x, plot_min_y, plot_max_y] = test_graphic_data_viz.wrapper_plotLimits();
  EXPECT_NEAR(plot_min_x, -2.09, 1e-6);
  EXPECT_NEAR(plot_max_x, 2.09, 1e-6);
  EXPECT_NEAR(plot_min_y, 98.910003662109375, 1e-6);
  EXPECT_NEAR(plot_max_y, 341, 1e-6);
}

class WorkflowWidget_Test : public WorkflowWidget
{
public:
  WorkflowWidget_Test(const std::string title, 
                      ApplicationHandler& application_handler, 
                      WorkflowManager& workflow_manager) :
    WorkflowWidget(title, application_handler, workflow_manager)
  {};

public:
  // wrappers to protected methods
  virtual void updatecommands() override
  {
    WorkflowWidget::updatecommands();
  }

  bool errorBuildingCommands()
  {
    return error_building_commands_;
  }
};

TEST(WorkflowWidget, updateCommands)
{
  ApplicationHandler application_handler;
  WorkflowManager workflow_manager;
  WorkflowWidget_Test workflow_widget("Workflow Widget", application_handler, workflow_manager);
  
  // initial state
  EXPECT_FALSE(workflow_widget.errorBuildingCommands());
  
  // empty commands
  workflow_widget.updatecommands();
  EXPECT_FALSE(workflow_widget.errorBuildingCommands());

  application_handler.sequenceHandler_.setWorkflow({ "LOAD_FEATURES" });
  workflow_widget.updatecommands();
  EXPECT_FALSE(workflow_widget.errorBuildingCommands());
  
  application_handler.sequenceHandler_.setWorkflow({ "NON_EXISTING_COMMAND" });
  workflow_widget.updatecommands();
  EXPECT_TRUE(workflow_widget.errorBuildingCommands());
}

class SessionFilesWidget_Test : public SessionFilesWidget
{
public:
  SessionFilesWidget_Test(ApplicationHandler& application_handler, SessionFilesWidget::Mode mode) :
  SessionFilesWidget(application_handler, mode, nullptr)
  {};

public:
  // wrappers to protected methods
  virtual void doUpdateSession() override
  {
    SessionFilesWidget::doUpdateSession();
  }

  virtual bool isModified(const std::string& file_id) const override
  {
    return SessionFilesWidget::isModified(file_id);
  }

  virtual bool isToBeSaved(const std::string& file_id) const override
  {
    return SessionFilesWidget::isToBeSaved(file_id);
  }

  virtual void clearEntry(const std::string& file_id) override
  {
    SessionFilesWidget::clearEntry(file_id);
  }

  // Access to private member to test
  std::map<std::string, FileEditorFields>& getEditorFileFields()
  {
    return file_editor_fields_;
  }
};

TEST(SessionFilesWidget, SessionFilesWidget_Create)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test(application_handler, SessionFilesWidget::Mode::ECreation);
  
  session_widget_test.open(filenames);
  auto& file_editor_fields = session_widget_test.getEditorFileFields();
  EXPECT_EQ(file_editor_fields.size(), 23);

  // we only test fields that are displayed to the user
  auto& fef = file_editor_fields.at("parameters");
  EXPECT_EQ(fef.text_editor_hint_, std::string("parameters.csv"));
  EXPECT_EQ(fef.text_editor_, std::string("parameters.csv"));
  EXPECT_EQ(fef.embedded_, false);

  fef = file_editor_fields.at("referenceData");
  EXPECT_EQ(fef.text_editor_hint_, std::string("referenceData.csv"));
  EXPECT_EQ(fef.text_editor_, std::string(""));
  EXPECT_EQ(fef.embedded_, false);

  for (auto& fef_m : file_editor_fields)
  {
    EXPECT_EQ(session_widget_test.isToBeSaved(fef_m.first), false);
  }
}

TEST(SessionFilesWidget, SessionFilesWidget_isModified)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test.open(filenames);
  EXPECT_EQ(session_widget_test.isModified("parameters"), false);
}

TEST(SessionFilesWidget, SessionFilesWidget_clearEntry)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test.open(filenames);

  auto& file_editor_fields = session_widget_test.getEditorFileFields();
  auto& fef = file_editor_fields.at("parameters");
  fef.embedded_ = true;
  fef.text_editor_ = "test";

  session_widget_test.clearEntry("parameters");
  EXPECT_EQ(session_widget_test.isModified("parameters"), true);
  EXPECT_EQ(fef.text_editor_hint_, std::string("parameters.csv"));
  EXPECT_EQ(fef.text_editor_, std::string(""));
  EXPECT_EQ(fef.embedded_, false);
}

TEST(SessionFilesWidget, SessionFilesWidget_doUpdateSession)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test(application_handler, SessionFilesWidget::Mode::ECreation);

  session_widget_test.open(filenames);
  session_widget_test.doUpdateSession();
  // TODO uncomment this: we need to set 'filenames' as not saved
  // EXPECT_EQ(application_handler.sessionIsSaved(), false);

  EXPECT_EQ(application_handler.sequenceHandler_.getSequence().size(), 2);
  const auto& app_filenames = application_handler.filenames_;
  EXPECT_EQ(app_filenames.getFileIds().size(), 23);
  EXPECT_EQ(application_handler.isFileSaved("parameters"), true);
}

TEST(SessionFilesWidget, SessionFilesWidget_Modify_ChangeExternalFile)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test_create(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test_create.open(filenames);
  session_widget_test_create.doUpdateSession();

  SessionFilesWidget_Test session_widget_test_modify(application_handler, SessionFilesWidget::Mode::EModification);
  session_widget_test_modify.open(application_handler.filenames_);

  auto& file_editor_fields = session_widget_test_modify.getEditorFileFields();
  EXPECT_EQ(file_editor_fields.size(), 23);

  auto& fef = file_editor_fields.at("parameters");
  EXPECT_EQ(fef.text_editor_hint_, std::string("parameters.csv"));
  EXPECT_EQ(fef.text_editor_, std::string("parameters.csv"));
  EXPECT_EQ(fef.embedded_, false);
  EXPECT_EQ(session_widget_test_modify.isModified("parameters"), false);

  ASSERT_GT(application_handler.sequenceHandler_.getSequence().size(), 0);
  ParameterSet& parameter_set = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  EXPECT_EQ(parameter_set.findParameter("MRMFeatureFinderScoring", "TransitionGroupPicker:peak_integration")->getValueAsString(), "smoothed");

  // Modify the parameter file pointer to another parameter file
  fef.text_editor_ = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_2.csv");
  EXPECT_EQ(session_widget_test_modify.isModified("parameters"), true);
  session_widget_test_modify.doUpdateSession();

  ASSERT_GT(application_handler.sequenceHandler_.getSequence().size(), 0);
  session_widget_test_modify.open(application_handler.filenames_);
  ParameterSet& parameter_set2 = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  auto parameter2 = parameter_set2.findParameter("MRMFeatureFinderScoring", "TransitionGroupPicker:peak_integration");
  ASSERT_NE(parameter2, nullptr);
  EXPECT_EQ(parameter2->getValueAsString(), "original");
}

TEST(SessionFilesWidget, SessionFilesWidget_Modify_ChangeFromExternalToEmbedded)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test_create(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test_create.open(filenames);
  session_widget_test_create.doUpdateSession();

  SessionFilesWidget_Test session_widget_test_modify(application_handler, SessionFilesWidget::Mode::EModification);
  session_widget_test_modify.open(application_handler.filenames_);

  auto& file_editor_fields = session_widget_test_modify.getEditorFileFields();
  EXPECT_EQ(file_editor_fields.size(), 23);

  auto& fef = file_editor_fields.at("parameters");
  EXPECT_EQ(fef.text_editor_hint_, std::string("parameters.csv"));
  EXPECT_EQ(fef.text_editor_, std::string("parameters.csv"));
  EXPECT_EQ(fef.embedded_, false);
  EXPECT_EQ(session_widget_test_modify.isModified("parameters"), false);

  ASSERT_GT(application_handler.sequenceHandler_.getSequence().size(), 0);
  ParameterSet& parameter_set = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  EXPECT_EQ(parameter_set.findParameter("MRMFeatureFinderScoring", "TransitionGroupPicker:peak_integration")->getValueAsString(), "smoothed");

  // Modify the parameter file pointer to another parameter file, and check it as embedded
  fef.text_editor_ = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_2.csv");
  fef.embedded_ = true;
  EXPECT_EQ(session_widget_test_modify.isModified("parameters"), true);
  session_widget_test_modify.doUpdateSession();

  // Check the change (without saving)
  ASSERT_GT(application_handler.sequenceHandler_.getSequence().size(), 0);
  session_widget_test_modify.open(application_handler.filenames_);
  ParameterSet& parameter_set2 = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  auto parameter2 = parameter_set2.findParameter("MRMFeatureFinderScoring", "TransitionGroupPicker:peak_integration");
  ASSERT_NE(parameter2, nullptr);
  EXPECT_EQ(parameter2->getValueAsString(), "original");

  // Save and Reload, to check it's well embedded
  auto tmp_dir_path = Utilities::createEmptyTempDirectory();
  auto db_path = tmp_dir_path / "session.db";
  SaveSession save_session(application_handler);
  save_session.onFilePicked(db_path, &application_handler);
  auto session_widget_test_modify2 = std::make_shared<SessionFilesWidget>(application_handler, SessionFilesWidget::Mode::EModification, nullptr);
  std::vector<IMetadataHandler*> to_serialize;
  auto load_session_wizard_ = std::make_shared<LoadSessionWizard>(session_widget_test_modify2, nullptr, to_serialize);
  load_session_wizard_->onFilePicked(db_path, &application_handler);
  ParameterSet& parameter_set3 = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  auto parameter3 = parameter_set3.findParameter("MRMFeatureFinderScoring", "TransitionGroupPicker:peak_integration");
  ASSERT_NE(parameter3, nullptr);
  EXPECT_EQ(parameter3->getValueAsString(), "original");
}

TEST(SessionFilesWidget, SessionFilesWidget_Modify_FileContent)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test_create(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test_create.open(filenames);
  session_widget_test_create.doUpdateSession();

  // Change value of one parameter
  ParameterSet& parameter_set = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  EXPECT_EQ(parameter_set.findParameter("AbsoluteQuantitation", "min_points")->getValueAsString(), "4");
  parameter_set.findParameter("AbsoluteQuantitation", "min_points")->setValueFromString("42");
  application_handler.sequenceHandler_.notifyParametersUpdated();

  SessionFilesWidget_Test session_widget_test_modify(application_handler, SessionFilesWidget::Mode::EModification);
  session_widget_test_modify.open(application_handler.filenames_);
  // only the parameter data source should be tagged as to be saved
  auto& file_editor_fields = session_widget_test_modify.getEditorFileFields();
  for (auto& fef_m : file_editor_fields)
  {
    EXPECT_EQ(session_widget_test_modify.isToBeSaved(fef_m.first), (fef_m.first =="parameters"));
  }
}

TEST(SessionFilesWidget, SessionFilesWidget_Modify_NoPopupError)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test_create(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test_create.open(filenames);

  // Clear all, keep sequence and parameters both embedded
  auto& file_editor_fields = session_widget_test_create.getEditorFileFields();
  for (auto& fef : file_editor_fields)
  {
    if ((fef.first == "parameters") || (fef.first == "sequence"))
    {
      fef.second.embedded_ = true;
    }
    else
    {
      session_widget_test_create.clearEntry(fef.first);
    }
  }
  session_widget_test_create.doUpdateSession();

  // save session in temporary place, outside working directory.
  auto tmp_dir_path = Utilities::createEmptyTempDirectory();
  auto db_path = tmp_dir_path / "session.db";
  SaveSession save_session(application_handler);
  save_session.onFilePicked(db_path, &application_handler);

  // load session
  auto session_widget_test_modify = std::make_shared<SessionFilesWidget>(application_handler, SessionFilesWidget::Mode::EModification, nullptr);
  std::vector<IMetadataHandler*> to_serialize;
  auto load_session_wizard_ = std::make_shared<LoadSessionWizard>(session_widget_test_modify, nullptr, to_serialize);
  load_session_wizard_->onFilePicked(db_path, &application_handler);
  
  // popup should not be displayed
  EXPECT_EQ(session_widget_test_modify->visible_, false);

  // check data
  EXPECT_EQ(application_handler.sequenceHandler_.getSequence().size(), 2);
  EXPECT_EQ(application_handler.sequenceHandler_.getSequence()[0].getRawData().getParameters().size(), 27);
}

TEST(SessionFilesWidget, LoadSessionWizard_PopupError)
{
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test_create(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test_create.open(filenames);

  // Clear all, keep sequence and parameters, sequence only embedded
  auto& file_editor_fields = session_widget_test_create.getEditorFileFields();
  for (auto& fef : file_editor_fields)
  {
    if (fef.first == "sequence")
    {
      fef.second.embedded_ = true;
    }
    else if (fef.first == "parameters")
    {
      // nothing
    }
    else
    {
      session_widget_test_create.clearEntry(fef.first);
    }
  }
  session_widget_test_create.doUpdateSession();

  // save session in temporary place, outside working directory.
  auto tmp_dir_path = Utilities::createEmptyTempDirectory();

  auto db_path = tmp_dir_path / "session.db";
  SaveSession save_session(application_handler);
  save_session.onFilePicked(db_path, &application_handler);

  // load session
  auto session_widget_test_modify = std::make_shared<SessionFilesWidget_Test>(application_handler, SessionFilesWidget::Mode::EModification);
  auto session_widget_modify = std::static_pointer_cast<SessionFilesWidget>(session_widget_test_modify);
  std::vector<IMetadataHandler*> to_serialize;
  auto load_session_wizard_ = std::make_shared<LoadSessionWizard>(session_widget_modify, nullptr, to_serialize);
  load_session_wizard_->onFilePicked(db_path, &application_handler);

  // check that the wizard is displayed.
  EXPECT_EQ(session_widget_modify->visible_, true);
  // other checks...
  auto& file_editor_fields2 = session_widget_test_modify->getEditorFileFields();
  for (auto& fef : file_editor_fields2)
  {
    EXPECT_EQ(session_widget_test_modify->isToBeSaved(fef.first), false);
  }
}

TEST(SessionFilesWidget, SessionFilesWidget_EmbedAllFiles)
{
  // More integration test like, 
  // we just try to embed lot of file from different nature: parameters, feature filters files ...
  // Note: we save in working directory as this implies differrent cases compared to other test case above.
  ApplicationHandler application_handler;
  Filenames filenames = Utilities::buildFilenamesFromDirectory(application_handler, SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files"));
  SessionFilesWidget_Test session_widget_test_create(application_handler, SessionFilesWidget::Mode::ECreation);
  session_widget_test_create.open(filenames);
  auto& file_editor_fields = session_widget_test_create.getEditorFileFields();
  EXPECT_EQ(file_editor_fields.size(), 23);

  // embed files
  file_editor_fields.at("parameters").embedded_ = true;
  file_editor_fields.at("featureBackgroundFilterComponentGroups").embedded_ = true;
  file_editor_fields.at("featureBackgroundFilterComponents").embedded_ = true;
  file_editor_fields.at("featureBackgroundQCComponentGroups").embedded_ = true;
  file_editor_fields.at("featureBackgroundQCComponents").embedded_ = true;
  file_editor_fields.at("featureFilterComponents").embedded_ = true;
  file_editor_fields.at("featureFilterComponentGroups").embedded_ = true;
  file_editor_fields.at("featureQCComponents").embedded_ = true;
  file_editor_fields.at("featureQCComponentGroups").embedded_ = true;
  file_editor_fields.at("featureRSDFilterComponentGroups").embedded_ = true;
  file_editor_fields.at("featureRSDFilterComponents").embedded_ = true;
  file_editor_fields.at("featureRSDQCComponentGroups").embedded_ = true;
  file_editor_fields.at("featureRSDQCComponents").embedded_ = true;
  file_editor_fields.at("sequence").embedded_ = true;
  file_editor_fields.at("standardsConcentrations").embedded_ = true;
  session_widget_test_create.doUpdateSession();

  // Save (in working directory) and Reload, to check it's well embedded
  auto tmp_dir_path = Utilities::createEmptyTempDirectory();
  auto db_path = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files") + std::string("/session_test.db");
  SaveSession save_session(application_handler);
  save_session.onFilePicked(db_path, &application_handler);

  auto session_widget_test_modify = std::make_shared<SessionFilesWidget_Test>(application_handler, SessionFilesWidget::Mode::EModification);
  auto session_widget_modify = std::static_pointer_cast<SessionFilesWidget>(session_widget_test_modify);
  auto load_session_wizard_ = std::make_shared<LoadSessionWizard>(session_widget_modify, nullptr);
  load_session_wizard_->onFilePicked(db_path, &application_handler);

  ParameterSet& parameter_set3 = application_handler.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
  auto parameter3 = parameter_set3.findParameter("MRMFeatureFinderScoring", "TransitionGroupPicker:peak_integration");
  ASSERT_NE(parameter3, nullptr);
  EXPECT_EQ(parameter3->getValueAsString(), "smoothed");
}