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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#include <gtest/gtest.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/io/InputDataValidation.h> // check filenames and headers
#include <SmartPeak/core/RawDataProcessors/LoadRawData.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>
#include <SmartPeak/core/RawDataProcessors/LoadTransitions.h>
#include <SmartPeak/core/RawDataProcessors/PickMRMFeatures.h>
#include <SmartPeak/core/RawDataProcessors/FilterFeatures.h>
#include <SmartPeak/core/RawDataProcessors/SelectFeatures.h>
#include <SmartPeak/core/RawDataProcessors/ValidateFeatures.h>
#include <SmartPeak/core/RawDataProcessors/QuantifyFeatures.h>
#include <SmartPeak/core/RawDataProcessors/CheckFeatures.h>
#include <SmartPeak/core/RawDataProcessors/StoreFeatures.h>
#include <SmartPeak/core/RawDataProcessors/MapChromatograms.h>
#include <SmartPeak/core/RawDataProcessors/ZeroChromatogramBaseline.h>
#include <SmartPeak/core/RawDataProcessors/ExtractChromatogramWindows.h>
#include <SmartPeak/core/RawDataProcessors/FitFeaturesEMG.h>
#include <SmartPeak/core/RawDataProcessors/FilterFeaturesRSDs.h>
#include <SmartPeak/core/RawDataProcessors/CheckFeaturesRSDs.h>
#include <SmartPeak/core/RawDataProcessors/FilterFeaturesBackgroundInterferences.h>
#include <SmartPeak/core/RawDataProcessors/CheckFeaturesBackgroundInterferences.h>
#include <SmartPeak/core/RawDataProcessors/ExtractSpectraWindows.h>
#include <SmartPeak/core/RawDataProcessors/MatchSpectra.h>
#include <SmartPeak/core/RawDataProcessors/MergeSpectra.h>
#include <SmartPeak/core/RawDataProcessors/Pick2DFeatures.h>
#include <SmartPeak/core/RawDataProcessors/Pick3DFeatures.h>
#include <SmartPeak/core/RawDataProcessors/SearchAccurateMass.h>
#include <SmartPeak/core/RawDataProcessors/MergeFeaturesMS1.h>
#include <SmartPeak/core/RawDataProcessors/MergeFeaturesMS1.h>
#include <SmartPeak/core/RawDataProcessors/LoadAnnotations.h>
#include <SmartPeak/core/RawDataProcessors/SearchSpectrumMS1.h>
#include <SmartPeak/core/RawDataProcessors/SearchSpectrumMS2.h>
#include <SmartPeak/core/RawDataProcessors/StoreAnnotations.h>
#include <SmartPeak/core/RawDataProcessors/StoreMSP.h>
#include <SmartPeak/core/RawDataProcessors/ClearData.h>
#include <SmartPeak/core/RawDataProcessors/StoreRawData.h>
#include <SmartPeak/core/RawDataProcessors/CalculateMDVs.h>
#include <SmartPeak/core/RawDataProcessors/IsotopicCorrections.h>
#include <SmartPeak/core/RawDataProcessors/CalculateIsotopicPurities.h>
#include <SmartPeak/core/RawDataProcessors/CalculateMDVAccuracies.h>
#include <SmartPeak/core/RawDataProcessors/ConstructTransitionsList.h>
#include <SmartPeak/core/RawDataProcessors/LoadParameters.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatureFiltersRDP.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatureQCsRDP.h>
#include <SmartPeak/core/RawDataProcessors/LoadValidationData.h>
#include <SmartPeak/core/RawDataProcessors/StoreFeatureFiltersRDP.h>
#include <SmartPeak/core/RawDataProcessors/StoreFeatureQCsRDP.h>
#include <SmartPeak/core/RawDataProcessors/PlotFeatures.h>
#include <SmartPeak/core/RawDataProcessors/ExtractSpectraNonTargeted.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadQuantitationMethods.h>

#include <OpenMS/ANALYSIS/OPENSWATH/DATAACCESS/DataAccessHelper.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>  // load traML
#include <OpenMS/FORMAT/FeatureXMLFile.h>  // load/store featureXML
#include <OpenMS/FORMAT/TraMLFile.h>
#include <OpenMS/FORMAT/MzMLFile.h>
#include <OpenMS/FORMAT/MSPGenericFile.h>

using namespace SmartPeak;
using namespace std;

void load_data(
  ParameterSet& params_1,
  ParameterSet& params_2
)
{
  Filenames filenames1, filenames2;
  filenames1.setFullPath("parameters", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core_tmpFix.csv"));
  filenames2.setFullPath("parameters", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_2_tmpFix.csv"));
  RawDataHandler rawDataHandler;
  LoadParameters loadParameters;
  loadParameters.process(rawDataHandler, {}, filenames1);
  params_1 = rawDataHandler.getParameters();
  rawDataHandler.clear();
  loadParameters.process(rawDataHandler, {}, filenames2);
  params_2 = rawDataHandler.getParameters();
}

/**
  ClearData Tests
*/
TEST(RawDataProcessor, constructorClearData)
{
  ClearData* ptrClearData = nullptr;
  ClearData* nullPointerClearData = nullptr;
  EXPECT_EQ(ptrClearData, nullPointerClearData);
}

TEST(RawDataProcessor, destructorClearData)
{
  ClearData* ptrClearData = nullptr;
  ptrClearData = new ClearData();
  delete ptrClearData;
}

TEST(RawDataProcessor, gettersClearData)
{
  ClearData processor;
  EXPECT_EQ(processor.getName(), "CLEAR_DATA");
}

TEST(RawDataProcessor, processorClearData)
{
  // Load all of the raw and processed data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  LoadRawData::extractMetaData(rawDataHandler);
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_test_1_io_FileReaderOpenMS.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Control
  EXPECT_GE(rawDataHandler.getExperiment().getChromatograms().size(), 0);
  EXPECT_GE(rawDataHandler.getFeatureMapHistory().size(), 0);
  EXPECT_TRUE(!rawDataHandler.getMetaData().getFilename().empty());

  ClearData clearData;
  clearData.process(rawDataHandler, params_1, filenames);
  EXPECT_EQ(rawDataHandler.getExperiment().getChromatograms().size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 0);
  EXPECT_TRUE(!rawDataHandler.getMetaData().getFilename().empty());
}

/**
  LoadRawData Tests
*/
TEST(RawDataProcessor, constructorLoadRawData)
{
  LoadRawData* ptrLoadRawData = nullptr;
  LoadRawData* nullPointerLoadRawData = nullptr;
  EXPECT_EQ(ptrLoadRawData, nullPointerLoadRawData);
}

TEST(RawDataProcessor, destructorLoadRawData)
{
  LoadRawData* ptrLoadRawData = nullptr;
  ptrLoadRawData = new LoadRawData();
  delete ptrLoadRawData;
}

TEST(RawDataProcessor, gettersLoadRawData)
{
  LoadRawData processor;
  EXPECT_EQ(processor.getName(), "LOAD_RAW_DATA");
}

TEST(RawDataProcessor, processorLoadRawData)
{  // TODO: add more tests once loadMSExperiment is split
  LoadRawData processor;

  // TEST CASE 1:  mzML with out baseline correction
  RawDataHandler rawDataHandler;
  std::vector<std::map<std::string, std::string>> params_tmp = { {
    {"name", "zero_baseline"},
    {"type", "bool"},
    {"value", "false"}
    } };
  ParameterSet params_I;
  params_I.addFunctionParameters(FunctionParameters("mzML", params_tmp));
  params_I.addFunctionParameters(FunctionParameters("ChromatogramExtractor"));
  params_I.addFunctionParameters(FunctionParameters("MRMMapping"));

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML"));
  processor.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getExperiment().getChromatograms();

  EXPECT_EQ(chromatograms1.size(), 2);

  EXPECT_NEAR(chromatograms1[0][0].getIntensity(), 3.0, 1e-3);
  EXPECT_NEAR(chromatograms1[0][1].getIntensity(), 4.0, 1e-3);
  EXPECT_NEAR(chromatograms1[0][2].getIntensity(), 6.0, 1e-3);
  EXPECT_NEAR(chromatograms1[0][3].getIntensity(), 19.0, 1e-3);
  EXPECT_NEAR(chromatograms1[0][4].getIntensity(), 1.0, 1e-3);

  EXPECT_NEAR(chromatograms1[1][0].getIntensity(), 3.0, 1e-3);
  EXPECT_NEAR(chromatograms1[1][1].getIntensity(), 4.0, 1e-3);
  EXPECT_NEAR(chromatograms1[1][2].getIntensity(), -2.0, 1e-3);
  EXPECT_NEAR(chromatograms1[1][3].getIntensity(), -8.0, 1e-3);
  EXPECT_NEAR(chromatograms1[1][4].getIntensity(), 1.0, 1e-3);

  // TEST CASE 2:  Chromeleon file format
  params_I.at("mzML")[0].setValueFromString("false");
  std::map<std::string, std::string> param_struct = 
    {
      {"name", "format"},
      {"type", "string"},
      {"value", "ChromeleonFile"}
    };
  auto param = Parameter(param_struct);
  params_I.at("mzML").addParameter(param);
  
  rawDataHandler.clear();
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_ChromeleonFile_10ug.txt"));
  processor.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms3 = rawDataHandler.getExperiment().getChromatograms();

  EXPECT_EQ(chromatograms3.size(), 1);
  EXPECT_EQ(chromatograms3[0].size(), 3301);

  EXPECT_NEAR(chromatograms3[0][0].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms3[0][600].getIntensity(), -503.815, 1e-3);
  EXPECT_NEAR(chromatograms3[0][1200].getIntensity(), -666.694, 1e-3);
  EXPECT_NEAR(chromatograms3[0][1800].getIntensity(), -232.843, 1e-3);
  EXPECT_NEAR(chromatograms3[0][2400].getIntensity(), -223.644, 1e-3);
  EXPECT_NEAR(chromatograms3[0][3300].getIntensity(), 126.958, 1e-3);
}

TEST(RawDataProcessor, extractMetaData)
{
  // Pre-requisites: load the parameters
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  EXPECT_EQ(params_1.size(), 31);
  EXPECT_EQ(params_2.size(), 28);
  RawDataHandler rawDataHandler;

  // Pre-requisites: load the transitions and raw data
  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, {}, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);

  // Test 1
  LoadRawData::extractMetaData(rawDataHandler);

  const MetaDataHandler& metaDataHandler = rawDataHandler.getMetaData();

  string filename = metaDataHandler.getFilename();
  filename = filename.substr(filename.find("src/tests")); // otherwise it would contain /home/username/SmartPeak2/ on Linux
  EXPECT_EQ(filename, "src/tests/class_tests/smartpeak/data/RawDataProcessor_mzML_1.mzML");
  EXPECT_EQ(metaDataHandler.getSampleName(), "150601_0_BloodProject01_PLT_QC_Broth-1");

  EXPECT_EQ(metaDataHandler.proc_method_name, "Analyst");
  EXPECT_EQ(metaDataHandler.instrument, "QTRAP 5500");
  // EXPECT_EQ(rawDataHandler.acq_method_name, "");
  EXPECT_EQ(metaDataHandler.operator_name, "");
  EXPECT_EQ(metaDataHandler.acquisition_date_and_time.tm_mday, 10);
  EXPECT_EQ(metaDataHandler.acquisition_date_and_time.tm_mon, 06);
  EXPECT_EQ(metaDataHandler.acquisition_date_and_time.tm_year, 2015);
  EXPECT_EQ(metaDataHandler.acquisition_date_and_time.tm_hour, 01);
  EXPECT_EQ(metaDataHandler.acquisition_date_and_time.tm_min, 14);
  EXPECT_EQ(metaDataHandler.acquisition_date_and_time.tm_sec, 10);
}

/**
  StoreRawData Tests
*/
TEST(RawDataProcessor, constructorStoreRawData)
{
  StoreRawData* ptrStoreRawData = nullptr;
  StoreRawData* nullPointerStoreRawData = nullptr;
  EXPECT_EQ(ptrStoreRawData, nullPointerStoreRawData);
}

TEST(RawDataProcessor, destructorStoreRawData)
{
  StoreRawData* ptrStoreRawData = nullptr;
  ptrStoreRawData = new StoreRawData();
  delete ptrStoreRawData;
}

TEST(RawDataProcessor, gettersStoreRawData)
{
  StoreRawData processor;
  EXPECT_EQ(processor.getName(), "STORE_RAW_DATA");
}

TEST(RawDataProcessor, processStoreRawData)
{
  // no tests, it wraps OpenMS store function
}

/**
  MapChromatograms Tests
*/
TEST(RawDataProcessor, constructorMapChromatograms)
{
  MapChromatograms* ptrMapChromatograms = nullptr;
  MapChromatograms* nullPointerMapChromatograms = nullptr;
  EXPECT_EQ(ptrMapChromatograms, nullPointerMapChromatograms);
}

TEST(RawDataProcessor, destructorMapChromatograms)
{
  MapChromatograms* ptrMapChromatograms = nullptr;
  ptrMapChromatograms = new MapChromatograms();
  delete ptrMapChromatograms;
}

TEST(RawDataProcessor, gettersMapChromatograms)
{
  MapChromatograms processor;
  EXPECT_EQ(processor.getName(), "MAP_CHROMATOGRAMS");
}

TEST(RawDataProcessor, processorMapChromatograms)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Test map chromatograms
  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getChromatogramMap().getChromatograms();

  EXPECT_EQ(chromatograms1.size(), 324);

  EXPECT_EQ(chromatograms1.front().getNativeID(), "arg-L.arg-L_1.Heavy");
  EXPECT_NEAR(chromatograms1.front().getPrecursor().getMZ(), 179, 1e-3);
  EXPECT_NEAR(chromatograms1.front().getProduct().getMZ(), 136, 1e-3);

  EXPECT_EQ(chromatograms1.back().getNativeID(), "nadph.nadph_2.Light");
  EXPECT_NEAR(chromatograms1.back().getPrecursor().getMZ(), 744, 1e-3);
  EXPECT_NEAR(chromatograms1.back().getProduct().getMZ(), 79, 1e-3);
}

/**
  ZeroChromatogramBaseline Tests
*/
TEST(RawDataProcessor, constructorZeroChromatogramBaseline)
{
  ZeroChromatogramBaseline* ptrZeroChromatogramBaseline = nullptr;
  ZeroChromatogramBaseline* nullPointerZeroChromatogramBaseline = nullptr;
  EXPECT_EQ(ptrZeroChromatogramBaseline, nullPointerZeroChromatogramBaseline);
}

TEST(RawDataProcessor, destructorZeroChromatogramBaseline)
{
  ZeroChromatogramBaseline* ptrZeroChromatogramBaseline = nullptr;
  ptrZeroChromatogramBaseline = new ZeroChromatogramBaseline();
  delete ptrZeroChromatogramBaseline;
}

TEST(RawDataProcessor, gettersZeroChromatogramBaseline)
{
  ZeroChromatogramBaseline processor;
  EXPECT_EQ(processor.getName(), "ZERO_CHROMATOGRAM_BASELINE");
}

TEST(RawDataProcessor, processorZeroChromatogramBaseline)
{
  // TEST CASE 1:  mzML with baseline correction
  RawDataHandler rawDataHandler;
  std::vector<std::map<std::string, std::string>> params_tmp = { { // Must be initialized step by step due to Compiler Error C2665 on Windows...
    {"name", "zero_baseline"},
    {"type", "bool"},
    {"value", "true"}
    } };
  FunctionParameters mzML_params("mzML", params_tmp);
  ParameterSet params_I;
  params_I.addFunctionParameters(mzML_params);
  params_I.addFunctionParameters(FunctionParameters("ChromatogramExtractor"));

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML"));
  LoadRawData processor;
  processor.process(rawDataHandler, params_I, filenames);
  rawDataHandler.setChromatogramMap(rawDataHandler.getExperiment()); // Avoiding the mapping step for testing purposes
  ZeroChromatogramBaseline zeroChromBase;
  zeroChromBase.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms2 = rawDataHandler.getChromatogramMap().getChromatograms();
  EXPECT_EQ(chromatograms2.size(), 2);
  EXPECT_NEAR(chromatograms2[0][0].getIntensity(), 2.0, 1e-3);
  EXPECT_NEAR(chromatograms2[0][1].getIntensity(), 3.0, 1e-3);
  EXPECT_NEAR(chromatograms2[0][2].getIntensity(), 5.0, 1e-3);
  EXPECT_NEAR(chromatograms2[0][3].getIntensity(), 18.0, 1e-3);
  EXPECT_NEAR(chromatograms2[0][4].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms2[1][0].getIntensity(), 11.0, 1e-3);
  EXPECT_NEAR(chromatograms2[1][1].getIntensity(), 12.0, 1e-3);
  EXPECT_NEAR(chromatograms2[1][2].getIntensity(), 6.0, 1e-3);
  EXPECT_NEAR(chromatograms2[1][3].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms2[1][4].getIntensity(), 9.0, 1e-3);

  // TEST CASE 2:  Chromeleon file format with baseline correction
  std::vector<std::map<std::string, std::string>> params = { {
      {"name", "format"},
      {"type", "string"},
      {"value", "ChromeleonFile"}
  }};
  params_I.addFunctionParameters(FunctionParameters("mzML", params));
  rawDataHandler.clear();
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_ChromeleonFile_10ug.txt"));
  processor.process(rawDataHandler, params_I, filenames);
  rawDataHandler.setChromatogramMap(rawDataHandler.getExperiment()); // Avoiding the mapping step for testing purposes
  zeroChromBase.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms4 = rawDataHandler.getChromatogramMap().getChromatograms();
  EXPECT_EQ(chromatograms4.size(), 1);
  EXPECT_EQ(chromatograms4[0].size(), 3301);
  EXPECT_NEAR(chromatograms4[0][0].getIntensity(), 1004.634, 1e-3);
  EXPECT_NEAR(chromatograms4[0][600].getIntensity(), 500.819, 1e-3);
  EXPECT_NEAR(chromatograms4[0][1200].getIntensity(), 337.94, 1e-3);
  EXPECT_NEAR(chromatograms4[0][1800].getIntensity(), 771.791, 1e-3);
  EXPECT_NEAR(chromatograms4[0][2400].getIntensity(), 780.99, 1e-3);
  EXPECT_NEAR(chromatograms4[0][3300].getIntensity(), 1131.592, 1e-3);
}

/**
  ExtractChromatogramWindows Tests
*/
TEST(RawDataProcessor, constructorExtractChromatogramWindows)
{
  ExtractChromatogramWindows* ptrExtractChromatogramWindows = nullptr;
  ExtractChromatogramWindows* nullPointerExtractChromatogramWindows = nullptr;
  EXPECT_EQ(ptrExtractChromatogramWindows, nullPointerExtractChromatogramWindows);
}

TEST(RawDataProcessor, destructorExtractChromatogramWindows)
{
  ExtractChromatogramWindows* ptrExtractChromatogramWindows = nullptr;
  ptrExtractChromatogramWindows = new ExtractChromatogramWindows();
  delete ptrExtractChromatogramWindows;
}

TEST(RawDataProcessor, gettersExtractChromatogramWindows)
{
  ExtractChromatogramWindows processor;
  EXPECT_EQ(processor.getName(), "EXTRACT_CHROMATOGRAM_WINDOWS");
}

TEST(RawDataProcessor, processorExtractChromatogramWindows)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Control
  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getChromatogramMap().getChromatograms();
  EXPECT_EQ(chromatograms1.size(), 324);
  EXPECT_NEAR(chromatograms1.front()[0].getIntensity(), 158.0, 1e-3);
  EXPECT_NEAR(chromatograms1.front()[0].getMZ(), 38.621, 1e-3);
  EXPECT_NEAR(chromatograms1.front()[1].getIntensity(), 211, 1e-3);
  EXPECT_NEAR(chromatograms1.front()[1].getMZ(), 38.7209999, 1e-3);
  EXPECT_NEAR(chromatograms1.front()[2].getIntensity(), 158, 1e-3);
  EXPECT_NEAR(chromatograms1.front()[2].getMZ(), 38.82, 1e-3);
  EXPECT_NEAR(chromatograms1.back()[0].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms1.back()[0].getMZ(), 912.233, 1e-3);
  EXPECT_NEAR(chromatograms1.back()[1].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms1.back()[1].getMZ(), 913.1870, 1e-3);
  EXPECT_NEAR(chromatograms1.back()[2].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms1.back()[2].getMZ(), 914.139, 1e-3);

  // Test window extraction
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_extractChromWindowTest_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, {}, filenames);

  ExtractChromatogramWindows extractChromWin;
  extractChromWin.process(rawDataHandler, {}, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms2 = rawDataHandler.getChromatogramMap().getChromatograms();
  EXPECT_EQ(chromatograms2.size(), 324);
  EXPECT_NEAR(chromatograms2.front()[0].getIntensity(), 158.0, 1e-3); // No filtering; within the RT window
  EXPECT_NEAR(chromatograms2.front()[0].getMZ(), 38.621, 1e-3);
  EXPECT_NEAR(chromatograms2.front()[1].getIntensity(), 211, 1e-3);
  EXPECT_NEAR(chromatograms2.front()[1].getMZ(), 38.7209999, 1e-3);
  EXPECT_NEAR(chromatograms2.front()[2].getIntensity(), 158, 1e-3);
  EXPECT_NEAR(chromatograms2.front()[2].getMZ(), 38.82, 1e-3);
  EXPECT_NEAR(chromatograms2.back()[0].getIntensity(), 0.0, 1e-3); // Filtering; outside of the RT window
  EXPECT_NEAR(chromatograms2.back()[0].getMZ(), 951.20299, 1e-3);
  EXPECT_NEAR(chromatograms2.back()[1].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms2.back()[1].getMZ(), 952.31299, 1e-3);
  EXPECT_NEAR(chromatograms2.back()[2].getIntensity(), 0.0, 1e-3);
  EXPECT_NEAR(chromatograms2.back()[2].getMZ(), 953.40699, 1e-3);
}

/**
  ExtractSpectraWindows Tests
*/
TEST(RawDataProcessor, constructorExtractSpectraWindows)
{
  ExtractSpectraWindows* ptrExtractSpectraWindows = nullptr;
  ExtractSpectraWindows* nullPointerExtractSpectraWindows = nullptr;
  EXPECT_EQ(ptrExtractSpectraWindows, nullPointerExtractSpectraWindows);
}

TEST(RawDataProcessor, destructorExtractSpectraWindows)
{
  ExtractSpectraWindows* ptrExtractSpectraWindows = nullptr;
  ptrExtractSpectraWindows = new ExtractSpectraWindows();
  delete ptrExtractSpectraWindows;
}

TEST(RawDataProcessor, gettersExtractSpectraWindows)
{
  ExtractSpectraWindows processor;
  EXPECT_EQ(processor.getName(), "EXTRACT_SPECTRA_WINDOWS");
}

TEST(RawDataProcessor, processorExtractSpectraWindows)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Control
  const vector<OpenMS::MSSpectrum>& spectra1 = rawDataHandler.getExperiment().getSpectra();
  EXPECT_EQ(spectra1.size(), 873);
  EXPECT_NEAR(spectra1.front().getRT(), 0.56525150400000002, 1e-3);
  EXPECT_NEAR(spectra1.back().getRT(), 300.56175465600001, 1e-3);

  // Test window extraction
  ExtractSpectraWindows extractSpectraWin;
  extractSpectraWin.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSSpectrum>& spectra2 = rawDataHandler.getExperiment().getSpectra();
  EXPECT_EQ(spectra2.size(), 173);
  EXPECT_NEAR(spectra2.front().getRT(), 0.56525150400000002, 1e-3);
  EXPECT_NEAR(spectra2.back().getRT(), 59.74130393598, 1e-3);
}

/**
  MergeSpectra Tests
*/
TEST(RawDataProcessor, constructorMergeSpectra)
{
  MergeSpectra* ptrMergeSpectra = nullptr;
  MergeSpectra* nullPointerMergeSpectra = nullptr;
  EXPECT_EQ(ptrMergeSpectra, nullPointerMergeSpectra);
}

TEST(RawDataProcessor, destructorMergeSpectra)
{
  MergeSpectra* ptrMergeSpectra = nullptr;
  ptrMergeSpectra = new MergeSpectra();
  delete ptrMergeSpectra;
}

TEST(RawDataProcessor, gettersMergeSpectra)
{
  MergeSpectra processor;
  EXPECT_EQ(processor.getName(), "MERGE_SPECTRA");
}

TEST(RawDataProcessor, processorMergeSpectra)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Control
  const vector<OpenMS::MSSpectrum>& spectra1 = rawDataHandler.getExperiment().getSpectra();
  EXPECT_EQ(spectra1.size(), 873);

  // Test merge spectra
  MergeSpectra mergeSpectra;
  mergeSpectra.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSSpectrum>& spectra2 = rawDataHandler.getExperiment().getSpectra();
  EXPECT_EQ(spectra2.size(), 1);
  EXPECT_NEAR(spectra2.front().getRT(), -1, 1e-3);
  EXPECT_EQ(spectra2.front().size(), 240);
  EXPECT_EQ(spectra2.front().getNativeID(), "MergeSpectra");
  EXPECT_EQ(spectra2.front().front().getMZ(), 109.95009243262952);
  EXPECT_EQ(spectra2.front().back().getMZ(), 109.99988410050186);
  EXPECT_EQ(spectra2.front().front().getIntensity(), 0);
  EXPECT_EQ(spectra2.front().back().getIntensity(), 3236006.75);
}

TEST(RawDataProcessor, processorMergeSpectraZeroPeak)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest_SpecWithZeroPeak.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Control
  const vector<OpenMS::MSSpectrum>& spectra1 = rawDataHandler.getExperiment().getSpectra();
  EXPECT_EQ(spectra1.size(), 873);

  // Test merge spectra
  MergeSpectra mergeSpectra;
  mergeSpectra.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSSpectrum>& spectra2 = rawDataHandler.getExperiment().getSpectra();
  EXPECT_EQ(spectra2.size(), 1);
  EXPECT_NEAR(spectra2.front().getRT(), -1, 1e-3);
  EXPECT_EQ(spectra2.front().size(), 0);
  EXPECT_EQ(spectra2.front().getNativeID(), "MergeSpectra");
  EXPECT_NEAR(spectra2[0].getMetaValue("lowest observed m/z"), 109.99865014868708, 1e-6);
  EXPECT_NEAR(spectra2[0].getMetaValue("highest observed m/z"), 109.99994568761217, 1e-6);
}

/**
  LoadFeatures Tests
*/
TEST(RawDataProcessor, constructorLoadFeatures)
{
  LoadFeatures* ptrLoadFeatures = nullptr;
  LoadFeatures* nullPointerLoadFeatures = nullptr;
  EXPECT_EQ(ptrLoadFeatures, nullPointerLoadFeatures);
}

TEST(RawDataProcessor, destructorLoadFeatures)
{
  LoadFeatures* ptrLoadFeatures = nullptr;
  ptrLoadFeatures = new LoadFeatures();
  delete ptrLoadFeatures;
}

TEST(RawDataProcessor, gettersLoadFeatures)
{
  LoadFeatures processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURES");
}

TEST(RawDataProcessor, processLoadFeatures)
{
  Filenames filenames;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_test_1_io_FileReaderOpenMS.featureXML"));
  RawDataHandler rawDataHandler;
  rawDataHandler.getMetaData().setFilename("filename");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, {}, filenames);

  const OpenMS::FeatureMap& fm = rawDataHandler.getFeatureMap(); // Test feature_map
  EXPECT_EQ(fm.size(), 481);
  OpenMS::StringList filename_test;
  fm.getPrimaryMSRunPath(filename_test);
  EXPECT_EQ(filename_test.at(0), "filename");

  EXPECT_NEAR(static_cast<double>(fm[0].getSubordinates()[0].getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_NEAR(static_cast<double>(fm[0].getSubordinates()[0].getRT()), 15.8944563381195, 1e-6);
  EXPECT_EQ(fm[0].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(fm[0].getMetaValue("peak_apices_sum")), 583315.0, 1e-6);
  EXPECT_EQ(fm[0].getMetaValue("PeptideRef").toString(), "23dpg");

  EXPECT_NEAR(static_cast<double>(fm[1].getSubordinates()[0].getMetaValue("peak_apex_int")), 3436.0, 1e-6);
  EXPECT_NEAR(static_cast<double>(fm[1].getSubordinates()[0].getRT()), 16.2997193464915, 1e-6);
  EXPECT_EQ(fm[1].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(fm[1].getMetaValue("peak_apices_sum")), 13859.0, 1e-6);
  EXPECT_EQ(fm[1].getMetaValue("PeptideRef").toString(), "23dpg");

  const OpenMS::FeatureMap& fmh = rawDataHandler.getFeatureMapHistory(); // Test feature_map_history
  EXPECT_EQ(fmh.size(), 481);
  fm.getPrimaryMSRunPath(filename_test);
  EXPECT_EQ(filename_test.at(0), "filename");

  EXPECT_NEAR(static_cast<double>(fmh[0].getSubordinates()[0].getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_NEAR(static_cast<double>(fmh[0].getSubordinates()[0].getRT()), 15.8944563381195, 1e-6);
  EXPECT_EQ(fmh[0].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(fmh[0].getMetaValue("peak_apices_sum")), 583315.0, 1e-6);
  EXPECT_EQ(fmh[0].getMetaValue("PeptideRef").toString(), "23dpg");
  EXPECT_TRUE(fmh[0].getSubordinates()[0].getMetaValue("used_").toBool());

  EXPECT_NEAR(static_cast<double>(fmh[1].getSubordinates()[0].getMetaValue("peak_apex_int")), 3436.0, 1e-6);
  EXPECT_NEAR(static_cast<double>(fmh[1].getSubordinates()[0].getRT()), 16.2997193464915, 1e-6);
  EXPECT_EQ(fmh[1].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(fmh[1].getMetaValue("peak_apices_sum")), 13859.0, 1e-6);
  EXPECT_EQ(fmh[1].getMetaValue("PeptideRef").toString(), "23dpg");
  EXPECT_TRUE(fmh[1].getSubordinates()[0].getMetaValue("used_").toBool());
}

/**
  StoreFeatures Tests
*/
TEST(RawDataProcessor, constructorStoreFeatures)
{
  StoreFeatures* ptrStoreFeatures = nullptr;
  StoreFeatures* nullPointerStoreFeatures = nullptr;
  EXPECT_EQ(ptrStoreFeatures, nullPointerStoreFeatures);
}

TEST(RawDataProcessor, destructorStoreFeatures)
{
  StoreFeatures* ptrStoreFeatures = nullptr;
  ptrStoreFeatures = new StoreFeatures();
  delete ptrStoreFeatures;
}

TEST(RawDataProcessor, gettersStoreFeatures)
{
  StoreFeatures processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURES");
}

TEST(RawDataProcessor, processStoreFeatures)
{
  // no tests, it wraps OpenMS store function
}

/**
  LoadAnnotations Tests
*/
TEST(RawDataProcessor, constructorLoadAnnotations)
{
  LoadAnnotations* ptrLoadAnnotations = nullptr;
  LoadAnnotations* nullPointerLoadAnnotations = nullptr;
  EXPECT_EQ(ptrLoadAnnotations, nullPointerLoadAnnotations);
}

TEST(RawDataProcessor, destructorLoadAnnotations)
{
  LoadAnnotations* ptrLoadAnnotations = nullptr;
  ptrLoadAnnotations = new LoadAnnotations();
  delete ptrLoadAnnotations;
}

TEST(RawDataProcessor, gettersLoadAnnotations)
{
  LoadAnnotations processor;
  EXPECT_EQ(processor.getName(), "LOAD_ANNOTATIONS");
}

TEST(RawDataProcessor, processLoadAnnotations)
{
  Filenames filenames;
  filenames.setFullPath("mzTab_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest.mzTab"));
  RawDataHandler rawDataHandler;
  LoadAnnotations loadAnnotations;
  loadAnnotations.process(rawDataHandler, {}, filenames);

  EXPECT_EQ(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().size(), 108);
  EXPECT_NEAR(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().calc_mass_to_charge.get(), 500.02851705499575, 1e-6);
  EXPECT_EQ(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().chemical_formula.get()), "C82H156O17P2");
  EXPECT_EQ(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().identifier.get().at(0).get(), "HMDB:HMDB0073108");
  EXPECT_NEAR(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().back().calc_mass_to_charge.get(), 799.94828338599575, 1e-6);
  EXPECT_EQ(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().back().chemical_formula.get()), "C76H135N3O29");
  EXPECT_EQ(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().back().identifier.get().at(0).get(), "HMDB:HMDB0011867");
}

/**
  StoreAnnotations Tests
*/
TEST(RawDataProcessor, constructorStoreAnnotations)
{
  StoreAnnotations* ptrStoreAnnotations = nullptr;
  StoreAnnotations* nullPointerStoreAnnotations = nullptr;
  EXPECT_EQ(ptrStoreAnnotations, nullPointerStoreAnnotations);
}

TEST(RawDataProcessor, destructorStoreAnnotations)
{
  StoreAnnotations* ptrStoreAnnotations = nullptr;
  ptrStoreAnnotations = new StoreAnnotations();
  delete ptrStoreAnnotations;
}

TEST(RawDataProcessor, gettersStoreAnnotations)
{
  StoreAnnotations processor;
  EXPECT_EQ(processor.getName(), "STORE_ANNOTATIONS");
}

TEST(RawDataProcessor, processStoreAnnotations)
{
  // no tests, it wraps OpenMS store function
}

/**
  LoadTransitions Tests
*/
TEST(RawDataProcessor, constructorLoadTransitions)
{
  LoadTransitions* ptrLoadTransitions = nullptr;
  LoadTransitions* nullPointerLoadTransitions = nullptr;
  EXPECT_EQ(ptrLoadTransitions, nullPointerLoadTransitions);
}

TEST(RawDataProcessor, destructorLoadTransitions)
{
  LoadTransitions* ptrLoadTransitions = nullptr;
  ptrLoadTransitions = new LoadTransitions();
  delete ptrLoadTransitions;
}

TEST(RawDataProcessor, gettersLoadTransitions)
{
  LoadTransitions processor;
  EXPECT_EQ(processor.getName(), "LOAD_TRANSITIONS");
}

TEST(RawDataProcessor, processLoadTransitions_csv)
{
  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  RawDataHandler rawDataHandler;
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, {}, filenames);
  const std::vector<OpenMS::ReactionMonitoringTransition>& t = rawDataHandler.getTargetedExperiment().getTransitions();

  EXPECT_EQ(t.size(), 324);

  EXPECT_EQ(t[0].getPeptideRef(), "arg-L");
  EXPECT_NEAR(t[0].getPrecursorMZ(), 179.0, 1e-6);
  EXPECT_NEAR(t[0].getProductMZ(), 136.0, 1e-6);

  EXPECT_EQ(t[10].getPeptideRef(), "citr-L");
  EXPECT_NEAR(t[10].getPrecursorMZ(), 180.0, 1e-6);
  EXPECT_NEAR(t[10].getProductMZ(), 136.0, 1e-6);

  EXPECT_EQ(t[19].getPeptideRef(), "Lcystin");
  EXPECT_NEAR(t[19].getPrecursorMZ(), 239.0, 1e-6);
  EXPECT_NEAR(t[19].getProductMZ(), 120.0, 1e-6);
}

TEST(RawDataProcessor, processLoadTransitions_traML)
{
  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("LoadTransitions_test.TraML"));
  RawDataHandler rawDataHandler;
  LoadTransitions loadTransitions;
  map<std::string, vector<map<string, string>>> params_struct({
    {"LoadTransitions", {
      { {"name", "format"}, {"type", "string"}, {"value", "traML"} },
    }}
    });
  ParameterSet params(params_struct);
  loadTransitions.process(rawDataHandler, params, filenames);
  const std::vector<OpenMS::ReactionMonitoringTransition>& t = rawDataHandler.getTargetedExperiment().getTransitions();

  EXPECT_EQ(t.size(), 324);

  EXPECT_EQ(t[0].getPeptideRef(), "arg-L");
  EXPECT_NEAR(t[0].getPrecursorMZ(), 179.0, 1e-6);
  EXPECT_NEAR(t[0].getProductMZ(), 136.0, 1e-6);

  EXPECT_EQ(t[10].getPeptideRef(), "citr-L");
  EXPECT_NEAR(t[10].getPrecursorMZ(), 180.0, 1e-6);
  EXPECT_NEAR(t[10].getProductMZ(), 136.0, 1e-6);

  EXPECT_EQ(t[19].getPeptideRef(), "Lcystin");
  EXPECT_NEAR(t[19].getPrecursorMZ(), 239.0, 1e-6);
  EXPECT_NEAR(t[19].getProductMZ(), 120.0, 1e-6);
}

/**
  LoadFeatureFiltersRDP Tests
*/
TEST(RawDataProcessor, constructorLoadFeatureFilters)
{
  LoadFeatureFiltersRDP* ptrLoadFeatureFilters = nullptr;
  LoadFeatureFiltersRDP* nullPointerLoadFeatureFilters = nullptr;
  EXPECT_EQ(ptrLoadFeatureFilters, nullPointerLoadFeatureFilters);
}

TEST(RawDataProcessor, destructorLoadFeatureFilters)
{
  LoadFeatureFiltersRDP* ptrLoadFeatureFilters = nullptr;
  ptrLoadFeatureFilters = new LoadFeatureFiltersRDP();
  delete ptrLoadFeatureFilters;
}

TEST(RawDataProcessor, gettersLoadFeatureFilters)
{
  LoadFeatureFiltersRDP processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_FILTERS");
}

TEST(RawDataProcessor, processLoadFeatureFilters)
{
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureFilter();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureQCsRDP Tests
*/
TEST(RawDataProcessor, constructorLoadFeatureQCs)
{
  LoadFeatureQCsRDP* ptrLoadFeatureQCs = nullptr;
  LoadFeatureQCsRDP* nullPointerLoadFeatureQCs = nullptr;
  EXPECT_EQ(ptrLoadFeatureQCs, nullPointerLoadFeatureQCs);
}

TEST(RawDataProcessor, destructorLoadFeatureQCs)
{
  LoadFeatureQCsRDP* ptrLoadFeatureQCs = nullptr;
  ptrLoadFeatureQCs = new LoadFeatureQCsRDP();
  delete ptrLoadFeatureQCs;
}

TEST(RawDataProcessor, gettersLoadFeatureQCs)
{
  LoadFeatureQCsRDP processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_QCS");
}

TEST(RawDataProcessor, processLoadFeatureQCs)
{
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureQC();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureFiltersRDP Tests
*/
TEST(RawDataProcessor, constructorStoreFeatureFilters)
{
  StoreFeatureFiltersRDP* ptrStoreFeatureFilters = nullptr;
  StoreFeatureFiltersRDP* nullPointerStoreFeatureFilters = nullptr;
  EXPECT_EQ(ptrStoreFeatureFilters, nullPointerStoreFeatureFilters);
}

TEST(RawDataProcessor, destructorStoreFeatureFilters)
{
  StoreFeatureFiltersRDP* ptrStoreFeatureFilters = nullptr;
  ptrStoreFeatureFilters = new StoreFeatureFiltersRDP();
  delete ptrStoreFeatureFilters;
}

TEST(RawDataProcessor, gettersStoreFeatureFilters)
{
  StoreFeatureFiltersRDP processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_FILTERS");
}

TEST(RawDataProcessor, processStoreFeatureFilters)
{
  RawDataHandler rawDataHandler, rawDataHandler_test;

  Filenames filenames;
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, {}, filenames);
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureFiltersRDP storeFeatureFilters;
  storeFeatureFilters.process(rawDataHandler, {}, filenames);
  loadFeatureFilters.process(rawDataHandler_test, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureFilter();
  const OpenMS::MRMFeatureQC& fQC_test = rawDataHandler_test.getFeatureFilter();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureQCsRDP Tests
*/
TEST(RawDataProcessor, constructorStoreFeatureQCs)
{
  StoreFeatureQCsRDP* ptrStoreFeatureQCs = nullptr;
  StoreFeatureQCsRDP* nullPointerStoreFeatureQCs = nullptr;
  EXPECT_EQ(ptrStoreFeatureQCs, nullPointerStoreFeatureQCs);
}

TEST(RawDataProcessor, destructorStoreFeatureQCs)
{
  StoreFeatureQCsRDP* ptrStoreFeatureQCs = nullptr;
  ptrStoreFeatureQCs = new StoreFeatureQCsRDP();
  delete ptrStoreFeatureQCs;
}

TEST(RawDataProcessor, gettersStoreFeatureQCs)
{
  StoreFeatureQCsRDP processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_QCS");
}

TEST(RawDataProcessor, processStoreFeatureQCs)
{
  RawDataHandler rawDataHandler, rawDataHandler_test;

  Filenames filenames;
  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, {}, filenames);
  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureQCsRDP storeFeatureQCs;
  storeFeatureQCs.process(rawDataHandler, {}, filenames);
  loadFeatureQCs.process(rawDataHandler_test, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureQC();
  const OpenMS::MRMFeatureQC& fQC_test = rawDataHandler_test.getFeatureQC();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadValidationData Tests
*/
TEST(RawDataProcessor, constructorLoadValidationData)
{
  LoadValidationData* ptrLoadValidationData = nullptr;
  LoadValidationData* nullPointerLoadValidationData = nullptr;
  EXPECT_EQ(ptrLoadValidationData, nullPointerLoadValidationData);
}

TEST(RawDataProcessor, destructorLoadValidationData)
{
  LoadValidationData* ptrLoadValidationData = nullptr;
  ptrLoadValidationData = new LoadValidationData();
  delete ptrLoadValidationData;
}

TEST(RawDataProcessor, gettersLoadValidationData)
{
  LoadValidationData processor;
  EXPECT_EQ(processor.getName(), "LOAD_VALIDATION_DATA");
}

TEST(RawDataProcessor, processLoadValidationData)
{
  Filenames filenames;
  filenames.setFullPath("referenceData", SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv"));
  RawDataHandler rawDataHandler;

  LoadValidationData loadValidationData;
  loadValidationData.process(rawDataHandler, {}, filenames);
  const std::vector<std::map<std::string, CastValue>>& ref_data = rawDataHandler.getReferenceData();

  EXPECT_EQ(ref_data.size(), 179);
  EXPECT_EQ(ref_data[0].at("component_name").s_, "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(ref_data[0].at("area").f_, 932543.098, 1e-1);
  EXPECT_NEAR(ref_data[0].at("retention_time").f_, static_cast<float>(15.89495171), 1e-1);
  EXPECT_EQ(ref_data[0].at("acquisition_date_and_time").s_, "09-06-2015 17:14");
  // TODO: Should we just use double instead of float? I had to go down to -1 to make the test pass
  EXPECT_EQ(ref_data[178].at("component_name").s_, "xan.xan_1.Light");
  EXPECT_NEAR(ref_data[178].at("area").f_, 206951.3035, 1e-2);
  EXPECT_NEAR(ref_data[178].at("retention_time").f_, static_cast<float>(1.492980468), 1e-1);
  EXPECT_EQ(ref_data[178].at("acquisition_date_and_time").s_, "09-06-2015 17:14");
}

/**
  LoadParameters Tests
*/
TEST(RawDataProcessor, constructorLoadParameters)
{
  LoadParameters* ptrLoadParameters = nullptr;
  LoadParameters* nullPointerLoadParameters = nullptr;
  EXPECT_EQ(ptrLoadParameters, nullPointerLoadParameters);
}

TEST(RawDataProcessor, destructorLoadParameters)
{
  LoadParameters* ptrLoadParameters = nullptr;
  ptrLoadParameters = new LoadParameters();
  delete ptrLoadParameters;
}

TEST(RawDataProcessor, gettersLoadParameters)
{
  LoadParameters processor;
  EXPECT_EQ(processor.getName(), "LOAD_PARAMETERS");
}

TEST(RawDataProcessor, processLoadParameters)
{
  // no tests, it calls ParametersParser::read and LoadParameters::sanitizeRawDataProcessorParameters
}

TEST(RawDataProcessor, sanitizeRawDataProcessorParameters)
{
  RawDataHandler rawDataHandler;

  vector<map<string, string>> function_params = {
    {
      {"name", "elem1"},
      {"value", "value1"}
    },
    {
      {"name", "elem2"},
      {"value", "value2"}
    },
    {
      {"name", "elem3"},
      {"value", "value3"}
    }
  };
  ParameterSet params;
  params.addFunctionParameters(FunctionParameters("SequenceSegmentPlotter", function_params));

  LoadParameters loadParameters;
  loadParameters.sanitizeParameters(params);
  EXPECT_EQ(params.size(), 26);
  EXPECT_EQ(params.count("SequenceSegmentPlotter"), 1);
  EXPECT_EQ(params.count("FeaturePlotter"), 1);
  EXPECT_EQ(params.count("AbsoluteQuantitation"), 1);
  EXPECT_EQ(params.count("mzML"), 1);
  EXPECT_EQ(params.count("MRMMapping"), 1);
  EXPECT_EQ(params.count("ChromatogramExtractor"), 1);
  EXPECT_EQ(params.count("MRMFeatureFinderScoring"), 1);
  EXPECT_EQ(params.count("MRMFeatureFilter.filter_MRMFeatures"), 1);
  EXPECT_EQ(params.count("MRMFeatureSelector.select_MRMFeatures_qmip"), 1);
  EXPECT_EQ(params.count("MRMFeatureSelector.schedule_MRMFeatures_qmip"), 0);
  EXPECT_EQ(params.count("MRMFeatureSelector.select_MRMFeatures_score"), 1);
  EXPECT_EQ(params.count("ReferenceDataMethods.getAndProcess_referenceData_samples"), 1);
  EXPECT_EQ(params.count("MRMFeatureValidator.validate_MRMFeatures"), 1);
  EXPECT_EQ(params.count("MRMFeatureFilter.filter_MRMFeatures.qc"), 1);
  EXPECT_EQ(params.count("SequenceProcessor"), 1);
  EXPECT_EQ(params.count("FIAMS"), 1);
  EXPECT_EQ(params.count("Pick2DFeatures"), 1);
  EXPECT_EQ(params.count("Pick3DFeatures"), 1);
  EXPECT_EQ(params.count("AccurateMassSearchEngine"), 1);
  EXPECT_EQ(params.count("MergeInjections"), 1);
  EXPECT_EQ(params.at("SequenceSegmentPlotter").size(), 3);
  EXPECT_EQ(params.at("MRMFeatureFilter.filter_MRMFeatures.qc").size(), 0);
  EXPECT_EQ(params.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences").size(), 0);
  EXPECT_EQ(params.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc").size(), 0);
  EXPECT_EQ(params.at("MRMFeatureFilter.filter_MRMFeaturesRSDs").size(), 0);
  EXPECT_EQ(params.at("MRMFeatureFilter.filter_MRMFeaturesRSDs.qc").size(), 0);
  EXPECT_EQ(params.at("SequenceProcessor").size(), 0);
  EXPECT_EQ(params.at("FIAMS").size(), 0);
  EXPECT_EQ(params.at("Pick2DFeatures").size(), 0);
  EXPECT_EQ(params.at("Pick3DFeatures").size(), 0);
  EXPECT_EQ(params.at("AccurateMassSearchEngine").size(), 0);
  EXPECT_EQ(params.at("MergeInjections").size(), 0);
}

/**
  PickMRMFeatures Tests
*/
TEST(RawDataProcessor, constructorPickMRMFeatures)
{
  PickMRMFeatures* ptrPickFeatures = nullptr;
  PickMRMFeatures* nullPointerPickFeatures = nullptr;
  EXPECT_EQ(ptrPickFeatures, nullPointerPickFeatures);
}

TEST(RawDataProcessor, destructorPickMRMFeatures)
{
  PickMRMFeatures* ptrPickFeatures = nullptr;
  ptrPickFeatures = new PickMRMFeatures();
  delete ptrPickFeatures;
}

TEST(RawDataProcessor, gettersPickMRMFeatures)
{
  PickMRMFeatures processor;
  EXPECT_EQ(processor.getName(), "PICK_MRM_FEATURES");
}

TEST(RawDataProcessor, pickFeaturesMRM)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Test pick features
  PickMRMFeatures pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 481);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0]; // feature_map_
  EXPECT_NEAR(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate1.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  EXPECT_EQ(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate2.getRT()), 1067.5447296543123, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 481);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0]; // feature_map_history_
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  EXPECT_TRUE(hsubordinate1.getMetaValue("used_").toBool());

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  EXPECT_EQ(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  EXPECT_TRUE(hsubordinate2.getMetaValue("used_").toBool());
}

/**
  Pick2DFeatures Tests
*/
TEST(RawDataProcessor, constructorPick2DFeatures)
{
  Pick2DFeatures* ptrPickFeatures = nullptr;
  Pick2DFeatures* nullPointerPickFeatures = nullptr;
  EXPECT_EQ(ptrPickFeatures, nullPointerPickFeatures);
}

TEST(RawDataProcessor, destructorPick2DFeatures)
{
  Pick2DFeatures* ptrPickFeatures = nullptr;
  ptrPickFeatures = new Pick2DFeatures();
  delete ptrPickFeatures;
}

TEST(RawDataProcessor, gettersPick2DFeatures)
{
  Pick2DFeatures processor;
  EXPECT_EQ(processor.getName(), "PICK_2D_FEATURES");
}

TEST(RawDataProcessor, pick2DFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest_merged.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);
  rawDataHandler.getMetaData().scan_polarity = "positive";

  // Test pick features
  Pick2DFeatures pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 10);

  const OpenMS::Feature& feature1 = rawDataHandler.getFeatureMap().at(0); // feature_map_
  EXPECT_EQ(feature1.getMetaValue("native_id"), "spectrum=0");
  EXPECT_EQ(feature1.getMetaValue("PeptideRef").toString(), "Unknown");
  EXPECT_NEAR(static_cast<double>(feature1.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getMetaValue("peak_apex_int")), 1930.90576171875, 1e-6);
  EXPECT_EQ(feature1.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(feature1.getMetaValue("leftWidth")), 109.95009243262952, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getMetaValue("rightWidth")), 109.95238409935168, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getConvexHull().getHullPoints().at(0).getX()), 109.95009243262952, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getMZ()), 109.95124192810006, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getIntensity()), 2.2940683364868164, 1e-6);

  const OpenMS::Feature& feature2 = rawDataHandler.getFeatureMap().back();
  EXPECT_EQ(feature2.getMetaValue("PeptideRef").toString(), "Unknown");
  EXPECT_NEAR(static_cast<double>(feature2.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getMetaValue("peak_apex_int")), 1564.805908203125, 1e-6);
  EXPECT_EQ(feature2.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(feature2.getMetaValue("leftWidth")), 109.99321743367376, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getMetaValue("rightWidth")), 109.99550910039592, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getConvexHull().getHullPoints().at(0).getX()), 109.99321743367376, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getMZ()), 109.99435797732117, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getIntensity()), 2.3054807186126709, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 10);

  const OpenMS::Feature& hfeature1 = rawDataHandler.getFeatureMapHistory().at(0); // feature_map_history_
  EXPECT_EQ(hfeature1.getMetaValue("native_id"), "spectrum=0");
  EXPECT_EQ(hfeature1.getMetaValue("PeptideRef").toString(), "Unknown");
  EXPECT_NEAR(static_cast<double>(hfeature1.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getMetaValue("peak_apex_int")), 1930.90576171875, 1e-6);
  EXPECT_EQ(hfeature1.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hfeature1.getMetaValue("leftWidth")), 109.95009243262952, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getMetaValue("rightWidth")), 109.95238409935168, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getConvexHull().getHullPoints().at(0).getX()), 109.95009243262952, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getMZ()), 109.95124192810006, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getIntensity()), 2.2940683364868164, 1e-6);

  const OpenMS::Feature& hfeature2 = rawDataHandler.getFeatureMapHistory().back();
  EXPECT_EQ(hfeature2.getMetaValue("PeptideRef").toString(), "Unknown");
  EXPECT_NEAR(static_cast<double>(hfeature2.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getMetaValue("peak_apex_int")), 1564.805908203125, 1e-6);
  EXPECT_EQ(hfeature2.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hfeature2.getMetaValue("leftWidth")), 109.99321743367376, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getMetaValue("rightWidth")), 109.99550910039592, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getConvexHull().getHullPoints().at(0).getX()), 109.99321743367376, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getMZ()), 109.99435797732117, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getIntensity()), 2.3054807186126709, 1e-6);
}

/**
  Pick3DFeatures Tests
*/
TEST(RawDataProcessor, constructorPick3DFeatures)
{
  Pick3DFeatures* ptrPickFeatures = nullptr;
  Pick3DFeatures* nullPointerPickFeatures = nullptr;
  EXPECT_EQ(ptrPickFeatures, nullPointerPickFeatures);
}

TEST(RawDataProcessor, destructorPick3DFeatures)
{
  Pick3DFeatures* ptrPickFeatures = nullptr;
  ptrPickFeatures = new Pick3DFeatures();
  delete ptrPickFeatures;
}

TEST(RawDataProcessor, gettersPick3DFeatures)
{
  Pick3DFeatures processor;
  EXPECT_EQ(processor.getName(), "PICK_3D_FEATURES");
}

TEST(RawDataProcessor, pick3DFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_germicidin.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Test pick features
  Pick3DFeatures pickFeatures;
  map<std::string, vector<map<string, string>>> feat_params_struct({
  {"FeatureFindingMetabo", {
    { {"name", "report_chromatograms"}, {"type", "bool"}, {"value", "true"} },
    { {"name", "report_convex_hulls"}, {"type", "bool"}, {"value", "true"} },
  }}
  });
  ParameterSet feat_params(feat_params_struct);
  params_1.merge(feat_params);

  pickFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 2258);
  EXPECT_EQ(rawDataHandler.getExperiment().getChromatograms().size(), 2258);

  const OpenMS::Feature& feature1 = rawDataHandler.getFeatureMap().at(0); // feature_map_
  EXPECT_EQ(feature1.getMetaValue("num_of_masstraces").toString(), "1");
  EXPECT_EQ(feature1.getMetaValue("scan_polarity"), "positive");
  ASSERT_TRUE(feature1.getConvexHulls().size() == 1);
  EXPECT_NEAR(feature1.getConvexHull().getBoundingBox().minX(), 381.298f, 1e-3);
  EXPECT_NEAR(feature1.getConvexHull().getBoundingBox().minY(), 79.021f, 1e-3);
  EXPECT_NEAR(feature1.getConvexHull().getBoundingBox().maxX(), 540.557f, 1e-3);
  EXPECT_NEAR(feature1.getConvexHull().getBoundingBox().maxY(), 79.023f, 1e-3);
  EXPECT_NEAR(static_cast<double>(feature1.getRT()), 453.462, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getMZ()), 79.022321098842482, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature1.getIntensity()), 7978.17578125, 1e-6);

  const OpenMS::Feature& feature2 = rawDataHandler.getFeatureMap().back();
  EXPECT_EQ(feature2.getMetaValue("num_of_masstraces").toString(), "1");
  ASSERT_TRUE(feature2.getConvexHulls().size() == 1);
  EXPECT_NEAR(feature2.getConvexHull().getBoundingBox().minX(), 547.524f, 1e-2);
  EXPECT_NEAR(feature2.getConvexHull().getBoundingBox().minY(), 848.610f, 1e-3);
  EXPECT_NEAR(feature2.getConvexHull().getBoundingBox().maxX(), 592.812f, 1e-3);
  EXPECT_NEAR(feature2.getConvexHull().getBoundingBox().maxY(), 848.641f, 1e-3);
  EXPECT_NEAR(static_cast<double>(feature2.getRT()), 568.428, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getMZ()), 848.63375701405562, 1e-6);
  EXPECT_NEAR(static_cast<double>(feature2.getIntensity()), 46520.29296875, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 2258);

  const OpenMS::Feature& hfeature1 = rawDataHandler.getFeatureMapHistory().at(0); // feature_map_history_
  EXPECT_EQ(hfeature1.getMetaValue("num_of_masstraces").toString(), "1");
  EXPECT_EQ(hfeature1.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hfeature1.getRT()), 453.462, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getMZ()), 79.022321098842482, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature1.getIntensity()), 7978.17578125, 1e-6);

  const OpenMS::Feature& hfeature2 = rawDataHandler.getFeatureMapHistory().back();
  EXPECT_EQ(hfeature2.getMetaValue("num_of_masstraces").toString(), "1");
  EXPECT_NEAR(static_cast<double>(hfeature2.getRT()), 568.428, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getMZ()), 848.63375701405562, 1e-6);
  EXPECT_NEAR(static_cast<double>(hfeature2.getIntensity()), 46520.29296875, 1e-6);
}

/**
  SearchAccurateMass Tests
*/
TEST(RawDataProcessor, constructorSearchAccurateMass)
{
  SearchAccurateMass* ptrPickFeatures = nullptr;
  SearchAccurateMass* nullPointerPickFeatures = nullptr;
  EXPECT_EQ(ptrPickFeatures, nullPointerPickFeatures);
}

TEST(RawDataProcessor, destructorSearchAccurateMass)
{
  SearchAccurateMass* ptrPickFeatures = nullptr;
  ptrPickFeatures = new SearchAccurateMass();
  delete ptrPickFeatures;
}

TEST(RawDataProcessor, gettersSearchAccurateMass)
{
  SearchAccurateMass processor;
  EXPECT_EQ(processor.getName(), "SEARCH_ACCURATE_MASS");
}

TEST(RawDataProcessor, searchAccurateMass)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_serumTest.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Test accurate mass search
  SearchAccurateMass searchAccurateMass;
  searchAccurateMass.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().size(), 21);
  EXPECT_NEAR(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().calc_mass_to_charge.get(), 109.9994567849957, 1e-6);
  EXPECT_EQ(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().chemical_formula.get()), "C9H11NO6S");
  EXPECT_EQ(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().identifier.get().at(0).get(), "HMDB:HMDB0062550");
  EXPECT_NEAR(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().at(1).calc_mass_to_charge.get(), 109.99915278004569, 1e-6);
  EXPECT_EQ(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().at(1).chemical_formula.get()), "C6H12O2S2");
  EXPECT_EQ(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().at(1).identifier.get().at(0).get(), "HMDB:HMDB0033556");

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 20);

  const auto& hits2 = rawDataHandler.getFeatureMap().at(19);
  EXPECT_EQ(hits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");

  EXPECT_EQ(hits2.getSubordinates().size(), 1);

  const auto& hits2sub = hits2.getSubordinates().at(0);
  EXPECT_EQ(hits2sub.getMetaValue("identifier").toStringList().at(0), "HMDB:HMDB0062550");
  EXPECT_NEAR(static_cast<double>(hits2sub.getMetaValue("peak_apex_int")), 4385.34716796875, 1e-6);
  EXPECT_EQ(hits2sub.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hits2sub.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2sub.getMZ()), 109.99971621401676, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2sub.getIntensity()), 4385.34716796875, 1e-6);
  EXPECT_EQ(hits2sub.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  EXPECT_EQ(hits2sub.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  EXPECT_EQ(static_cast<int>(hits2sub.getCharge()), 3);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 71);

  const auto& hhits2 = rawDataHandler.getFeatureMapHistory().at(70);
  EXPECT_EQ(hhits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");

  EXPECT_EQ(hhits2.getSubordinates().size(), 1);

  const auto& hhits2sub = hhits2.getSubordinates().at(0);
  EXPECT_EQ(hhits2sub.getMetaValue("identifier").toStringList().at(0), "HMDB:HMDB0062550");
  EXPECT_NEAR(static_cast<double>(hhits2sub.getMetaValue("peak_apex_int")), 4385.34716796875, 1e-6);
  EXPECT_EQ(hhits2sub.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hhits2sub.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2sub.getMZ()), 109.99971621401676, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2sub.getIntensity()), 4385.34716796875, 1e-6);
  EXPECT_EQ(hhits2sub.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  EXPECT_EQ(hhits2sub.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  EXPECT_EQ(static_cast<int>(hhits2sub.getCharge()), 3);
}

/**
  MergeFeaturesMS1 Tests
*/
TEST(RawDataProcessor, constructorMergeFeaturesMS1)
{
  MergeFeaturesMS1* ptrPickFeatures = nullptr;
  MergeFeaturesMS1* nullPointerPickFeatures = nullptr;
  EXPECT_EQ(ptrPickFeatures, nullPointerPickFeatures);
}

TEST(RawDataProcessor, destructorMergeFeatures)
{
  MergeFeaturesMS1* ptrPickFeatures = nullptr;
  ptrPickFeatures = new MergeFeaturesMS1();
  delete ptrPickFeatures;
}

TEST(RawDataProcessor, gettersMergeFeaturesMS1)
{
  MergeFeaturesMS1 processor;
  EXPECT_EQ(processor.getName(), "MERGE_FEATURES_MS1");
}

TEST(RawDataProcessor, MergeFeaturesMS1)
{
  Filenames filenames;
  RawDataHandler rawDataHandler;
  ParameterSet params;
  filenames.setTagValue(Filenames::Tag::MAIN_DIR, SMARTPEAK_GET_TEST_DATA_PATH("DDA"));

  filenames.setFullPath("parameters", SMARTPEAK_GET_TEST_DATA_PATH("DDA/parameters.csv"));
  LoadParameters load_parameters;
  load_parameters.process(rawDataHandler, params, filenames);
  params = rawDataHandler.getParameters();

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/Germicidin A standard.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  LoadFeatures loadFeatures;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/MergeFeaturesMS1/Germicidin A standard merge features ms1.featureXML"));
  loadFeatures.process(rawDataHandler, params, filenames);

  MergeFeaturesMS1 merge_features_ms1;
  merge_features_ms1.process(rawDataHandler, params, filenames);

  const auto& feature_map = rawDataHandler.getFeatureMap();
  ASSERT_EQ(feature_map.size(), 1);
  const auto& feature_1 = feature_map.at(0);
  EXPECT_EQ(std::string(feature_1.getMetaValue("PeptideRef")), std::string("HMDB:HMDB0000001"));
  const auto& sub_features = feature_1.getSubordinates();
  ASSERT_EQ(sub_features.size(), 1);
  const auto& sub_feature_1 = feature_map.at(0);
  EXPECT_EQ(std::string(sub_feature_1.getMetaValue("PeptideRef")), std::string("HMDB:HMDB0000001"));
}

TEST(RawDataProcessor, consensusFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_serumTest_accurateMassSearch.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Test accurate mass search
  MergeFeaturesMS1 makeConsensusFeatures;
  makeConsensusFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 4);
  const auto& hits2 = rawDataHandler.getFeatureMap().at(3);
  EXPECT_NEAR(static_cast<double>(hits2.getMetaValue("peak_apex_int")), 75014.837158203125, 1e-6);
  EXPECT_EQ(hits2.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hits2.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2.getMZ()), 109.99967790230706, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2.getIntensity()), 75014.8359375, 1e-6);
  EXPECT_EQ(hits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  EXPECT_EQ(static_cast<int>(hits2.getCharge()), 0);

  EXPECT_EQ(hits2.getSubordinates().size(), 17);
  const auto& hits2_sub1 = hits2.getSubordinates().at(0);
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getMetaValue("peak_apex_int")), 1782.034423828125, 1e-6);
  EXPECT_EQ(hits2_sub1.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getMZ()), 109.99950621264246, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getIntensity()), 1782.034423828125, 1e-6);
  EXPECT_EQ(hits2_sub1.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  EXPECT_EQ(hits2_sub1.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  EXPECT_EQ(hits2_sub1.getMetaValue("identifier").toString(), "[HMDB:HMDB0062550]");
  EXPECT_EQ(hits2_sub1.getMetaValue("description").toString(), "[2-Methoxyacetaminophen sulfate]");
  EXPECT_EQ(hits2_sub1.getMetaValue("modifications").toString(), "M+3Na;3+");
  EXPECT_EQ(hits2_sub1.getMetaValue("adducts").toString(), "+3Na");
  EXPECT_EQ(hits2_sub1.getMetaValue("chemical_formula").toString(), "C9H11NO6S");
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getMetaValue("dc_charge_adduct_mass")), 68.967857984674083, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getMetaValue("mz_error_ppm")), 0.449344462258211, 1e-6);
  EXPECT_NEAR(static_cast<double>(hits2_sub1.getMetaValue("mz_error_Da")), 4.942764675774924e-05, 1e-6);
  EXPECT_EQ(static_cast<int>(hits2_sub1.getCharge()), 3);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 75);

  const auto& hhits2 = rawDataHandler.getFeatureMapHistory().at(74);
  EXPECT_NEAR(static_cast<double>(hhits2.getMetaValue("peak_apex_int")), 75014.837158203125, 1e-6);
  EXPECT_EQ(hhits2.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hhits2.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2.getMZ()), 109.99967790230706, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2.getIntensity()), 75014.8359375, 1e-6);
  EXPECT_EQ(hhits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  EXPECT_EQ(static_cast<int>(hhits2.getCharge()), 0);

  EXPECT_EQ(hhits2.getSubordinates().size(), 17);
  const auto& hhits2_sub1 = hhits2.getSubordinates().at(0);
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getMetaValue("peak_apex_int")), 1782.034423828125, 1e-6);
  EXPECT_EQ(hhits2_sub1.getMetaValue("scan_polarity"), "positive");
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getRT()), 0, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getMZ()), 109.99950621264246, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getIntensity()), 1782.034423828125, 1e-6);
  EXPECT_EQ(hhits2_sub1.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  EXPECT_EQ(hhits2_sub1.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  EXPECT_EQ(hhits2_sub1.getMetaValue("identifier").toString(), "[HMDB:HMDB0062550]");
  EXPECT_EQ(hhits2_sub1.getMetaValue("description").toString(), "[2-Methoxyacetaminophen sulfate]");
  EXPECT_EQ(hhits2_sub1.getMetaValue("modifications").toString(), "M+3Na;3+");
  EXPECT_EQ(hhits2_sub1.getMetaValue("adducts").toString(), "+3Na");
  EXPECT_EQ(hhits2_sub1.getMetaValue("chemical_formula").toString(), "C9H11NO6S");
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getMetaValue("dc_charge_adduct_mass")), 68.967857984674083, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getMetaValue("mz_error_ppm")), 0.449344462258211, 1e-6);
  EXPECT_NEAR(static_cast<double>(hhits2_sub1.getMetaValue("mz_error_Da")), 4.942764675774924e-05, 1e-6);
  EXPECT_EQ(static_cast<int>(hhits2_sub1.getCharge()), 3);
}

/**
  FilterFeatures Tests
*/
TEST(RawDataProcessor, constructorFilterFeatures)
{
  FilterFeatures* ptrFilterFeatures = nullptr;
  FilterFeatures* nullPointerFilterFeatures = nullptr;
  EXPECT_EQ(ptrFilterFeatures, nullPointerFilterFeatures);
}

TEST(RawDataProcessor, destructorFilterFeatures)
{
  FilterFeatures* ptrFilterFeatures = nullptr;
  ptrFilterFeatures = new FilterFeatures();
  delete ptrFilterFeatures;
}

TEST(RawDataProcessor, gettersFilterFeatures)
{
  FilterFeatures processor;
  EXPECT_EQ(processor.getName(), "FILTER_FEATURES");
}

TEST(RawDataProcessor, filterFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);

  // Test feature filter
  FilterFeatures filterFeatures;
  filterFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 329); // Test feature_map_
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
  EXPECT_EQ(subordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate2.getRT()), 46.652168337345103, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history_
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  EXPECT_TRUE(hsubordinate1.getMetaValue("used_").toBool());

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  EXPECT_EQ(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  EXPECT_TRUE(!hsubordinate2.getMetaValue("used_").toBool());
}

/**
  SelectFeatures Tests
*/
TEST(RawDataProcessor, constructorSelectFeatures)
{
  SelectFeatures* ptrSelectFeatures = nullptr;
  SelectFeatures* nullPointerSelectFeatures = nullptr;
  EXPECT_EQ(ptrSelectFeatures, nullPointerSelectFeatures);
}

TEST(RawDataProcessor, destructorSelectFeatures)
{
  SelectFeatures* ptrSelectFeatures = nullptr;
  ptrSelectFeatures = new SelectFeatures();
  delete ptrSelectFeatures;
}

TEST(RawDataProcessor, gettersSelectFeatures)
{
  SelectFeatures processor;
  EXPECT_EQ(processor.getName(), "SELECT_FEATURES");
}

TEST(RawDataProcessor, selectFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_2_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Test select features
  SelectFeatures selectFeatures;
  selectFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 114); // Test feature_map_
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 4132.0, 1e-6);
  EXPECT_EQ(subordinate2.getMetaValue("native_id").toString(), "f6p.f6p_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate2.getRT()), 439.165833, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 329); // Test feature_map_history_
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getRT()), 953.66569377291205, 1e-6);
  EXPECT_TRUE(hsubordinate1.getMetaValue("used_").toBool());

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
  EXPECT_EQ(hsubordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getRT()), 46.652168337345103, 1e-6);
  EXPECT_TRUE(hsubordinate2.getMetaValue("used_").toBool());
}

/**
  ValidateFeatures Tests
*/
TEST(RawDataProcessor, constructorValidateFeatures)
{
  ValidateFeatures* ptrValidateFeatures = nullptr;
  ValidateFeatures* nullPointerValidateFeatures = nullptr;
  EXPECT_EQ(ptrValidateFeatures, nullPointerValidateFeatures);
}

TEST(RawDataProcessor, destructorValidateFeatures)
{
  ValidateFeatures* ptrValidateFeatures = nullptr;
  ptrValidateFeatures = new ValidateFeatures();
  delete ptrValidateFeatures;
}

TEST(RawDataProcessor, gettersValidateFeatures)
{
  ValidateFeatures processor;
  EXPECT_EQ(processor.getName(), "VALIDATE_FEATURES");
}

TEST(RawDataProcessor, validateFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("referenceData", SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv"));
  LoadValidationData loadValidationData;
  loadValidationData.process(rawDataHandler, params_1, filenames);

  MetaDataHandler& mdh = rawDataHandler.getMetaData();
  mdh.setSampleName("150601_0_BloodProject01_PLT_QC_Broth-1"); // info taken from .csv file
  mdh.inj_number = 1;
  mdh.batch_name = "BloodProject01";
  mdh.setAcquisitionDateAndTimeFromString("09-06-2015 17:14", "%m-%d-%Y %H:%M");

  // Test validate features
  ValidateFeatures validateFeatures;
  validateFeatures.process(rawDataHandler, params_1, filenames);

  const std::map<std::string, float>& validation_metrics = rawDataHandler.getValidationMetrics();
  // Confusion matrix: [TP, FP, FN, TN] = [0, 155, 0, 0]
  EXPECT_NEAR(validation_metrics.at("accuracy"), 0.0, 1e-3);
  EXPECT_NEAR(validation_metrics.at("precision"), 0.0, 1e-3);
  EXPECT_EQ(std::isnan(validation_metrics.at("recall")), true);
}
/**
  PlotFeatures Tests
*/
TEST(RawDataProcessor, constructorPlotFeatures)
{
  PlotFeatures* ptrPlotFeatures = nullptr;
  PlotFeatures* nullPointerPlotFeatures = nullptr;
  EXPECT_EQ(ptrPlotFeatures, nullPointerPlotFeatures);
}

TEST(RawDataProcessor, destructorPlotFeatures)
{
  PlotFeatures* ptrPlotFeatures = nullptr;
  ptrPlotFeatures = new PlotFeatures();
  delete ptrPlotFeatures;
}

TEST(RawDataProcessor, gettersPlotFeatures)
{
  PlotFeatures processor;
  EXPECT_EQ(processor.getName(), "PLOT_FEATURES");
}

TEST(RawDataProcessor, plotFeatures)
{
// TODO: Uncomment once FeaturePlotter is ready
}

/**
  QuantifyFeatures Tests
*/
TEST(RawDataProcessor, constructorQuantifyFeatures)
{
  QuantifyFeatures* ptrQuantifyFeatures = nullptr;
  QuantifyFeatures* nullPointerQuantifyFeatures = nullptr;
  EXPECT_EQ(ptrQuantifyFeatures, nullPointerQuantifyFeatures);
}

TEST(RawDataProcessor, destructorQuantifyFeatures)
{
  QuantifyFeatures* ptrQuantifyFeatures = nullptr;
  ptrQuantifyFeatures = new QuantifyFeatures();
  delete ptrQuantifyFeatures;
}

TEST(RawDataProcessor, gettersQuantifyFeatures)
{
  QuantifyFeatures processor;
  EXPECT_EQ(processor.getName(), "QUANTIFY_FEATURES");
}

TEST(RawDataProcessor, quantifyComponents)
{
  // Pre-requisites: load the parameters and associated raw data
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv"));
  SequenceSegmentHandler sequenceSegmentHandler_IO;
  
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(sequenceSegmentHandler_IO, SequenceHandler(), {}, filenames);

  RawDataHandler rawDataHandler;
  rawDataHandler.setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, {}, filenames);

  // Test quantify features
  QuantifyFeatures quantifyFeatures;
  quantifyFeatures.process(rawDataHandler, {}, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 98);  // Test feature_map
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  EXPECT_EQ(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_NEAR(static_cast<double>(sub.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  EXPECT_EQ(sub.getMetaValue("concentration_units").toString(), "uM");

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 98); // Test feature_history
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  EXPECT_EQ(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_NEAR(static_cast<double>(hsub.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  EXPECT_EQ(hsub.getMetaValue("concentration_units").toString(), "uM");
  EXPECT_TRUE(hsub.getMetaValue("used_").toBool());
}

/**
  CheckFeatures Tests
*/
TEST(RawDataProcessor, constructorCheckFeatures)
{
  CheckFeatures* ptrCheckFeatures = nullptr;
  CheckFeatures* nullPointerCheckFeatures = nullptr;
  EXPECT_EQ(ptrCheckFeatures, nullPointerCheckFeatures);
}

TEST(RawDataProcessor, destructorCheckFeatures)
{
  CheckFeatures* ptrCheckFeatures = nullptr;
  ptrCheckFeatures = new CheckFeatures();
  delete ptrCheckFeatures;
}

TEST(RawDataProcessor, gettersCheckFeatures)
{
  CheckFeatures processor;
  EXPECT_EQ(processor.getName(), "CHECK_FEATURES");
}

TEST(RawDataProcessor, checkFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, params_1, filenames);

  // Test check features
  CheckFeatures checkFeatures;
  checkFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 98);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getMetaValue("QC_transition_group_pass").toString(), "1");
  EXPECT_EQ(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(sub.getMetaValue("QC_transition_pass").toString(), "1");

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 98);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("QC_transition_group_pass").toString(), "1");
  EXPECT_EQ(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(hsub.getMetaValue("QC_transition_pass").toString(), "1");
  EXPECT_TRUE(hsub.getMetaValue("used_").toBool());
}

/**
  FilterFeaturesRSDs Tests
*/
TEST(RawDataProcessor, constructorFilterFeaturesRSDs)
{
  FilterFeaturesRSDs* ptrFilterFeaturesRSDs = nullptr;
  FilterFeaturesRSDs* nullPointerFilterFeaturesRSDs = nullptr;
  EXPECT_EQ(ptrFilterFeaturesRSDs, nullPointerFilterFeaturesRSDs);
}

TEST(RawDataProcessor, destructorFilterFeaturesRSDs)
{
  FilterFeaturesRSDs* ptrFilterFeaturesRSDs = nullptr;
  ptrFilterFeaturesRSDs = new FilterFeaturesRSDs();
  delete ptrFilterFeaturesRSDs;
}

TEST(RawDataProcessor, gettersFilterFeaturesRSDs)
{
  FilterFeaturesRSDs processor;
  EXPECT_EQ(processor.getName(), "FILTER_FEATURES_RSDS");
}

TEST(RawDataProcessor, filterFeaturesRSDs)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv"));
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);
  rawDataHandler.setFeatureRSDFilter(rawDataHandler.getFeatureFilter()); // copy over the feature filter

  // Make the feature RSD estimates
  OpenMS::MRMFeatureQC feature_rsd_estimations = rawDataHandler.getFeatureFilter();
  feature_rsd_estimations.component_group_qcs.at(115).retention_time_u = 40; // accoa
  feature_rsd_estimations.component_qcs.at(292).meta_value_qc.at("peak_apex_int").second = 40; // "23dpg.23dpg_1.Heavy"
  rawDataHandler.setFeatureRSDEstimations(feature_rsd_estimations);

  // Test feature filter
  FilterFeaturesRSDs filterFeatures;
  filterFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 478); // Test feature_map
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 262509, 1e-6);
  EXPECT_EQ(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_NEAR(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 640, 1e-6);
  EXPECT_EQ(subordinate2.getMetaValue("native_id").toString(), "acon-C.acon-C_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate2.getRT()), 842.11033066606501, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history_
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  EXPECT_TRUE(!hsubordinate1.getMetaValue("used_").toBool());

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  EXPECT_EQ(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  EXPECT_TRUE(!hsubordinate2.getMetaValue("used_").toBool());
}

/**
  FilterFeaturesBackgroundInterferences Tests
*/
TEST(RawDataProcessor, constructorFilterFeaturesBackgroundInterferences)
{
  FilterFeaturesBackgroundInterferences* ptrFilterFeaturesBackgroundInterferences = nullptr;
  FilterFeaturesBackgroundInterferences* nullPointerFilterFeaturesBackgroundInterferences = nullptr;
  EXPECT_EQ(ptrFilterFeaturesBackgroundInterferences, nullPointerFilterFeaturesBackgroundInterferences);
}

TEST(RawDataProcessor, destructorFilterFeaturesBackgroundInterferences)
{
  FilterFeaturesBackgroundInterferences* ptrFilterFeaturesBackgroundInterferences = nullptr;
  ptrFilterFeaturesBackgroundInterferences = new FilterFeaturesBackgroundInterferences();
  delete ptrFilterFeaturesBackgroundInterferences;
}

TEST(RawDataProcessor, gettersFilterFeaturesBackgroundInterferences)
{
  FilterFeaturesBackgroundInterferences processor;
  EXPECT_EQ(processor.getName(), "FILTER_FEATURES_BACKGROUND_INTERFERENCES");
}

TEST(RawDataProcessor, filterFeaturesBackgroundInterferences)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv"));
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);
  rawDataHandler.setFeatureBackgroundFilter(rawDataHandler.getFeatureFilter()); // copy over the feature filter
  rawDataHandler.setFeatureBackgroundEstimations(rawDataHandler.getFeatureFilter());

  // Test feature filter
  FilterFeaturesBackgroundInterferences filterFeatures;
  filterFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 479); // Test feature_map_
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 1);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 3842, 1e-6);
  EXPECT_EQ(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Light");
  EXPECT_NEAR(static_cast<double>(subordinate2.getRT()), 1067.54472965431, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history_
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  EXPECT_TRUE(hsubordinate1.getMetaValue("used_").toBool());

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  EXPECT_EQ(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  EXPECT_TRUE(!hsubordinate2.getMetaValue("used_").toBool());
}

/**
  CheckFeaturesBackgroundInterferences Tests
*/
TEST(RawDataProcessor, constructorCheckFeaturesBackgroundInterferences)
{
  CheckFeaturesBackgroundInterferences* ptrCheckFeaturesBackgroundInterferences = nullptr;
  CheckFeaturesBackgroundInterferences* nullPointerCheckFeaturesBackgroundInterferences = nullptr;
  EXPECT_EQ(ptrCheckFeaturesBackgroundInterferences, nullPointerCheckFeaturesBackgroundInterferences);
}

TEST(RawDataProcessor, destructorCheckFeaturesBackgroundInterferences)
{
  CheckFeaturesBackgroundInterferences* ptrCheckFeaturesBackgroundInterferences = nullptr;
  ptrCheckFeaturesBackgroundInterferences = new CheckFeaturesBackgroundInterferences();
  delete ptrCheckFeaturesBackgroundInterferences;
}

TEST(RawDataProcessor, gettersCheckFeaturesBackgroundInterferences)
{
  CheckFeaturesBackgroundInterferences processor;
  EXPECT_EQ(processor.getName(), "CHECK_FEATURES_BACKGROUND_INTERFERENCES");
}

TEST(RawDataProcessor, checkFeaturesBackgroundInterferences)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv"));
  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, params_1, filenames);

  // Make the feature background estimates
  OpenMS::MRMFeatureQC feature_background_estimations = rawDataHandler.getFeatureQC();
  feature_background_estimations.component_qcs.at(293).intensity_u = 30; // "23dpg.23dpg_1.Light"
  rawDataHandler.setFeatureBackgroundQC(feature_background_estimations); // copy over the feature filter
  feature_background_estimations.component_qcs.at(293).intensity_u = 1e12; // "23dpg.23dpg_1.Light"
  rawDataHandler.setFeatureBackgroundEstimations(feature_background_estimations);

  // Test check features
  CheckFeaturesBackgroundInterferences checkFeatures;
  checkFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 98);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getMetaValue("QC_transition_group_%BackgroundInterference_pass").toString(), "1");
  EXPECT_EQ(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(sub.getMetaValue("QC_transition_%BackgroundInterference_pass").toString(), "0");

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 98);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("QC_transition_group_%BackgroundInterference_pass").toString(), "1");
  EXPECT_EQ(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(hsub.getMetaValue("QC_transition_%BackgroundInterference_pass").toString(), "0");
  EXPECT_TRUE(hsub.getMetaValue("used_").toBool());
}

/**
  CheckFeaturesRSDs Tests
*/
TEST(RawDataProcessor, constructorCheckFeaturesRSDs)
{
  CheckFeaturesRSDs* ptrCheckFeaturesRSDs = nullptr;
  CheckFeaturesRSDs* nullPointerCheckFeaturesRSDs = nullptr;
  EXPECT_EQ(ptrCheckFeaturesRSDs, nullPointerCheckFeaturesRSDs);
}

TEST(RawDataProcessor, destructorCheckFeaturesRSDs)
{
  CheckFeaturesRSDs* ptrCheckFeaturesRSDs = nullptr;
  ptrCheckFeaturesRSDs = new CheckFeaturesRSDs();
  delete ptrCheckFeaturesRSDs;
}

TEST(RawDataProcessor, gettersCheckFeaturesRSDs)
{
  CheckFeaturesRSDs processor;
  EXPECT_EQ(processor.getName(), "CHECK_FEATURES_RSDS");
}

TEST(RawDataProcessor, checkFeaturesRSDs)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv"));
  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, params_1, filenames);
  rawDataHandler.setFeatureRSDQC(rawDataHandler.getFeatureQC()); // copy over the feature filter

  // Make the feature RSD estimates
  OpenMS::MRMFeatureQC feature_rsd_estimations = rawDataHandler.getFeatureQC();
  feature_rsd_estimations.component_qcs.at(293).meta_value_qc.at("peak_apex_int").second = 40; // "23dpg.23dpg_1.Heavy"
  rawDataHandler.setFeatureRSDEstimations(feature_rsd_estimations);

  // Test check features
  CheckFeaturesRSDs checkFeatures;
  checkFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 98);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getMetaValue("QC_transition_group_%RSD_pass").toString(), "1");
  EXPECT_EQ(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(sub.getMetaValue("QC_transition_%RSD_pass").toString(), "0");

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 98);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("QC_transition_group_%RSD_pass").toString(), "1");
  EXPECT_EQ(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(hsub.getMetaValue("QC_transition_%RSD_pass").toString(), "0");
  EXPECT_TRUE(hsub.getMetaValue("used_").toBool());
}

TEST(RawDataProcessor, process)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("quantitationMethods", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv"));
  SequenceSegmentHandler sequenceSegmentHandler_IO;
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(sequenceSegmentHandler_IO, SequenceHandler(), {}, filenames);
  rawDataHandler.setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  params_1.addFunctionParameters(FunctionParameters("ChromatogramExtractor"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Test integrated workflow: pickFeatures, filterFeatures, SelectFeatures, quantifyFeatures, and checkFeatures
  PickMRMFeatures pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);
  FilterFeatures filterFeatures;
  filterFeatures.process(rawDataHandler, params_1, filenames);
  SelectFeatures selectFeatures;
  selectFeatures.process(rawDataHandler, params_1, filenames);
  QuantifyFeatures quantifyFeatures;
  quantifyFeatures.process(rawDataHandler, params_1, filenames);
  CheckFeatures checkFeatures;
  checkFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 114); // test feature_map_
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate0 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate0.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(subordinate0.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate0.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  EXPECT_EQ(subordinate1.getMetaValue("QC_transition_pass").toString(), "1");
  EXPECT_NEAR(static_cast<double>(subordinate1.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  EXPECT_EQ(subordinate1.getMetaValue("concentration_units").toString(), "uM");

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[8].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 2780997.0, 1e-6);
  EXPECT_EQ(subordinate2.getMetaValue("native_id").toString(), "Pool_2pg_3pg.Pool_2pg_3pg_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate2.getRT()), 832.69577069044112, 1e-6);

  const OpenMS::Feature& subordinate3 = rawDataHandler.getFeatureMap()[49].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(subordinate3.getMetaValue("peak_apex_int")), 217684.0, 1e-6);
  EXPECT_EQ(subordinate3.getMetaValue("native_id").toString(), "f1p.f1p_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate3.getRT()), 602.45962461757676, 1e-6);

  const OpenMS::Feature& subordinate4 = rawDataHandler.getFeatureMap()[113].getSubordinates()[0]; // this is [49][0] in python tests
  EXPECT_NEAR(static_cast<double>(subordinate4.getMetaValue("peak_apex_int")), 4066240.0, 1e-6);
  EXPECT_EQ(subordinate4.getMetaValue("native_id").toString(), "xan.xan_1.Heavy");
  EXPECT_NEAR(static_cast<double>(subordinate4.getRT()), 89.418783654689776, 1e-6);

  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 481); // test feature_map_history_
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  EXPECT_EQ(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("concentration_units").toString(), "uM");
  EXPECT_NEAR(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  EXPECT_EQ(hsubordinate1.getMetaValue("QC_transition_pass").toString(), "1");
  EXPECT_TRUE(hsubordinate1.getMetaValue("used_").toBool());

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  EXPECT_EQ(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  EXPECT_NEAR(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  EXPECT_TRUE(!hsubordinate2.getMetaValue("used_").toBool());
}

TEST(RawDataProcessor, emg_processor)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Test pick features
  PickMRMFeatures pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 481);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature sub1 { rawDataHandler.getFeatureMap()[0].getSubordinates()[0] };
  EXPECT_NEAR(static_cast<double>(sub1.getIntensity()), 922154.75, 1e-6);
  EXPECT_NEAR(static_cast<double>(sub1.getMetaValue("peak_apex_position")), 953.40699999999993, 1e-6);
  EXPECT_NEAR(static_cast<double>(sub1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  EXPECT_EQ(sub1.metaValueExists("area_background_level") == false, 1);
  EXPECT_EQ(sub1.metaValueExists("noise_background_level") == false, 1);
  // EXPECT_NEAR(static_cast<double>(sub1.getMetaValue("area_background_level")), 0.0, 1e-6);
  // EXPECT_NEAR(static_cast<double>(sub1.getMetaValue("noise_background_level")), 0.0, 1e-6);

  // reduce the number of features for test purposes
  OpenMS::FeatureMap& m = rawDataHandler.getFeatureMap();
  m.erase(m.begin() + 1, m.end());

  EXPECT_EQ(m.size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 1);

  FitFeaturesEMG emg;
  emg.process(rawDataHandler, params_1, filenames);

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature sub2 { rawDataHandler.getFeatureMap()[0].getSubordinates()[0] };
  EXPECT_NEAR(static_cast<double>(sub2.getIntensity()), 758053.375, 1e-6);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("peak_apex_position")), 953.40699999999993, 1e-6);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("peak_apex_int")), 202893.72550713021, 1e-6);
  EXPECT_EQ(sub2.metaValueExists("area_background_level") == false, 0);
  EXPECT_EQ(sub2.metaValueExists("noise_background_level") == false, 0);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("area_background_level")), 0.0083339133585532514, 1e-6);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("noise_background_level")), 0.00012474754841647006, 1e-6);

  // test feature storing
  RawDataHandler rawDataHandler2;
  filenames.setFullPath("featureXML_o", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.featureXML"));
  StoreFeatures storeFeatures;
  storeFeatures.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.featureXML"));
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler2, params_1, filenames);

  const OpenMS::FeatureMap& m2 = rawDataHandler2.getFeatureMap();
  const OpenMS::Feature sub3 { m2[0].getSubordinates()[0] };

  EXPECT_NEAR(static_cast<double>(sub2.getIntensity()), static_cast<double>(sub3.getIntensity()), 1e-6);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("peak_apex_position")), static_cast<double>(sub3.getMetaValue("peak_apex_position")), 1e-6);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("peak_apex_int")), static_cast<double>(sub3.getMetaValue("peak_apex_int")), 1e-6);
  EXPECT_EQ(sub3.metaValueExists("area_background_level") == false, 0);
  EXPECT_EQ(sub3.metaValueExists("noise_background_level") == false, 0);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("area_background_level")), static_cast<double>(sub3.getMetaValue("area_background_level")), 1e-6);
  EXPECT_NEAR(static_cast<double>(sub2.getMetaValue("noise_background_level")), static_cast<double>(sub3.getMetaValue("noise_background_level")), 1e-6);

  std::remove(SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.featureXML"));
}

TEST(RawDataProcessor, calculateMDVs)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  
  std::vector<OpenMS::Peak2D::IntensityType> L1_peak_apex_int {3.61e+08, 1.20e+04, 1.02e+05, 2.59e+04};
  std::vector<OpenMS::Peak2D::IntensityType> L2_peak_apex_int {2.77e+07, 5.45e+04, 6.26e+05, 7.46e+04, 2.75e+04};

  std::vector<OpenMS::Peak2D::IntensityType> L1_norm_max {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};
  std::vector<OpenMS::Peak2D::IntensityType> L1_norm_sum {9.9961e-01, 3.3228e-05, 2.8243e-04, 7.1717e-05};

  std::vector<OpenMS::Peak2D::IntensityType> L2_norm_max {1.00e+00, 1.967e-03, 2.259e-02, 2.693e-03, 9.927e-04};
  std::vector<OpenMS::Peak2D::IntensityType> L2_norm_sum {9.7252e-01, 1.9134e-03, 2.1978e-02, 2.6191e-03, 9.655e-04};

  // Lactate1 & Lactate2 - peak_apex_int - norm_max
  OpenMS::Feature               lactate_1_normmax;
  OpenMS::Feature               lactate_1_normalized_normmax;
  std::vector<OpenMS::Feature>  L1_subordinates_normmax;

  lactate_1_normmax.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_peak_apex_int[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normmax.setSubordinates(L1_subordinates_normmax);
  
  OpenMS::Feature               lactate_2_normmax;
  OpenMS::Feature               lactate_2_normalized_normmax;
  std::vector<OpenMS::Feature>  L2_subordinates_normmax;

  lactate_2_normmax.setMetaValue("PeptideRef", "Lactate2");
  for (uint16_t i = 0; i < L2_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate2_"+std::to_string(219+i));
    sub.setMetaValue("peak_apex_int", L2_peak_apex_int[i]);
    L2_subordinates_normmax.push_back(sub);
  }
  lactate_2_normmax.setSubordinates(L2_subordinates_normmax);
  
  OpenMS::Feature               lactate_1_normsum;
  OpenMS::Feature               lactate_1_normalized_normsum;
  std::vector<OpenMS::Feature>  L1_subordinates_normsum;

  lactate_1_normsum.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_peak_apex_int[i]);
    L1_subordinates_normsum.push_back(sub);
  }
  lactate_1_normsum.setSubordinates(L1_subordinates_normsum);
  
  OpenMS::Feature lactate_2_normsum; OpenMS::Feature lactate_2_normalized_normsum;
  std::vector<OpenMS::Feature> L2_subordinates_normsum;

  lactate_2_normsum.setMetaValue("PeptideRef", "Lactate2");
  for (uint16_t i = 0; i < L2_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate2_"+std::to_string(219+i));
    sub.setMetaValue("peak_apex_int", L2_peak_apex_int[i]);
    L2_subordinates_normsum.push_back(sub);
  }
  lactate_2_normsum.setSubordinates(L2_subordinates_normsum);
  
  OpenMS::FeatureMap  lactate_normmax;
  OpenMS::FeatureMap  lactate_normsum;
  OpenMS::FeatureMap  lactate_normalized_normmax;
  OpenMS::FeatureMap  lactate_normalized_normsum;

  lactate_normmax.push_back(lactate_1_normmax);
  lactate_normmax.push_back(lactate_2_normmax);

  lactate_normsum.push_back(lactate_1_normsum);
  lactate_normsum.push_back(lactate_2_normsum);

  rawDataHandler.setFeatureMap(lactate_normmax);

  CalculateMDVs calculateMDVs;
  calculateMDVs.process(rawDataHandler, params_1, filenames);
  
  lactate_normalized_normsum = rawDataHandler.getFeatureMap();
  
  for(size_t i = 0; i < lactate_normalized_normsum.size(); ++i)
  {
    for(size_t j = 0; j < lactate_1_normalized_normsum.getSubordinates().size(); ++j)
    {
      if (i == 0) // lactate_1
      {
        EXPECT_NEAR((float)lactate_normalized_normsum.at(i).getSubordinates().at(j).getMetaValue("peak_apex_int"),
                          L1_norm_sum.at(j), 1e-6);
      }
      else if (i == 1) // lactate_2
      {
        EXPECT_NEAR((float)lactate_normalized_normsum.at(i).getSubordinates().at(j).getMetaValue("peak_apex_int"),
                          L2_norm_sum.at(j), 1e-6);
      }
    }
  }
}

TEST(RawDataProcessor, isotopicCorrections)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  
  // case 1: validating corrected results (corrected peak_apex_int)
  IsotopicCorrections                   isotopicCorrections;
  OpenMS::Feature                       lactate_1_normalized;
  OpenMS::Feature                       lactate_1_corrected;
  OpenMS::FeatureMap                    lactate_1_featureMap;
  OpenMS::FeatureMap                    lactate_1_corrected_featureMap;
  std::vector<std::vector<double>>      correction_matrix_inversed(4, std::vector<double>(4,0));

  // L1_norm_max, L1_peak_apex_int From CHO_190316_Flux.xlsx provided by Douglas McCloskey
  // L1_corrected self calculated
  std::vector<double>                         L1_norm_max       {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};
  std::vector<double>                         L1_corrected      {-12.7699, 140.7289, -45.3788, -47.2081};
  std::vector<OpenMS::Peak2D::IntensityType>  L1_peak_apex_int  {3.61e+08, 1.20e+04, 1.02e+05, 2.59e+04};
  std::vector<OpenMS::Feature>                L1_subordinates_normmax;

  
  lactate_1_normalized.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_norm_max.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_norm_max[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normalized.setSubordinates(L1_subordinates_normmax);

  for(uint8_t i = 0; i < 3; ++i)
  {
    lactate_1_featureMap.push_back(lactate_1_normalized);
  }
 
  rawDataHandler.setFeatureMap(lactate_1_featureMap);
  isotopicCorrections.process(rawDataHandler, params_1, filenames);
  lactate_1_corrected_featureMap = rawDataHandler.getFeatureMap();
  
  for(uint8_t i = 0; i < lactate_1_corrected_featureMap.size(); ++i)
  {
    for(uint8_t j = 0; j < lactate_1_corrected_featureMap.at(i).getSubordinates().size(); ++j)
    {
      EXPECT_NEAR((double)lactate_1_corrected_featureMap.at(i).getSubordinates().at(j).getMetaValue("peak_apex_int"),
                        L1_corrected[j], 1e-3);
    }
  }
}

TEST(RawDataProcessor, calculateIsotopicPurities)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;

  CalculateIsotopicPurities     calculateIsotopicPurities;
  OpenMS::Feature               lactate_1_normalized;
  OpenMS::FeatureMap            lactate_1_featureMap;
  OpenMS::FeatureMap            lactate_1_with_isotopic_purity_featureMap;
  std::vector<double>           L1_norm_max {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};
  std::vector<OpenMS::Feature>  L1_subordinates_normmax;
  
  lactate_1_normalized.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_norm_max.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_norm_max[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normalized.setSubordinates(L1_subordinates_normmax);

  for(uint8_t i = 0; i < 3; ++i)
  {
    lactate_1_featureMap.push_back(lactate_1_normalized);
  }
  
  rawDataHandler.setFeatureMap(lactate_1_featureMap);
  calculateIsotopicPurities.process(rawDataHandler, params_1, filenames);
  lactate_1_with_isotopic_purity_featureMap = rawDataHandler.getFeatureMap();
  std::vector<OpenMS::String> keys;
  lactate_1_with_isotopic_purity_featureMap.at(0).getKeys(keys);

  for(uint8_t i = 0; i < lactate_1_with_isotopic_purity_featureMap.size(); ++i)
  {
    EXPECT_NEAR((double)(lactate_1_with_isotopic_purity_featureMap.at(i).getMetaValue("1_2-13C_glucose_experiment")) * 100,
                      99.6469, 1e-4);
  }
}

TEST(RawDataProcessor, calculateMDVAccuracies)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  // for peptide SumFormula
  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, {}, filenames);
  
  CalculateMDVAccuracies calculateMDVAccuracies;
  OpenMS::Feature        feature_1;
  OpenMS::FeatureMap     featureMap_1;
  
  std::vector<double>    accoa_C23H37N7O17P3S_MRM_measured_13    {0.627, 0.253, 0.096, 0.02, 0.004, 0.001};
  std::vector<double>    accoa_C23H37N7O17P3S_abs_diff           {0.0632108, 0.0505238, 0.0119821, 0.0014131614, 3.15664365e-05, 0.000323269283};
  std::vector<double>    Average_accuracy_groundtruth            {0.02374, 0.03451}; // [accoa_13, fad_48]

  std::map<std::string,std::string> theoretical_formulas                    {{"accoa","C23H37N7O17P3S"},{"fad","C27H32N9O15P2"}};

  std::vector<OpenMS::Feature>      L1_subordinates, L2_subordinates;
  
  feature_1.setMetaValue("PeptideRef", "accoa");
  for (uint16_t i = 0; i < accoa_C23H37N7O17P3S_MRM_measured_13.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", accoa_C23H37N7O17P3S_MRM_measured_13[i]);
    L1_subordinates.push_back(sub);
  }
  feature_1.setSubordinates(L1_subordinates);

  for (uint8_t i = 0; i < 3; ++i)
  {
    featureMap_1.push_back(feature_1);
  }

  rawDataHandler.setFeatureMap(featureMap_1);
  calculateMDVAccuracies.process(rawDataHandler, params_1, filenames);
  featureMap_1 = rawDataHandler.getFeatureMap();

  for (size_t i = 0; i < featureMap_1.size(); ++i)
  {
    EXPECT_NEAR( (float)featureMap_1.at(i).getMetaValue("average_accuracy"), Average_accuracy_groundtruth[0], 1e-1 );

    for (size_t feature_subordinate = 0; feature_subordinate < featureMap_1.at(i).getSubordinates().size(); ++feature_subordinate)
    {
      EXPECT_NEAR( (float)featureMap_1.at(i).getSubordinates().at(feature_subordinate).getMetaValue("absolute_difference"),
                          accoa_C23H37N7O17P3S_abs_diff[feature_subordinate], 1e-2 );
    }
  }
}

/**
  SearchSpectrumMS1 Tests
*/
TEST(RawDataProcessor, gettersSearchSpectrumMS1)
{
  SearchSpectrumMS1 processor;
  EXPECT_EQ(processor.getName(), "SEARCH_SPECTRUM_MS1");
}

TEST(RawDataProcessor, SearchSpectrumMS1)
{
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("dda_min_traML.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("dda_min.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  LoadFeatures loadFeatures;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("dda_min.featureXML"));
  loadFeatures.process(rawDataHandler, params_1, filenames);

  SearchSpectrumMS1 searchSpectrum;
  searchSpectrum.process(rawDataHandler, params_1, filenames);

  ASSERT_EQ(rawDataHandler.getFeatureMap().size(), 1986);
  ASSERT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 1);
  const auto& f = rawDataHandler.getFeatureMap()[0];
  EXPECT_EQ(f.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0002362");
  const auto& s = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  EXPECT_EQ(s.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0002362");
  EXPECT_EQ(s.getMetaValue("native_id").toString(), "C4H10N2O2;M+H+K;2+");
  EXPECT_EQ(s.getMetaValue("identifier").toString(), "[HMDB:HMDB0002362, HMDB:HMDB0006284]");
  EXPECT_EQ(s.getMetaValue("modifications").toString(), "M+H+K;2+");
  EXPECT_NEAR(s.getMetaValue("dc_charge_adduct_mass"), 39.97047649376457, 1e-6);
  EXPECT_EQ(s.getMetaValue("chemical_formula").toString(), "C4H10N2O2");
  EXPECT_NEAR(s.getMetaValue("mz_error_ppm"), 0.180061646746413, 1e-6);
  EXPECT_NEAR(s.getMetaValue("mz_error_Da"), 1.4228891060952265e-05, 1e-6);
}

/**
  Match Spectra Tests
*/
TEST(RawDataProcessor, getterMatchSpectra)
{
  MatchSpectra processor;
  EXPECT_EQ(processor.getName(), "MATCH_SPECTRA");
}

TEST(RawDataProcessor, MatchSpectra)
{
  Filenames filenames;

  ParameterSet params;
  RawDataHandler rawDataHandler;
  filenames.setFullPath("cmp_spectra", SMARTPEAK_GET_TEST_DATA_PATH("Germicidin_A_standard.msp"));
  MatchSpectra match_spectra;
  match_spectra.process(rawDataHandler, params, filenames);
}

/**
  Store MSP Tests
*/
TEST(RawDataProcessor, getterStoreMSP)
{
  StoreMSP processor;
  EXPECT_EQ(processor.getName(), "STORE_MSP");
}

TEST(RawDataProcessor, StoreMSP)
{
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.setFullPath("traML", SMARTPEAK_GET_TEST_DATA_PATH("dda_min_traML.csv"));
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("dda_min.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  LoadFeatures loadFeatures;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("dda_min.featureXML"));
  loadFeatures.process(rawDataHandler, params_1, filenames);

  auto path_msp = std::tmpnam(nullptr);
  filenames.setFullPath("output_ms2", path_msp);
  StoreMSP store_msp;
  store_msp.process(rawDataHandler, params_1, filenames);

  OpenMS::MSPGenericFile msp_file;
  OpenMS::MSExperiment experiment;
  msp_file.load(path_msp, experiment);

  ASSERT_EQ(experiment.size(), 926);
}

/**
  Extract Spectra Non Targeted
*/
TEST(RawDataProcessor, getterExtractSpectraNonTargeted)
{
  ExtractSpectraNonTargeted processor;
  EXPECT_EQ(processor.getName(), "EXTRACT_SPECTRA_NON_TARGETED");
}


TEST(RawDataProcessor, ExtractSpectraNonTargeted)
{
  Filenames filenames;
  RawDataHandler rawDataHandler;
  ParameterSet params;

  filenames.setFullPath("parameters", SMARTPEAK_GET_TEST_DATA_PATH("DDA/parameters.csv"));
  LoadParameters load_parameters;
  load_parameters.process(rawDataHandler, params, filenames);
  params = rawDataHandler.getParameters();

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/Germicidin A standard.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  LoadFeatures loadFeatures;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/ExtractSpectraNonTargeted/Germicidin A standard.featureXML"));
  loadFeatures.process(rawDataHandler, params, filenames);

  ExtractSpectraNonTargeted extract_spectra_non_targeted;
  extract_spectra_non_targeted.process(rawDataHandler, params, filenames);
  const auto& feature_map = rawDataHandler.getFeatureMap();
  ASSERT_EQ(feature_map.size(), 1);
  const auto& feature = feature_map.begin();
  EXPECT_FLOAT_EQ(feature->getRT(), 390.259);
  EXPECT_FLOAT_EQ(feature->getMZ(), 149.02496);
  EXPECT_EQ(feature->getMetaValue("transition_name"), std::string("HMDB:HMDB0000001"));
}


TEST(RawDataProcessor, ConstructTransitionsList_csv)
{
  Filenames filenames;
  RawDataHandler rawDataHandler;
  ParameterSet params;

  filenames.setFullPath("parameters", SMARTPEAK_GET_TEST_DATA_PATH("DDA/parameters.csv"));
  LoadParameters load_parameters;
  load_parameters.process(rawDataHandler, params, filenames);
  params = rawDataHandler.getParameters();

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/Germicidin A standard.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  LoadFeatures loadFeatures_ms1;
  RawDataHandler rawDataHandler_ms1;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/ConstructTransitionsList/Germicidin A standard ms1 merged features.featureXML"));
  loadFeatures_ms1.process(rawDataHandler_ms1, params, filenames);

  LoadFeatures loadFeatures;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/ConstructTransitionsList/Germicidin A standard ms2 merged features.featureXML"));
  loadFeatures.process(rawDataHandler, params, filenames);

  rawDataHandler.setFeatureMap("ms1_merged_features", rawDataHandler_ms1.getFeatureMap());

  std::string output_file_path = std::tmpnam(nullptr);
  filenames.setFullPath("output_transitions_csv", output_file_path);
  ConstructTransitionsList construct_transitions_list;
  construct_transitions_list.process(rawDataHandler, params, filenames);

  const auto& feature_map = rawDataHandler.getFeatureMap();
  ASSERT_EQ(feature_map.size(), 2);
  const auto& feature_1 = feature_map.at(0);
  EXPECT_FLOAT_EQ(feature_1.getRT(), 404.30066);
  EXPECT_FLOAT_EQ(feature_1.getMZ(), 195.10199);
  EXPECT_EQ(static_cast<int>(feature_1.getMetaValue("ms_level")), 1);

  const auto& feature_2 = feature_map.at(1);
  EXPECT_FLOAT_EQ(feature_2.getRT(), 390.259);
  EXPECT_FLOAT_EQ(feature_2.getMZ(), 149.02496);
  EXPECT_EQ(static_cast<int>(feature_2.getMetaValue("ms_level")), 2);

  // load and check transitions file
  LoadTransitions load_transition;
  filenames.setFullPath("traML", output_file_path);
  load_transition.process(rawDataHandler, params, filenames);
  const auto& transitions = rawDataHandler.getTargetedExperiment().getTransitions();
  ASSERT_EQ(transitions.size(), 1);
  const auto& transition = transitions.at(0);
  EXPECT_EQ(std::string(transition.getName()), std::string("Unknown 1_HMDB:HMDB0000001"));
  EXPECT_FLOAT_EQ(transition.getLibraryIntensity(), 90780);
  EXPECT_EQ(std::string(transition.getPeptideRef()), std::string("HMDB:HMDB0000001"));
  EXPECT_FLOAT_EQ(transition.getPrecursorMZ(), 195.102);
  EXPECT_FLOAT_EQ(transition.getProductMZ(), 149.025);
}

/**
  SearchSpectrumMS2 Tests
*/
TEST(RawDataProcessor, gettersSearchSpectrumMS2)
{
  SearchSpectrumMS2 processor;
  EXPECT_EQ(processor.getName(), "SEARCH_SPECTRUM_MS2");
}

TEST(RawDataProcessor, SearchSpectrumMS2)
{
  Filenames filenames;
  RawDataHandler rawDataHandler;
  ParameterSet params;
  filenames.setTagValue(Filenames::Tag::MAIN_DIR, SMARTPEAK_GET_TEST_DATA_PATH("DDA"));

  filenames.setFullPath("parameters", SMARTPEAK_GET_TEST_DATA_PATH("DDA/parameters.csv"));
  LoadParameters load_parameters;
  load_parameters.process(rawDataHandler, params, filenames);
  params = rawDataHandler.getParameters();

  filenames.setFullPath("mzML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/Germicidin A standard.mzML"));
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  LoadFeatures loadFeatures;
  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_TEST_DATA_PATH("DDA/SearchSpectrumMS2/Germicidin A standard search spectrum ms2.featureXML"));
  loadFeatures.process(rawDataHandler, params, filenames);

  SearchSpectrumMS2 search_spectrum_ms2;
  search_spectrum_ms2.process(rawDataHandler, params, filenames);

  const auto& feature_map = rawDataHandler.getFeatureMap();
  ASSERT_EQ(feature_map.size(), 1);
  const auto& feature_1 = feature_map.at(0);
  EXPECT_EQ(std::string(feature_1.getMetaValue("PeptideRef")), std::string("149.025"));
  const auto& sub_features = feature_1.getSubordinates();
  ASSERT_EQ(sub_features.size(), 1);
  const auto& sub_feature_1 = feature_map.at(0);
  EXPECT_EQ(std::string(sub_feature_1.getMetaValue("PeptideRef")), std::string("149.025"));
}
