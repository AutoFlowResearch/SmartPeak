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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>
#undef main

void getDummyTableEntries(Eigen::Tensor<std::string, 2>& rows_out)
{
  SmartPeak::SequenceHandler sequenceHandler;
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
    SmartPeak::MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(SmartPeak::SampleType::Unknown);
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

    SmartPeak::Filenames filenames;
    filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH(metaDataHandler.getInjectionName() + ".featureXML");
    SmartPeak::RawDataHandler rawDataHandler;
    SmartPeak::LoadFeatures loadFeatures;
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
  const std::set<SmartPeak::SampleType> sample_types = {SmartPeak::SampleType::Unknown};

  SmartPeak::SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out,
                                                         meta_data, sample_types, std::set<std::string>(),
                                                         std::set<std::string>(), std::set<std::string>());
}


TEST(Widget, widget_constructors)
{
  SmartPeak::GenericTableWidget* generictablewidget_ptr = nullptr;
  EXPECT_NO_THROW(generictablewidget_ptr = new SmartPeak::GenericTableWidget("empty_generic_table"));
  delete generictablewidget_ptr;
  
  SmartPeak::ExplorerWidget* explorerwidget_ptr = nullptr;
  EXPECT_NO_THROW(explorerwidget_ptr = new SmartPeak::ExplorerWidget("empty_explorer_table"));
  delete explorerwidget_ptr;
}

TEST(Widget, GenericTableWidget_sorter)
{
  bool is_scanned =                 false;
  std::vector<SmartPeak::ImEntry>   Im_table_entries;
  const Eigen::Tensor<bool, 2>      checkbox_columns(false, false);
  Eigen::Tensor<std::string, 2>     rows_out;
  
  getDummyTableEntries(rows_out);
  SmartPeak::GenericTableWidget TestTable1("TestTable1");
  TestTable1.updateTableContents(Im_table_entries, is_scanned, rows_out, checkbox_columns);
  EXPECT_TRUE(is_scanned);  // updateTableContents is successful
  
  EXPECT_TRUE(Im_table_entries.size() > 0);
  EXPECT_TRUE(Im_table_entries[0].entry_contents.size() > 0);
  
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
  std::vector<SmartPeak::ImEntry>   Im_table_entries;
  const Eigen::Tensor<bool, 2>      checkbox_columns(false, false);
  Eigen::Tensor<std::string, 2>     rows_out;
  
  getDummyTableEntries(rows_out);
  SmartPeak::GenericTableWidget TestTable1("TestTable2");
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
