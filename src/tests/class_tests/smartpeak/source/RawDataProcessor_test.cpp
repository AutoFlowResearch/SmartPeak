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

#define BOOST_TEST_MODULE RawDataProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <OpenMS/ANALYSIS/OPENSWATH/DATAACCESS/DataAccessHelper.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <OpenMS/ANALYSIS/OPENSWATH/TransitionTSVFile.h>  // load traML
#include <OpenMS/FORMAT/FeatureXMLFile.h>  // load/store featureXML
#include <SmartPeak/io/InputDataValidation.h> // check filenames and headers
#include <OpenMS/FORMAT/TraMLFile.h>

#include <OpenMS/FORMAT/MzMLFile.h>

using namespace SmartPeak;
using namespace std;

void load_data(
  ParameterSet& params_1,
  ParameterSet& params_2
)
{
  Filenames filenames1, filenames2;
  filenames1.parameters_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core_tmpFix.csv");
  filenames2.parameters_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_2_tmpFix.csv");
  RawDataHandler rawDataHandler;
  LoadParameters loadParameters;
  loadParameters.process(rawDataHandler, {}, filenames1);
  params_1 = rawDataHandler.getParameters();
  rawDataHandler.clear();
  loadParameters.process(rawDataHandler, {}, filenames2);
  params_2 = rawDataHandler.getParameters();
}

BOOST_AUTO_TEST_SUITE(rawdataprocessor)

/**
  ClearData Tests
*/
BOOST_AUTO_TEST_CASE(constructorClearData)
{
  ClearData* ptrClearData = nullptr;
  ClearData* nullPointerClearData = nullptr;
  BOOST_CHECK_EQUAL(ptrClearData, nullPointerClearData);
}

BOOST_AUTO_TEST_CASE(destructorClearData)
{
  ClearData* ptrClearData = nullptr;
  ptrClearData = new ClearData();
  delete ptrClearData;
}

BOOST_AUTO_TEST_CASE(gettersClearData)
{
  ClearData processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "CLEAR_DATA");
}

BOOST_AUTO_TEST_CASE(processorClearData)
{
  // Load all of the raw and processed data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  LoadRawData::extractMetaData(rawDataHandler);
  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_test_1_io_FileReaderOpenMS.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Control
  BOOST_CHECK_GT(rawDataHandler.getExperiment().getChromatograms().size(), 0);
  BOOST_CHECK_GT(rawDataHandler.getFeatureMapHistory().size(), 0);
  BOOST_CHECK(!rawDataHandler.getMetaData().getFilename().empty());

  ClearData clearData;
  clearData.process(rawDataHandler, params_1, filenames);
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().getChromatograms().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 0);
  BOOST_CHECK(!rawDataHandler.getMetaData().getFilename().empty());
}

/**
  LoadRawData Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadRawData)
{
  LoadRawData* ptrLoadRawData = nullptr;
  LoadRawData* nullPointerLoadRawData = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadRawData, nullPointerLoadRawData);
}

BOOST_AUTO_TEST_CASE(destructorLoadRawData)
{
  LoadRawData* ptrLoadRawData = nullptr;
  ptrLoadRawData = new LoadRawData();
  delete ptrLoadRawData;
}

BOOST_AUTO_TEST_CASE(gettersLoadRawData)
{
  LoadRawData processor;

  BOOST_CHECK_EQUAL(processor.getID(), 1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_RAW_DATA");
}

BOOST_AUTO_TEST_CASE(processorLoadRawData)
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
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML");
  processor.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getExperiment().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms1.size(), 2);

  BOOST_CHECK_CLOSE(chromatograms1[0][0].getIntensity(), 3.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][1].getIntensity(), 4.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][2].getIntensity(), 6.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][3].getIntensity(), 19.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[0][4].getIntensity(), 1.0, 1e-3);

  BOOST_CHECK_CLOSE(chromatograms1[1][0].getIntensity(), 3.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][1].getIntensity(), 4.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][2].getIntensity(), -2.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][3].getIntensity(), -8.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1[1][4].getIntensity(), 1.0, 1e-3);

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
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_ChromeleonFile_10ug.txt");
  processor.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms3 = rawDataHandler.getExperiment().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms3.size(), 1);
  BOOST_CHECK_EQUAL(chromatograms3[0].size(), 3301);

  BOOST_CHECK_CLOSE(chromatograms3[0][0].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][600].getIntensity(), -503.815, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][1200].getIntensity(), -666.694, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][1800].getIntensity(), -232.843, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][2400].getIntensity(), -223.644, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms3[0][3300].getIntensity(), 126.958, 1e-3);
}

BOOST_AUTO_TEST_CASE(extractMetaData)
{
  // Pre-requisites: load the parameters
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  BOOST_CHECK_EQUAL(params_1.size(), 31);
  BOOST_CHECK_EQUAL(params_2.size(), 28);
  RawDataHandler rawDataHandler;

  // Pre-requisites: load the transitions and raw data
  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, {}, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);

  // Test 1
  LoadRawData::extractMetaData(rawDataHandler);

  const MetaDataHandler& metaDataHandler = rawDataHandler.getMetaData();

  string filename = metaDataHandler.getFilename();
  filename = filename.substr(filename.find("src/tests")); // otherwise it would contain /home/username/SmartPeak2/ on Linux
  BOOST_CHECK_EQUAL(filename, "src/tests/class_tests/smartpeak/data/RawDataProcessor_mzML_1.mzML");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "150601_0_BloodProject01_PLT_QC_Broth-1");

  BOOST_CHECK_EQUAL(metaDataHandler.proc_method_name, "Analyst");
  BOOST_CHECK_EQUAL(metaDataHandler.instrument, "QTRAP 5500");
  // BOOST_CHECK_EQUAL(rawDataHandler.acq_method_name, "");
  BOOST_CHECK_EQUAL(metaDataHandler.operator_name, "");
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_mday, 10);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_mon, 06);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_year, 2015);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_hour, 01);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_min, 14);
  BOOST_CHECK_EQUAL(metaDataHandler.acquisition_date_and_time.tm_sec, 10);
}

/**
  StoreRawData Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreRawData)
{
  StoreRawData* ptrStoreRawData = nullptr;
  StoreRawData* nullPointerStoreRawData = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreRawData, nullPointerStoreRawData);
}

BOOST_AUTO_TEST_CASE(destructorStoreRawData)
{
  StoreRawData* ptrStoreRawData = nullptr;
  ptrStoreRawData = new StoreRawData();
  delete ptrStoreRawData;
}

BOOST_AUTO_TEST_CASE(gettersStoreRawData)
{
  StoreRawData processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_RAW_DATA");
}

BOOST_AUTO_TEST_CASE(processStoreRawData)
{
  // no tests, it wraps OpenMS store function
}

/**
  MapChromatograms Tests
*/
BOOST_AUTO_TEST_CASE(constructorMapChromatograms)
{
  MapChromatograms* ptrMapChromatograms = nullptr;
  MapChromatograms* nullPointerMapChromatograms = nullptr;
  BOOST_CHECK_EQUAL(ptrMapChromatograms, nullPointerMapChromatograms);
}

BOOST_AUTO_TEST_CASE(destructorMapChromatograms)
{
  MapChromatograms* ptrMapChromatograms = nullptr;
  ptrMapChromatograms = new MapChromatograms();
  delete ptrMapChromatograms;
}

BOOST_AUTO_TEST_CASE(gettersMapChromatograms)
{
  MapChromatograms processor;

  BOOST_CHECK_EQUAL(processor.getID(), 11);
  BOOST_CHECK_EQUAL(processor.getName(), "MAP_CHROMATOGRAMS");
}

BOOST_AUTO_TEST_CASE(processorMapChromatograms)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Test map chromatograms
  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getChromatogramMap().getChromatograms();

  BOOST_CHECK_EQUAL(chromatograms1.size(), 324);

  BOOST_CHECK_EQUAL(chromatograms1.front().getNativeID(), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_CLOSE(chromatograms1.front().getPrecursor().getMZ(), 179, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.front().getProduct().getMZ(), 136, 1e-3);

  BOOST_CHECK_EQUAL(chromatograms1.back().getNativeID(), "nadph.nadph_2.Light");
  BOOST_CHECK_CLOSE(chromatograms1.back().getPrecursor().getMZ(), 744, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back().getProduct().getMZ(), 79, 1e-3);
}

/**
  ZeroChromatogramBaseline Tests
*/
BOOST_AUTO_TEST_CASE(constructorZeroChromatogramBaseline)
{
  ZeroChromatogramBaseline* ptrZeroChromatogramBaseline = nullptr;
  ZeroChromatogramBaseline* nullPointerZeroChromatogramBaseline = nullptr;
  BOOST_CHECK_EQUAL(ptrZeroChromatogramBaseline, nullPointerZeroChromatogramBaseline);
}

BOOST_AUTO_TEST_CASE(destructorZeroChromatogramBaseline)
{
  ZeroChromatogramBaseline* ptrZeroChromatogramBaseline = nullptr;
  ptrZeroChromatogramBaseline = new ZeroChromatogramBaseline();
  delete ptrZeroChromatogramBaseline;
}

BOOST_AUTO_TEST_CASE(gettersZeroChromatogramBaseline)
{
  ZeroChromatogramBaseline processor;

  BOOST_CHECK_EQUAL(processor.getID(), 12);
  BOOST_CHECK_EQUAL(processor.getName(), "ZERO_CHROMATOGRAM_BASELINE");
}

BOOST_AUTO_TEST_CASE(processorZeroChromatogramBaseline)
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
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_baseline_correction.mzML");
  LoadRawData processor;
  processor.process(rawDataHandler, params_I, filenames);
  rawDataHandler.setChromatogramMap(rawDataHandler.getExperiment()); // Avoiding the mapping step for testing purposes
  ZeroChromatogramBaseline zeroChromBase;
  zeroChromBase.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms2 = rawDataHandler.getChromatogramMap().getChromatograms();
  BOOST_CHECK_EQUAL(chromatograms2.size(), 2);
  BOOST_CHECK_CLOSE(chromatograms2[0][0].getIntensity(), 2.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][1].getIntensity(), 3.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][2].getIntensity(), 5.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][3].getIntensity(), 18.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[0][4].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][0].getIntensity(), 11.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][1].getIntensity(), 12.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][2].getIntensity(), 6.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][3].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2[1][4].getIntensity(), 9.0, 1e-3);

  // TEST CASE 2:  Chromeleon file format with baseline correction
  std::vector<std::map<std::string, std::string>> params = { {
      {"name", "format"},
      {"type", "string"},
      {"value", "ChromeleonFile"}
  }};
  params_I.addFunctionParameters(FunctionParameters("mzML", params));
  rawDataHandler.clear();
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_ChromeleonFile_10ug.txt");
  processor.process(rawDataHandler, params_I, filenames);
  rawDataHandler.setChromatogramMap(rawDataHandler.getExperiment()); // Avoiding the mapping step for testing purposes
  zeroChromBase.process(rawDataHandler, params_I, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms4 = rawDataHandler.getChromatogramMap().getChromatograms();
  BOOST_CHECK_EQUAL(chromatograms4.size(), 1);
  BOOST_CHECK_EQUAL(chromatograms4[0].size(), 3301);
  BOOST_CHECK_CLOSE(chromatograms4[0][0].getIntensity(), 1004.634, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][600].getIntensity(), 500.819, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][1200].getIntensity(), 337.94, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][1800].getIntensity(), 771.791, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][2400].getIntensity(), 780.99, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms4[0][3300].getIntensity(), 1131.592, 1e-3);
}

/**
  ExtractChromatogramWindows Tests
*/
BOOST_AUTO_TEST_CASE(constructorExtractChromatogramWindows)
{
  ExtractChromatogramWindows* ptrExtractChromatogramWindows = nullptr;
  ExtractChromatogramWindows* nullPointerExtractChromatogramWindows = nullptr;
  BOOST_CHECK_EQUAL(ptrExtractChromatogramWindows, nullPointerExtractChromatogramWindows);
}

BOOST_AUTO_TEST_CASE(destructorExtractChromatogramWindows)
{
  ExtractChromatogramWindows* ptrExtractChromatogramWindows = nullptr;
  ptrExtractChromatogramWindows = new ExtractChromatogramWindows();
  delete ptrExtractChromatogramWindows;
}

BOOST_AUTO_TEST_CASE(gettersExtractChromatogramWindows)
{
  ExtractChromatogramWindows processor;

  BOOST_CHECK_EQUAL(processor.getID(), 13);
  BOOST_CHECK_EQUAL(processor.getName(), "EXTRACT_CHROMATOGRAM_WINDOWS");
}

BOOST_AUTO_TEST_CASE(processorExtractChromatogramWindows)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Control
  const vector<OpenMS::MSChromatogram>& chromatograms1 = rawDataHandler.getChromatogramMap().getChromatograms();
  BOOST_CHECK_EQUAL(chromatograms1.size(), 324);
  BOOST_CHECK_CLOSE(chromatograms1.front()[0].getIntensity(), 158.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.front()[0].getMZ(), 38.621, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.front()[1].getIntensity(), 211, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.front()[1].getMZ(), 38.7209999, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.front()[2].getIntensity(), 158, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.front()[2].getMZ(), 38.82, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back()[0].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back()[0].getMZ(), 912.233, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back()[1].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back()[1].getMZ(), 913.1870, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back()[2].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms1.back()[2].getMZ(), 914.139, 1e-3);

  // Test window extraction
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_extractChromWindowTest_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, {}, filenames);

  ExtractChromatogramWindows extractChromWin;
  extractChromWin.process(rawDataHandler, {}, filenames);

  const vector<OpenMS::MSChromatogram>& chromatograms2 = rawDataHandler.getChromatogramMap().getChromatograms();
  BOOST_CHECK_EQUAL(chromatograms2.size(), 324);
  BOOST_CHECK_CLOSE(chromatograms2.front()[0].getIntensity(), 158.0, 1e-3); // No filtering; within the RT window
  BOOST_CHECK_CLOSE(chromatograms2.front()[0].getMZ(), 38.621, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.front()[1].getIntensity(), 211, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.front()[1].getMZ(), 38.7209999, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.front()[2].getIntensity(), 158, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.front()[2].getMZ(), 38.82, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.back()[0].getIntensity(), 0.0, 1e-3); // Filtering; outside of the RT window
  BOOST_CHECK_CLOSE(chromatograms2.back()[0].getMZ(), 951.20299, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.back()[1].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.back()[1].getMZ(), 952.31299, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.back()[2].getIntensity(), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(chromatograms2.back()[2].getMZ(), 953.40699, 1e-3);
}

/**
  ExtractSpectraWindows Tests
*/
BOOST_AUTO_TEST_CASE(constructorExtractSpectraWindows)
{
  ExtractSpectraWindows* ptrExtractSpectraWindows = nullptr;
  ExtractSpectraWindows* nullPointerExtractSpectraWindows = nullptr;
  BOOST_CHECK_EQUAL(ptrExtractSpectraWindows, nullPointerExtractSpectraWindows);
}

BOOST_AUTO_TEST_CASE(destructorExtractSpectraWindows)
{
  ExtractSpectraWindows* ptrExtractSpectraWindows = nullptr;
  ptrExtractSpectraWindows = new ExtractSpectraWindows();
  delete ptrExtractSpectraWindows;
}

BOOST_AUTO_TEST_CASE(gettersExtractSpectraWindows)
{
  ExtractSpectraWindows processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "EXTRACT_SPECTRA_WINDOWS");
}

BOOST_AUTO_TEST_CASE(processorExtractSpectraWindows)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Control
  const vector<OpenMS::MSSpectrum>& spectra1 = rawDataHandler.getExperiment().getSpectra();
  BOOST_CHECK_EQUAL(spectra1.size(), 873);
  BOOST_CHECK_CLOSE(spectra1.front().getRT(), 0.56525150400000002, 1e-3);
  BOOST_CHECK_CLOSE(spectra1.back().getRT(), 300.56175465600001, 1e-3);

  // Test window extraction
  ExtractSpectraWindows extractSpectraWin;
  extractSpectraWin.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSSpectrum>& spectra2 = rawDataHandler.getExperiment().getSpectra();
  BOOST_CHECK_EQUAL(spectra2.size(), 173);
  BOOST_CHECK_CLOSE(spectra2.front().getRT(), 0.56525150400000002, 1e-3);
  BOOST_CHECK_CLOSE(spectra2.back().getRT(), 59.74130393598, 1e-3);
}

/**
  MergeSpectra Tests
*/
BOOST_AUTO_TEST_CASE(constructorMergeSpectra)
{
  MergeSpectra* ptrMergeSpectra = nullptr;
  MergeSpectra* nullPointerMergeSpectra = nullptr;
  BOOST_CHECK_EQUAL(ptrMergeSpectra, nullPointerMergeSpectra);
}

BOOST_AUTO_TEST_CASE(destructorMergeSpectra)
{
  MergeSpectra* ptrMergeSpectra = nullptr;
  ptrMergeSpectra = new MergeSpectra();
  delete ptrMergeSpectra;
}

BOOST_AUTO_TEST_CASE(gettersMergeSpectra)
{
  MergeSpectra processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "MERGE_SPECTRA");
}

BOOST_AUTO_TEST_CASE(processorMergeSpectra)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Control
  const vector<OpenMS::MSSpectrum>& spectra1 = rawDataHandler.getExperiment().getSpectra();
  BOOST_CHECK_EQUAL(spectra1.size(), 873);

  // Test merge spectra
  MergeSpectra mergeSpectra;
  mergeSpectra.process(rawDataHandler, params_1, filenames);

  const vector<OpenMS::MSSpectrum>& spectra2 = rawDataHandler.getExperiment().getSpectra();
  BOOST_CHECK_EQUAL(spectra2.size(), 1);
  BOOST_CHECK_CLOSE(spectra2.front().getRT(), -1, 1e-3);
  BOOST_CHECK_EQUAL(spectra2.front().size(), 240);
  BOOST_CHECK_EQUAL(spectra2.front().getNativeID(), "MergeSpectra");
  BOOST_CHECK_EQUAL(spectra2.front().front().getMZ(), 109.95009243262952);
  BOOST_CHECK_EQUAL(spectra2.front().back().getMZ(), 109.99988410050186);
  BOOST_CHECK_EQUAL(spectra2.front().front().getIntensity(), 0);
  BOOST_CHECK_EQUAL(spectra2.front().back().getIntensity(), 3236006.75);
}

/**
  LoadFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatures)
{
  LoadFeatures* ptrLoadFeatures = nullptr;
  LoadFeatures* nullPointerLoadFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatures, nullPointerLoadFeatures);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatures)
{
  LoadFeatures* ptrLoadFeatures = nullptr;
  ptrLoadFeatures = new LoadFeatures();
  delete ptrLoadFeatures;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatures)
{
  LoadFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 2);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURES");
}

BOOST_AUTO_TEST_CASE(processLoadFeatures)
{
  Filenames filenames;
  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_test_1_io_FileReaderOpenMS.featureXML");
  RawDataHandler rawDataHandler;
  rawDataHandler.getMetaData().setFilename("filename");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, {}, filenames);

  const OpenMS::FeatureMap& fm = rawDataHandler.getFeatureMap(); // Test feature_map
  BOOST_CHECK_EQUAL(fm.size(), 481);
  OpenMS::StringList filename_test;
  fm.getPrimaryMSRunPath(filename_test);
  BOOST_CHECK_EQUAL(filename_test.at(0), "filename");

  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getSubordinates()[0].getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getSubordinates()[0].getRT()), 15.8944563381195, 1e-6);
  BOOST_CHECK_EQUAL(fm[0].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getMetaValue("peak_apices_sum")), 583315.0, 1e-6);
  BOOST_CHECK_EQUAL(fm[0].getMetaValue("PeptideRef").toString(), "23dpg");

  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getSubordinates()[0].getMetaValue("peak_apex_int")), 3436.0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getSubordinates()[0].getRT()), 16.2997193464915, 1e-6);
  BOOST_CHECK_EQUAL(fm[1].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getMetaValue("peak_apices_sum")), 13859.0, 1e-6);
  BOOST_CHECK_EQUAL(fm[1].getMetaValue("PeptideRef").toString(), "23dpg");

  const OpenMS::FeatureMap& fmh = rawDataHandler.getFeatureMapHistory(); // Test feature_map_history
  BOOST_CHECK_EQUAL(fmh.size(), 481);
  fm.getPrimaryMSRunPath(filename_test);
  BOOST_CHECK_EQUAL(filename_test.at(0), "filename");

  BOOST_CHECK_CLOSE(static_cast<double>(fmh[0].getSubordinates()[0].getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(fmh[0].getSubordinates()[0].getRT()), 15.8944563381195, 1e-6);
  BOOST_CHECK_EQUAL(fmh[0].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fmh[0].getMetaValue("peak_apices_sum")), 583315.0, 1e-6);
  BOOST_CHECK_EQUAL(fmh[0].getMetaValue("PeptideRef").toString(), "23dpg");
  BOOST_CHECK(fmh[0].getSubordinates()[0].getMetaValue("used_").toBool());

  BOOST_CHECK_CLOSE(static_cast<double>(fmh[1].getSubordinates()[0].getMetaValue("peak_apex_int")), 3436.0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(fmh[1].getSubordinates()[0].getRT()), 16.2997193464915, 1e-6);
  BOOST_CHECK_EQUAL(fmh[1].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fmh[1].getMetaValue("peak_apices_sum")), 13859.0, 1e-6);
  BOOST_CHECK_EQUAL(fmh[1].getMetaValue("PeptideRef").toString(), "23dpg");
  BOOST_CHECK(fmh[1].getSubordinates()[0].getMetaValue("used_").toBool());
}

/**
  StoreFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatures)
{
  StoreFeatures* ptrStoreFeatures = nullptr;
  StoreFeatures* nullPointerStoreFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatures, nullPointerStoreFeatures);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatures)
{
  StoreFeatures* ptrStoreFeatures = nullptr;
  ptrStoreFeatures = new StoreFeatures();
  delete ptrStoreFeatures;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatures)
{
  StoreFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 9);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURES");
}

BOOST_AUTO_TEST_CASE(processStoreFeatures)
{
  // no tests, it wraps OpenMS store function
}

/**
  LoadAnnotations Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadAnnotations)
{
  LoadAnnotations* ptrLoadAnnotations = nullptr;
  LoadAnnotations* nullPointerLoadAnnotations = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadAnnotations, nullPointerLoadAnnotations);
}

BOOST_AUTO_TEST_CASE(destructorLoadAnnotations)
{
  LoadAnnotations* ptrLoadAnnotations = nullptr;
  ptrLoadAnnotations = new LoadAnnotations();
  delete ptrLoadAnnotations;
}

BOOST_AUTO_TEST_CASE(gettersLoadAnnotations)
{
  LoadAnnotations processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_ANNOTATIONS");
}

BOOST_AUTO_TEST_CASE(processLoadAnnotations)
{
  Filenames filenames;
  filenames.mzTab_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest.mzTab");
  RawDataHandler rawDataHandler;
  LoadAnnotations loadAnnotations;
  loadAnnotations.process(rawDataHandler, {}, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().size(), 108);
  BOOST_CHECK_CLOSE(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().calc_mass_to_charge.get(), 500.02851705499575, 1e-6);
  BOOST_CHECK_EQUAL(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().chemical_formula.get()), "C82H156O17P2");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().identifier.get().at(0).get(), "HMDB:HMDB0073108");
  BOOST_CHECK_CLOSE(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().back().calc_mass_to_charge.get(), 799.94828338599575, 1e-6);
  BOOST_CHECK_EQUAL(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().back().chemical_formula.get()), "C76H135N3O29");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().back().identifier.get().at(0).get(), "HMDB:HMDB0011867");
}

/**
  StoreAnnotations Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreAnnotations)
{
  StoreAnnotations* ptrStoreAnnotations = nullptr;
  StoreAnnotations* nullPointerStoreAnnotations = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreAnnotations, nullPointerStoreAnnotations);
}

BOOST_AUTO_TEST_CASE(destructorStoreAnnotations)
{
  StoreAnnotations* ptrStoreAnnotations = nullptr;
  ptrStoreAnnotations = new StoreAnnotations();
  delete ptrStoreAnnotations;
}

BOOST_AUTO_TEST_CASE(gettersStoreAnnotations)
{
  StoreAnnotations processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_ANNOTATIONS");
}

BOOST_AUTO_TEST_CASE(processStoreAnnotations)
{
  // no tests, it wraps OpenMS store function
}

/**
  LoadTransitions Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadTransitions)
{
  LoadTransitions* ptrLoadTransitions = nullptr;
  LoadTransitions* nullPointerLoadTransitions = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadTransitions, nullPointerLoadTransitions);
}

BOOST_AUTO_TEST_CASE(destructorLoadTransitions)
{
  LoadTransitions* ptrLoadTransitions = nullptr;
  ptrLoadTransitions = new LoadTransitions();
  delete ptrLoadTransitions;
}

BOOST_AUTO_TEST_CASE(gettersLoadTransitions)
{
  LoadTransitions processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_TRANSITIONS");
}

BOOST_AUTO_TEST_CASE(processLoadTransitions)
{
  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  RawDataHandler rawDataHandler;
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, {}, filenames);
  const std::vector<OpenMS::ReactionMonitoringTransition>& t = rawDataHandler.getTargetedExperiment().getTransitions();

  BOOST_CHECK_EQUAL(t.size(), 324);

  BOOST_CHECK_EQUAL(t[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_CLOSE(t[0].getPrecursorMZ(), 179.0, 1e-6);
  BOOST_CHECK_CLOSE(t[0].getProductMZ(), 136.0, 1e-6);

  BOOST_CHECK_EQUAL(t[10].getPeptideRef(), "citr-L");
  BOOST_CHECK_CLOSE(t[10].getPrecursorMZ(), 180.0, 1e-6);
  BOOST_CHECK_CLOSE(t[10].getProductMZ(), 136.0, 1e-6);

  BOOST_CHECK_EQUAL(t[19].getPeptideRef(), "Lcystin");
  BOOST_CHECK_CLOSE(t[19].getPrecursorMZ(), 239.0, 1e-6);
  BOOST_CHECK_CLOSE(t[19].getProductMZ(), 120.0, 1e-6);
}

/**
  LoadFeatureFiltersRDP Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureFilters)
{
  LoadFeatureFiltersRDP* ptrLoadFeatureFilters = nullptr;
  LoadFeatureFiltersRDP* nullPointerLoadFeatureFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureFilters, nullPointerLoadFeatureFilters);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureFilters)
{
  LoadFeatureFiltersRDP* ptrLoadFeatureFilters = nullptr;
  ptrLoadFeatureFilters = new LoadFeatureFiltersRDP();
  delete ptrLoadFeatureFilters;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureFilters)
{
  LoadFeatureFiltersRDP processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_FILTERS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureFilters)
{
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureQCsRDP Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureQCs)
{
  LoadFeatureQCsRDP* ptrLoadFeatureQCs = nullptr;
  LoadFeatureQCsRDP* nullPointerLoadFeatureQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureQCs, nullPointerLoadFeatureQCs);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureQCs)
{
  LoadFeatureQCsRDP* ptrLoadFeatureQCs = nullptr;
  ptrLoadFeatureQCs = new LoadFeatureQCsRDP();
  delete ptrLoadFeatureQCs;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureQCs)
{
  LoadFeatureQCsRDP processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_QCS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureQCs)
{
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureFiltersRDP Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureFilters)
{
  StoreFeatureFiltersRDP* ptrStoreFeatureFilters = nullptr;
  StoreFeatureFiltersRDP* nullPointerStoreFeatureFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureFilters, nullPointerStoreFeatureFilters);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureFilters)
{
  StoreFeatureFiltersRDP* ptrStoreFeatureFilters = nullptr;
  ptrStoreFeatureFilters = new StoreFeatureFiltersRDP();
  delete ptrStoreFeatureFilters;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureFilters)
{
  StoreFeatureFiltersRDP processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_FILTERS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureFilters)
{
  RawDataHandler rawDataHandler, rawDataHandler_test;

  Filenames filenames;
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, {}, filenames);
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureFiltersRDP storeFeatureFilters;
  storeFeatureFilters.process(rawDataHandler, {}, filenames);
  loadFeatureFilters.process(rawDataHandler_test, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureFilter();
  const OpenMS::MRMFeatureQC& fQC_test = rawDataHandler_test.getFeatureFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureQCsRDP Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureQCs)
{
  StoreFeatureQCsRDP* ptrStoreFeatureQCs = nullptr;
  StoreFeatureQCsRDP* nullPointerStoreFeatureQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureQCs, nullPointerStoreFeatureQCs);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureQCs)
{
  StoreFeatureQCsRDP* ptrStoreFeatureQCs = nullptr;
  ptrStoreFeatureQCs = new StoreFeatureQCsRDP();
  delete ptrStoreFeatureQCs;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureQCs)
{
  StoreFeatureQCsRDP processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_QCS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureQCs)
{
  RawDataHandler rawDataHandler, rawDataHandler_test;

  Filenames filenames;
  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, {}, filenames);
  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureQCsRDP storeFeatureQCs;
  storeFeatureQCs.process(rawDataHandler, {}, filenames);
  loadFeatureQCs.process(rawDataHandler_test, {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = rawDataHandler.getFeatureQC();
  const OpenMS::MRMFeatureQC& fQC_test = rawDataHandler_test.getFeatureQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadValidationData Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadValidationData)
{
  LoadValidationData* ptrLoadValidationData = nullptr;
  LoadValidationData* nullPointerLoadValidationData = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadValidationData, nullPointerLoadValidationData);
}

BOOST_AUTO_TEST_CASE(destructorLoadValidationData)
{
  LoadValidationData* ptrLoadValidationData = nullptr;
  ptrLoadValidationData = new LoadValidationData();
  delete ptrLoadValidationData;
}

BOOST_AUTO_TEST_CASE(gettersLoadValidationData)
{
  LoadValidationData processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_VALIDATION_DATA");
}

BOOST_AUTO_TEST_CASE(processLoadValidationData)
{
  Filenames filenames;
  filenames.referenceData_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
  RawDataHandler rawDataHandler;

  LoadValidationData loadValidationData;
  loadValidationData.process(rawDataHandler, {}, filenames);
  const std::vector<std::map<std::string, CastValue>>& ref_data = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(ref_data.size(), 179);
  BOOST_CHECK_EQUAL(ref_data[0].at("component_name").s_, "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(ref_data[0].at("area").f_, 932543.098, 1e-3);
  BOOST_CHECK_CLOSE(ref_data[0].at("retention_time").f_, static_cast<float>(15.89495171), 1e-1);
  BOOST_CHECK_EQUAL(ref_data[0].at("acquisition_date_and_time").s_, "09-06-2015 17:14");
  // TODO: Should we just use double instead of float? I had to go down to -1 to make the test pass
  BOOST_CHECK_EQUAL(ref_data[178].at("component_name").s_, "xan.xan_1.Light");
  BOOST_CHECK_CLOSE(ref_data[178].at("area").f_, 206951.3035, 1e-3);
  BOOST_CHECK_CLOSE(ref_data[178].at("retention_time").f_, static_cast<float>(1.492980468), 1e-1);
  BOOST_CHECK_EQUAL(ref_data[178].at("acquisition_date_and_time").s_, "09-06-2015 17:14");
}

/**
  LoadParameters Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadParameters)
{
  LoadParameters* ptrLoadParameters = nullptr;
  LoadParameters* nullPointerLoadParameters = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadParameters, nullPointerLoadParameters);
}

BOOST_AUTO_TEST_CASE(destructorLoadParameters)
{
  LoadParameters* ptrLoadParameters = nullptr;
  ptrLoadParameters = new LoadParameters();
  delete ptrLoadParameters;
}

BOOST_AUTO_TEST_CASE(gettersLoadParameters)
{
  LoadParameters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_PARAMETERS");
}

BOOST_AUTO_TEST_CASE(processLoadParameters)
{
  // no tests, it calls FileReader::parseOpenMSParams and LoadParameters::sanitizeRawDataProcessorParameters
}

BOOST_AUTO_TEST_CASE(sanitizeRawDataProcessorParameters)
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
  BOOST_CHECK_EQUAL(params.size(), 26);
  BOOST_CHECK_EQUAL(params.count("SequenceSegmentPlotter"), 1);
  BOOST_CHECK_EQUAL(params.count("FeaturePlotter"), 1);
  BOOST_CHECK_EQUAL(params.count("AbsoluteQuantitation"), 1);
  BOOST_CHECK_EQUAL(params.count("mzML"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMMapping"), 1);
  BOOST_CHECK_EQUAL(params.count("ChromatogramExtractor"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureFinderScoring"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureFilter.filter_MRMFeatures"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureSelector.select_MRMFeatures_qmip"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureSelector.schedule_MRMFeatures_qmip"), 0);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureSelector.select_MRMFeatures_score"), 1);
  BOOST_CHECK_EQUAL(params.count("ReferenceDataMethods.getAndProcess_referenceData_samples"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureValidator.validate_MRMFeatures"), 1);
  BOOST_CHECK_EQUAL(params.count("MRMFeatureFilter.filter_MRMFeatures.qc"), 1);
  BOOST_CHECK_EQUAL(params.count("SequenceProcessor"), 1);
  BOOST_CHECK_EQUAL(params.count("FIAMS"), 1);
  BOOST_CHECK_EQUAL(params.count("PickMS1Features"), 1);
  BOOST_CHECK_EQUAL(params.count("PickMS2Features"), 1);
  BOOST_CHECK_EQUAL(params.count("AccurateMassSearchEngine"), 1);
  BOOST_CHECK_EQUAL(params.count("MergeInjections"), 1);
  BOOST_CHECK_EQUAL(params.at("SequenceSegmentPlotter").size(), 3);
  BOOST_CHECK_EQUAL(params.at("MRMFeatureFilter.filter_MRMFeatures.qc").size(), 0);
  BOOST_CHECK_EQUAL(params.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences").size(), 0);
  BOOST_CHECK_EQUAL(params.at("MRMFeatureFilter.filter_MRMFeaturesBackgroundInterferences.qc").size(), 0);
  BOOST_CHECK_EQUAL(params.at("MRMFeatureFilter.filter_MRMFeaturesRSDs").size(), 0);
  BOOST_CHECK_EQUAL(params.at("MRMFeatureFilter.filter_MRMFeaturesRSDs.qc").size(), 0);
  BOOST_CHECK_EQUAL(params.at("SequenceProcessor").size(), 0);
  BOOST_CHECK_EQUAL(params.at("FIAMS").size(), 0);
  BOOST_CHECK_EQUAL(params.at("PickMS1Features").size(), 0);
  BOOST_CHECK_EQUAL(params.at("PickMS2Features").size(), 0);
  BOOST_CHECK_EQUAL(params.at("AccurateMassSearchEngine").size(), 0);
  BOOST_CHECK_EQUAL(params.at("MergeInjections").size(), 0);
}

/**
  PickMRMFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorPickMRMFeatures)
{
  PickMRMFeatures* ptrPickFeatures = nullptr;
  PickMRMFeatures* nullPointerPickFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrPickFeatures, nullPointerPickFeatures);
}

BOOST_AUTO_TEST_CASE(destructorPickMRMFeatures)
{
  PickMRMFeatures* ptrPickFeatures = nullptr;
  ptrPickFeatures = new PickMRMFeatures();
  delete ptrPickFeatures;
}

BOOST_AUTO_TEST_CASE(gettersPickMRMFeatures)
{
  PickMRMFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 3);
  BOOST_CHECK_EQUAL(processor.getName(), "PICK_MRM_FEATURES");
}

BOOST_AUTO_TEST_CASE(pickFeaturesMRM)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Test pick features
  PickMRMFeatures pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 481);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0]; // feature_map_
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 1067.5447296543123, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0]; // feature_map_history_
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(hsubordinate2.getMetaValue("used_").toBool());
}

/**
  PickMS1Features Tests
*/
BOOST_AUTO_TEST_CASE(constructorPickMS1Features)
{
  PickMS1Features* ptrPickFeatures = nullptr;
  PickMS1Features* nullPointerPickFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrPickFeatures, nullPointerPickFeatures);
}

BOOST_AUTO_TEST_CASE(destructorPickMS1Features)
{
  PickMS1Features* ptrPickFeatures = nullptr;
  ptrPickFeatures = new PickMS1Features();
  delete ptrPickFeatures;
}

BOOST_AUTO_TEST_CASE(gettersPickMS1Features)
{
  PickMS1Features processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "PICK_MS1_FEATURES");
}

BOOST_AUTO_TEST_CASE(pickMS1Features)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_SerumTest_merged.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);
  rawDataHandler.getMetaData().scan_polarity = "positive";

  // Test pick features
  PickMS1Features pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 10);

  const OpenMS::Feature& feature1 = rawDataHandler.getFeatureMap().at(0); // feature_map_
  BOOST_CHECK_EQUAL(feature1.getMetaValue("native_id"), "spectrum=0");
  BOOST_CHECK_EQUAL(feature1.getMetaValue("PeptideRef").toString(), "Unknown");
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getMetaValue("peak_apex_int")), 1930.90576171875, 1e-6);
  BOOST_CHECK_EQUAL(feature1.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getMetaValue("leftWidth")), 109.95009243262952, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getMetaValue("rightWidth")), 109.95238409935168, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getConvexHull().getHullPoints().at(0).getX()), 109.95009243262952, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getMZ()), 109.95124192810006, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getIntensity()), 2.2940683364868164, 1e-6);

  const OpenMS::Feature& feature2 = rawDataHandler.getFeatureMap().back();
  BOOST_CHECK_EQUAL(feature2.getMetaValue("PeptideRef").toString(), "Unknown");
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getMetaValue("peak_apex_int")), 1564.805908203125, 1e-6);
  BOOST_CHECK_EQUAL(feature2.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getMetaValue("leftWidth")), 109.99321743367376, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getMetaValue("rightWidth")), 109.99550910039592, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getConvexHull().getHullPoints().at(0).getX()), 109.99321743367376, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getMZ()), 109.99435797732117, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getIntensity()), 2.3054807186126709, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 10);

  const OpenMS::Feature& hfeature1 = rawDataHandler.getFeatureMapHistory().at(0); // feature_map_history_
  BOOST_CHECK_EQUAL(hfeature1.getMetaValue("native_id"), "spectrum=0");
  BOOST_CHECK_EQUAL(hfeature1.getMetaValue("PeptideRef").toString(), "Unknown");
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getMetaValue("peak_apex_int")), 1930.90576171875, 1e-6);
  BOOST_CHECK_EQUAL(hfeature1.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getMetaValue("leftWidth")), 109.95009243262952, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getMetaValue("rightWidth")), 109.95238409935168, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getConvexHull().getHullPoints().at(0).getX()), 109.95009243262952, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getMZ()), 109.95124192810006, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getIntensity()), 2.2940683364868164, 1e-6);

  const OpenMS::Feature& hfeature2 = rawDataHandler.getFeatureMapHistory().back();
  BOOST_CHECK_EQUAL(hfeature2.getMetaValue("PeptideRef").toString(), "Unknown");
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getMetaValue("logSN")), 10.439937656615268, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getMetaValue("peak_apex_int")), 1564.805908203125, 1e-6);
  BOOST_CHECK_EQUAL(hfeature2.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getMetaValue("leftWidth")), 109.99321743367376, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getMetaValue("rightWidth")), 109.99550910039592, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getConvexHull().getHullPoints().at(0).getX()), 109.99321743367376, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getConvexHull().getHullPoints().at(0).getY()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getMZ()), 109.99435797732117, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getIntensity()), 2.3054807186126709, 1e-6);
}

/**
  PickMS2Features Tests
*/
BOOST_AUTO_TEST_CASE(constructorPickMS2Features)
{
  PickMS2Features* ptrPickFeatures = nullptr;
  PickMS2Features* nullPointerPickFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrPickFeatures, nullPointerPickFeatures);
}

BOOST_AUTO_TEST_CASE(destructorPickMS2Features)
{
  PickMS2Features* ptrPickFeatures = nullptr;
  ptrPickFeatures = new PickMS2Features();
  delete ptrPickFeatures;
}

BOOST_AUTO_TEST_CASE(gettersPickMS2Features)
{
  PickMS2Features processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "PICK_MS2_FEATURES");
}

BOOST_AUTO_TEST_CASE(pickMS2Features)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_germicidin.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  // Test pick features
  PickMS2Features pickFeatures;
  map<std::string, vector<map<string, string>>> feat_params_struct({
  {"FeatureFindingMetabo", {
    { {"name", "report_chromatograms"}, {"type", "bool"}, {"value", "true"} },
    { {"name", "report_convex_hulls"}, {"type", "bool"}, {"value", "true"} },
  }}
  });
  ParameterSet feat_params(feat_params_struct);
  params_1.merge(feat_params);

  pickFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 2258);
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().getChromatograms().size(), 2258);

  const OpenMS::Feature& feature1 = rawDataHandler.getFeatureMap().at(0); // feature_map_
  BOOST_CHECK_EQUAL(feature1.getMetaValue("num_of_masstraces").toString(), "1");
  BOOST_CHECK_EQUAL(feature1.getMetaValue("scan_polarity"), "positive");
  BOOST_REQUIRE(feature1.getConvexHulls().size() == 1);
  BOOST_CHECK_CLOSE(feature1.getConvexHull().getBoundingBox().minX(), 381.298f, 1e-3);
  BOOST_CHECK_CLOSE(feature1.getConvexHull().getBoundingBox().minY(), 79.021f, 1e-3);
  BOOST_CHECK_CLOSE(feature1.getConvexHull().getBoundingBox().maxX(), 540.557f, 1e-3);
  BOOST_CHECK_CLOSE(feature1.getConvexHull().getBoundingBox().maxY(), 79.023f, 1e-3);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getRT()), 453.462, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getMZ()), 79.022321098842482, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature1.getIntensity()), 7978.17578125, 1e-6);

  const OpenMS::Feature& feature2 = rawDataHandler.getFeatureMap().back();
  BOOST_CHECK_EQUAL(feature2.getMetaValue("num_of_masstraces").toString(), "1");
  BOOST_REQUIRE(feature2.getConvexHulls().size() == 1);
  BOOST_CHECK_CLOSE(feature2.getConvexHull().getBoundingBox().minX(), 547.524f, 1e-3);
  BOOST_CHECK_CLOSE(feature2.getConvexHull().getBoundingBox().minY(), 848.610f, 1e-3);
  BOOST_CHECK_CLOSE(feature2.getConvexHull().getBoundingBox().maxX(), 592.812f, 1e-3);
  BOOST_CHECK_CLOSE(feature2.getConvexHull().getBoundingBox().maxY(), 848.641f, 1e-3);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getRT()), 568.428, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getMZ()), 848.63375701405562, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(feature2.getIntensity()), 46520.29296875, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 2258);

  const OpenMS::Feature& hfeature1 = rawDataHandler.getFeatureMapHistory().at(0); // feature_map_history_
  BOOST_CHECK_EQUAL(hfeature1.getMetaValue("num_of_masstraces").toString(), "1");
  BOOST_CHECK_EQUAL(hfeature1.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getRT()), 453.462, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getMZ()), 79.022321098842482, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature1.getIntensity()), 7978.17578125, 1e-6);

  const OpenMS::Feature& hfeature2 = rawDataHandler.getFeatureMapHistory().back();
  BOOST_CHECK_EQUAL(hfeature2.getMetaValue("num_of_masstraces").toString(), "1");
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getRT()), 568.428, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getMZ()), 848.63375701405562, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hfeature2.getIntensity()), 46520.29296875, 1e-6);
}

/**
  SearchAccurateMass Tests
*/
BOOST_AUTO_TEST_CASE(constructorSearchAccurateMass)
{
  SearchAccurateMass* ptrPickFeatures = nullptr;
  SearchAccurateMass* nullPointerPickFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrPickFeatures, nullPointerPickFeatures);
}

BOOST_AUTO_TEST_CASE(destructorSearchAccurateMass)
{
  SearchAccurateMass* ptrPickFeatures = nullptr;
  ptrPickFeatures = new SearchAccurateMass();
  delete ptrPickFeatures;
}

BOOST_AUTO_TEST_CASE(gettersSearchAccurateMass)
{
  SearchAccurateMass processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "SEARCH_ACCURATE_MASS");
}

BOOST_AUTO_TEST_CASE(searchAccurateMass)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_serumTest.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Test accurate mass search
  SearchAccurateMass searchAccurateMass;
  searchAccurateMass.process(rawDataHandler, params_1, filenames);

  // DELETE ME
  filenames.featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_serumTest_accurateMassSearch.featureXML");
  StoreFeatures storeFeatures;
  storeFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().size(), 21);
  BOOST_CHECK_CLOSE(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().calc_mass_to_charge.get(), 109.9994567849957, 1e-6);
  BOOST_CHECK_EQUAL(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().chemical_formula.get()), "C9H11NO6S");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().front().identifier.get().at(0).get(), "HMDB:HMDB0062550");
  BOOST_CHECK_CLOSE(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().at(1).calc_mass_to_charge.get(), 109.99915278004569, 1e-6);
  BOOST_CHECK_EQUAL(static_cast<std::string>(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().at(1).chemical_formula.get()), "C6H12O2S2");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getSmallMoleculeSectionRows().at(1).identifier.get().at(0).get(), "HMDB:HMDB0033556");

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 20);

  const auto& hits2 = rawDataHandler.getFeatureMap().at(19);
  BOOST_CHECK_EQUAL(hits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");

  BOOST_CHECK_EQUAL(hits2.getSubordinates().size(), 1);

  const auto& hits2sub = hits2.getSubordinates().at(0);
  BOOST_CHECK_EQUAL(hits2sub.getMetaValue("identifier").toStringList().at(0), "HMDB:HMDB0062550");
  BOOST_CHECK_CLOSE(static_cast<double>(hits2sub.getMetaValue("peak_apex_int")), 4385.34716796875, 1e-6);
  BOOST_CHECK_EQUAL(hits2sub.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hits2sub.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2sub.getMZ()), 109.99971621401676, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2sub.getIntensity()), 4385.34716796875, 1e-6);
  BOOST_CHECK_EQUAL(hits2sub.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  BOOST_CHECK_EQUAL(hits2sub.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  BOOST_CHECK_EQUAL(static_cast<int>(hits2sub.getCharge()), 3);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 71);

  const auto& hhits2 = rawDataHandler.getFeatureMapHistory().at(70);
  BOOST_CHECK_EQUAL(hhits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");

  BOOST_CHECK_EQUAL(hhits2.getSubordinates().size(), 1);

  const auto& hhits2sub = hhits2.getSubordinates().at(0);
  BOOST_CHECK_EQUAL(hhits2sub.getMetaValue("identifier").toStringList().at(0), "HMDB:HMDB0062550");
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2sub.getMetaValue("peak_apex_int")), 4385.34716796875, 1e-6);
  BOOST_CHECK_EQUAL(hhits2sub.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2sub.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2sub.getMZ()), 109.99971621401676, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2sub.getIntensity()), 4385.34716796875, 1e-6);
  BOOST_CHECK_EQUAL(hhits2sub.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  BOOST_CHECK_EQUAL(hhits2sub.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  BOOST_CHECK_EQUAL(static_cast<int>(hhits2sub.getCharge()), 3);
}

/**
  MergeFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorMergeFeatures)
{
  MergeFeatures* ptrPickFeatures = nullptr;
  MergeFeatures* nullPointerPickFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrPickFeatures, nullPointerPickFeatures);
}

BOOST_AUTO_TEST_CASE(destructorMergeFeatures)
{
  MergeFeatures* ptrPickFeatures = nullptr;
  ptrPickFeatures = new MergeFeatures();
  delete ptrPickFeatures;
}

BOOST_AUTO_TEST_CASE(gettersMergeFeatures)
{
  MergeFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "MERGE_FEATURES");
}

BOOST_AUTO_TEST_CASE(consensusFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_serumTest_accurateMassSearch.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Test accurate mass search
  MergeFeatures makeConsensusFeatures;
  makeConsensusFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 4);
  const auto& hits2 = rawDataHandler.getFeatureMap().at(3);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2.getMetaValue("peak_apex_int")), 75014.837158203125, 1e-6);
  BOOST_CHECK_EQUAL(hits2.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hits2.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2.getMZ()), 109.99967790230706, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2.getIntensity()), 75014.8359375, 1e-6);
  BOOST_CHECK_EQUAL(hits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  BOOST_CHECK_EQUAL(static_cast<int>(hits2.getCharge()), 0);

  BOOST_CHECK_EQUAL(hits2.getSubordinates().size(), 17);
  const auto& hits2_sub1 = hits2.getSubordinates().at(0);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getMetaValue("peak_apex_int")), 1782.034423828125, 1e-6);
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getMZ()), 109.99950621264246, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getIntensity()), 1782.034423828125, 1e-6);
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("identifier").toString(), "[HMDB:HMDB0062550]");
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("description").toString(), "[2-Methoxyacetaminophen sulfate]");
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("modifications").toString(), "M+3Na;3+");
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("adducts").toString(), "+3Na");
  BOOST_CHECK_EQUAL(hits2_sub1.getMetaValue("chemical_formula").toString(), "C9H11NO6S");
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getMetaValue("dc_charge_adduct_mass")), 68.967857984674083, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getMetaValue("mz_error_ppm")), 0.449344462258211, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hits2_sub1.getMetaValue("mz_error_Da")), 4.942764675774924e-05, 1e-6);
  BOOST_CHECK_EQUAL(static_cast<int>(hits2_sub1.getCharge()), 3);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 75);

  const auto& hhits2 = rawDataHandler.getFeatureMapHistory().at(74);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2.getMetaValue("peak_apex_int")), 75014.837158203125, 1e-6);
  BOOST_CHECK_EQUAL(hhits2.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2.getMZ()), 109.99967790230706, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2.getIntensity()), 75014.8359375, 1e-6);
  BOOST_CHECK_EQUAL(hhits2.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  BOOST_CHECK_EQUAL(static_cast<int>(hhits2.getCharge()), 0);

  BOOST_CHECK_EQUAL(hhits2.getSubordinates().size(), 17);
  const auto& hhits2_sub1 = hhits2.getSubordinates().at(0);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getMetaValue("peak_apex_int")), 1782.034423828125, 1e-6);
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("scan_polarity"), "positive");
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getRT()), 0, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getMZ()), 109.99950621264246, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getIntensity()), 1782.034423828125, 1e-6);
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("PeptideRef").toString(), "HMDB:HMDB0062550");
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("native_id").toString(), "C9H11NO6S;M+3Na;3+");
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("identifier").toString(), "[HMDB:HMDB0062550]");
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("description").toString(), "[2-Methoxyacetaminophen sulfate]");
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("modifications").toString(), "M+3Na;3+");
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("adducts").toString(), "+3Na");
  BOOST_CHECK_EQUAL(hhits2_sub1.getMetaValue("chemical_formula").toString(), "C9H11NO6S");
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getMetaValue("dc_charge_adduct_mass")), 68.967857984674083, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getMetaValue("mz_error_ppm")), 0.449344462258211, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(hhits2_sub1.getMetaValue("mz_error_Da")), 4.942764675774924e-05, 1e-6);
  BOOST_CHECK_EQUAL(static_cast<int>(hhits2_sub1.getCharge()), 3);
}

/**
  FilterFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorFilterFeatures)
{
  FilterFeatures* ptrFilterFeatures = nullptr;
  FilterFeatures* nullPointerFilterFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrFilterFeatures, nullPointerFilterFeatures);
}

BOOST_AUTO_TEST_CASE(destructorFilterFeatures)
{
  FilterFeatures* ptrFilterFeatures = nullptr;
  ptrFilterFeatures = new FilterFeatures();
  delete ptrFilterFeatures;
}

BOOST_AUTO_TEST_CASE(gettersFilterFeatures)
{
  FilterFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 4);
  BOOST_CHECK_EQUAL(processor.getName(), "FILTER_FEATURES");
}

BOOST_AUTO_TEST_CASE(filterFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);

  // Test feature filter
  FilterFeatures filterFeatures;
  filterFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 329); // Test feature_map_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 46.652168337345103, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(!hsubordinate2.getMetaValue("used_").toBool());
}

/**
  SelectFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorSelectFeatures)
{
  SelectFeatures* ptrSelectFeatures = nullptr;
  SelectFeatures* nullPointerSelectFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrSelectFeatures, nullPointerSelectFeatures);
}

BOOST_AUTO_TEST_CASE(destructorSelectFeatures)
{
  SelectFeatures* ptrSelectFeatures = nullptr;
  ptrSelectFeatures = new SelectFeatures();
  delete ptrSelectFeatures;
}

BOOST_AUTO_TEST_CASE(gettersSelectFeatures)
{
  SelectFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 5);
  BOOST_CHECK_EQUAL(processor.getName(), "SELECT_FEATURES");
}

BOOST_AUTO_TEST_CASE(selectFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_2_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  // Test select features
  SelectFeatures selectFeatures;
  selectFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 114); // Test feature_map_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 4132.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "f6p.f6p_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 439.165833, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 329); // Test feature_map_history_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.66569377291205, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 49333.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 46.652168337345103, 1e-6);
  BOOST_CHECK(hsubordinate2.getMetaValue("used_").toBool());
}

/**
  ValidateFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorValidateFeatures)
{
  ValidateFeatures* ptrValidateFeatures = nullptr;
  ValidateFeatures* nullPointerValidateFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrValidateFeatures, nullPointerValidateFeatures);
}

BOOST_AUTO_TEST_CASE(destructorValidateFeatures)
{
  ValidateFeatures* ptrValidateFeatures = nullptr;
  ptrValidateFeatures = new ValidateFeatures();
  delete ptrValidateFeatures;
}

BOOST_AUTO_TEST_CASE(gettersValidateFeatures)
{
  ValidateFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 6);
  BOOST_CHECK_EQUAL(processor.getName(), "VALIDATE_FEATURES");
}

BOOST_AUTO_TEST_CASE(validateFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.referenceData_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("MRMFeatureValidator_referenceData_1.csv");
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
  BOOST_CHECK_CLOSE(validation_metrics.at("accuracy"), 0.0, 1e-3);
  BOOST_CHECK_CLOSE(validation_metrics.at("precision"), 0.0, 1e-3);
  BOOST_CHECK_EQUAL(std::isnan(validation_metrics.at("recall")), true);
}
/**
  PlotFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorPlotFeatures)
{
  PlotFeatures* ptrPlotFeatures = nullptr;
  PlotFeatures* nullPointerPlotFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrPlotFeatures, nullPointerPlotFeatures);
}

BOOST_AUTO_TEST_CASE(destructorPlotFeatures)
{
  PlotFeatures* ptrPlotFeatures = nullptr;
  ptrPlotFeatures = new PlotFeatures();
  delete ptrPlotFeatures;
}

BOOST_AUTO_TEST_CASE(gettersPlotFeatures)
{
  PlotFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 10);
  BOOST_CHECK_EQUAL(processor.getName(), "PLOT_FEATURES");
}

BOOST_AUTO_TEST_CASE(plotFeatures)
{
// TODO: Uncomment once FeaturePlotter is ready
}

/**
  QuantifyFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorQuantifyFeatures)
{
  QuantifyFeatures* ptrQuantifyFeatures = nullptr;
  QuantifyFeatures* nullPointerQuantifyFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrQuantifyFeatures, nullPointerQuantifyFeatures);
}

BOOST_AUTO_TEST_CASE(destructorQuantifyFeatures)
{
  QuantifyFeatures* ptrQuantifyFeatures = nullptr;
  ptrQuantifyFeatures = new QuantifyFeatures();
  delete ptrQuantifyFeatures;
}

BOOST_AUTO_TEST_CASE(gettersQuantifyFeatures)
{
  QuantifyFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 7);
  BOOST_CHECK_EQUAL(processor.getName(), "QUANTIFY_FEATURES");
}

BOOST_AUTO_TEST_CASE(quantifyComponents)
{
  // Pre-requisites: load the parameters and associated raw data
  Filenames filenames;
  filenames.quantitationMethods_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler sequenceSegmentHandler_IO;
  
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(sequenceSegmentHandler_IO, SequenceHandler(), {}, filenames);

  RawDataHandler rawDataHandler;
  rawDataHandler.setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, {}, filenames);

  // Test quantify features
  QuantifyFeatures quantifyFeatures;
  quantifyFeatures.process(rawDataHandler, {}, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 98);  // Test feature_map
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(sub.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(sub.getMetaValue("concentration_units").toString(), "uM");

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 98); // Test feature_history
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(hsub.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(hsub.getMetaValue("concentration_units").toString(), "uM");
  BOOST_CHECK(hsub.getMetaValue("used_").toBool());
}

/**
  CheckFeatures Tests
*/
BOOST_AUTO_TEST_CASE(constructorCheckFeatures)
{
  CheckFeatures* ptrCheckFeatures = nullptr;
  CheckFeatures* nullPointerCheckFeatures = nullptr;
  BOOST_CHECK_EQUAL(ptrCheckFeatures, nullPointerCheckFeatures);
}

BOOST_AUTO_TEST_CASE(destructorCheckFeatures)
{
  CheckFeatures* ptrCheckFeatures = nullptr;
  ptrCheckFeatures = new CheckFeatures();
  delete ptrCheckFeatures;
}

BOOST_AUTO_TEST_CASE(gettersCheckFeatures)
{
  CheckFeatures processor;

  BOOST_CHECK_EQUAL(processor.getID(), 8);
  BOOST_CHECK_EQUAL(processor.getName(), "CHECK_FEATURES");
}

BOOST_AUTO_TEST_CASE(checkFeatures)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, params_1, filenames);

  // Test check features
  CheckFeatures checkFeatures;
  checkFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("QC_transition_group_pass").toString(), "1");
  BOOST_CHECK_EQUAL(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sub.getMetaValue("QC_transition_pass").toString(), "1");

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("QC_transition_group_pass").toString(), "1");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("QC_transition_pass").toString(), "1");
  BOOST_CHECK(hsub.getMetaValue("used_").toBool());
}

/**
  FilterFeaturesRSDs Tests
*/
BOOST_AUTO_TEST_CASE(constructorFilterFeaturesRSDs)
{
  FilterFeaturesRSDs* ptrFilterFeaturesRSDs = nullptr;
  FilterFeaturesRSDs* nullPointerFilterFeaturesRSDs = nullptr;
  BOOST_CHECK_EQUAL(ptrFilterFeaturesRSDs, nullPointerFilterFeaturesRSDs);
}

BOOST_AUTO_TEST_CASE(destructorFilterFeaturesRSDs)
{
  FilterFeaturesRSDs* ptrFilterFeaturesRSDs = nullptr;
  ptrFilterFeaturesRSDs = new FilterFeaturesRSDs();
  delete ptrFilterFeaturesRSDs;
}

BOOST_AUTO_TEST_CASE(gettersFilterFeaturesRSDs)
{
  FilterFeaturesRSDs processor;

  BOOST_CHECK_EQUAL(processor.getID(), 4);
  BOOST_CHECK_EQUAL(processor.getName(), "FILTER_FEATURES_RSDS");
}

BOOST_AUTO_TEST_CASE(filterFeaturesRSDs)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv");
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 478); // Test feature_map
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 262509, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 640, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "acon-C.acon-C_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 842.11033066606501, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK(!hsubordinate1.getMetaValue("used_").toBool());

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(!hsubordinate2.getMetaValue("used_").toBool());
}

/**
  FilterFeaturesBackgroundInterferences Tests
*/
BOOST_AUTO_TEST_CASE(constructorFilterFeaturesBackgroundInterferences)
{
  FilterFeaturesBackgroundInterferences* ptrFilterFeaturesBackgroundInterferences = nullptr;
  FilterFeaturesBackgroundInterferences* nullPointerFilterFeaturesBackgroundInterferences = nullptr;
  BOOST_CHECK_EQUAL(ptrFilterFeaturesBackgroundInterferences, nullPointerFilterFeaturesBackgroundInterferences);
}

BOOST_AUTO_TEST_CASE(destructorFilterFeaturesBackgroundInterferences)
{
  FilterFeaturesBackgroundInterferences* ptrFilterFeaturesBackgroundInterferences = nullptr;
  ptrFilterFeaturesBackgroundInterferences = new FilterFeaturesBackgroundInterferences();
  delete ptrFilterFeaturesBackgroundInterferences;
}

BOOST_AUTO_TEST_CASE(gettersFilterFeaturesBackgroundInterferences)
{
  FilterFeaturesBackgroundInterferences processor;

  BOOST_CHECK_EQUAL(processor.getID(), 4);
  BOOST_CHECK_EQUAL(processor.getName(), "FILTER_FEATURES_BACKGROUND_INTERFERENCES");
}

BOOST_AUTO_TEST_CASE(filterFeaturesBackgroundInterferences)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_1_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv");
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);
  rawDataHandler.setFeatureBackgroundFilter(rawDataHandler.getFeatureFilter()); // copy over the feature filter
  rawDataHandler.setFeatureBackgroundEstimations(rawDataHandler.getFeatureFilter());

  // Test feature filter
  FilterFeaturesBackgroundInterferences filterFeatures;
  filterFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 479); // Test feature_map_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getRT()), 953.66569377291205, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[50].getSubordinates().size(), 1);

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 3842, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 1067.54472965431, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481); // Test feature_map_history_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[50].getSubordinates().size(), 2);

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(!hsubordinate2.getMetaValue("used_").toBool());
}

/**
  CheckFeaturesBackgroundInterferences Tests
*/
BOOST_AUTO_TEST_CASE(constructorCheckFeaturesBackgroundInterferences)
{
  CheckFeaturesBackgroundInterferences* ptrCheckFeaturesBackgroundInterferences = nullptr;
  CheckFeaturesBackgroundInterferences* nullPointerCheckFeaturesBackgroundInterferences = nullptr;
  BOOST_CHECK_EQUAL(ptrCheckFeaturesBackgroundInterferences, nullPointerCheckFeaturesBackgroundInterferences);
}

BOOST_AUTO_TEST_CASE(destructorCheckFeaturesBackgroundInterferences)
{
  CheckFeaturesBackgroundInterferences* ptrCheckFeaturesBackgroundInterferences = nullptr;
  ptrCheckFeaturesBackgroundInterferences = new CheckFeaturesBackgroundInterferences();
  delete ptrCheckFeaturesBackgroundInterferences;
}

BOOST_AUTO_TEST_CASE(gettersCheckFeaturesBackgroundInterferences)
{
  CheckFeaturesBackgroundInterferences processor;

  BOOST_CHECK_EQUAL(processor.getID(), 8);
  BOOST_CHECK_EQUAL(processor.getName(), "CHECK_FEATURES_BACKGROUND_INTERFERENCES");
}

BOOST_AUTO_TEST_CASE(checkFeaturesBackgroundInterferences)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv");
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("QC_transition_group_%BackgroundInterference_pass").toString(), "1");
  BOOST_CHECK_EQUAL(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sub.getMetaValue("QC_transition_%BackgroundInterference_pass").toString(), "0");

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("QC_transition_group_%BackgroundInterference_pass").toString(), "1");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("QC_transition_%BackgroundInterference_pass").toString(), "0");
  BOOST_CHECK(hsub.getMetaValue("used_").toBool());
}

/**
  CheckFeaturesRSDs Tests
*/
BOOST_AUTO_TEST_CASE(constructorCheckFeaturesRSDs)
{
  CheckFeaturesRSDs* ptrCheckFeaturesRSDs = nullptr;
  CheckFeaturesRSDs* nullPointerCheckFeaturesRSDs = nullptr;
  BOOST_CHECK_EQUAL(ptrCheckFeaturesRSDs, nullPointerCheckFeaturesRSDs);
}

BOOST_AUTO_TEST_CASE(destructorCheckFeaturesRSDs)
{
  CheckFeaturesRSDs* ptrCheckFeaturesRSDs = nullptr;
  ptrCheckFeaturesRSDs = new CheckFeaturesRSDs();
  delete ptrCheckFeaturesRSDs;
}

BOOST_AUTO_TEST_CASE(gettersCheckFeaturesRSDs)
{
  CheckFeaturesRSDs processor;

  BOOST_CHECK_EQUAL(processor.getID(), 8);
  BOOST_CHECK_EQUAL(processor.getName(), "CHECK_FEATURES_RSDS");
}

BOOST_AUTO_TEST_CASE(checkFeaturesRSDs)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_test_3_core_RawDataProcessor.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponents_1.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeaturersdqccomponentgroups_1.csv");
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& sub = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("QC_transition_group_%RSD_pass").toString(), "1");
  BOOST_CHECK_EQUAL(sub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sub.getMetaValue("QC_transition_%RSD_pass").toString(), "0");

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 98);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsub = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("QC_transition_group_%RSD_pass").toString(), "1");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(hsub.getMetaValue("QC_transition_%RSD_pass").toString(), "0");
  BOOST_CHECK(hsub.getMetaValue("used_").toBool());
}

BOOST_AUTO_TEST_CASE(process)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureFiltersRDP loadFeatureFilters;
  loadFeatureFilters.process(rawDataHandler, params_1, filenames);

  filenames.quantitationMethods_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler sequenceSegmentHandler_IO;
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(sequenceSegmentHandler_IO, SequenceHandler(), {}, filenames);
  rawDataHandler.setQuantitationMethods(sequenceSegmentHandler_IO.getQuantitationMethods());

  LoadFeatureQCsRDP loadFeatureQCs;
  loadFeatureQCs.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
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

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 114); // test feature_map_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& subordinate0 = rawDataHandler.getFeatureMap()[0].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate0.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate0.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate0.getRT()), 953.665693772912, 1e-6);

  const OpenMS::Feature& subordinate1 = rawDataHandler.getFeatureMap()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("QC_transition_pass").toString(), "1");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate1.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(subordinate1.getMetaValue("concentration_units").toString(), "uM");

  const OpenMS::Feature& subordinate2 = rawDataHandler.getFeatureMap()[8].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getMetaValue("peak_apex_int")), 2780997.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate2.getMetaValue("native_id").toString(), "Pool_2pg_3pg.Pool_2pg_3pg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate2.getRT()), 832.69577069044112, 1e-6);

  const OpenMS::Feature& subordinate3 = rawDataHandler.getFeatureMap()[49].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate3.getMetaValue("peak_apex_int")), 217684.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate3.getMetaValue("native_id").toString(), "f1p.f1p_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate3.getRT()), 602.45962461757676, 1e-6);

  const OpenMS::Feature& subordinate4 = rawDataHandler.getFeatureMap()[113].getSubordinates()[0]; // this is [49][0] in python tests
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate4.getMetaValue("peak_apex_int")), 4066240.0, 1e-6);
  BOOST_CHECK_EQUAL(subordinate4.getMetaValue("native_id").toString(), "xan.xan_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(subordinate4.getRT()), 89.418783654689776, 1e-6);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 481); // test feature_map_history_
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 3);

  const OpenMS::Feature& hsubordinate1 = rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1];
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("native_id").toString(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getMetaValue("calculated_concentration")), 0.44335812456518986, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("concentration_units").toString(), "uM");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate1.getRT()), 953.665693772912, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate1.getMetaValue("QC_transition_pass").toString(), "1");
  BOOST_CHECK(hsubordinate1.getMetaValue("used_").toBool());

  const OpenMS::Feature& hsubordinate2 = rawDataHandler.getFeatureMapHistory()[50].getSubordinates()[0];
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getMetaValue("peak_apex_int")), 0.0, 1e-6);
  BOOST_CHECK_EQUAL(hsubordinate2.getMetaValue("native_id").toString(), "accoa.accoa_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(hsubordinate2.getRT()), 1067.5447296543123, 1e-6);
  BOOST_CHECK(!hsubordinate2.getMetaValue("used_").toBool());
}

BOOST_AUTO_TEST_CASE(emg_processor)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  LoadTransitions loadTransitions;
  loadTransitions.process(rawDataHandler, params_1, filenames);

  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.mzML");
  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler,params_1, filenames);
  loadRawData.extractMetaData(rawDataHandler);

  MapChromatograms mapChroms;
  mapChroms.process(rawDataHandler, params_1, filenames);

  // Test pick features
  PickMRMFeatures pickFeatures;
  pickFeatures.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 481);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature sub1 { rawDataHandler.getFeatureMap()[0].getSubordinates()[0] };
  BOOST_CHECK_CLOSE(static_cast<double>(sub1.getIntensity()), 922154.75, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub1.getMetaValue("peak_apex_position")), 953.40699999999993, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub1.getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(sub1.metaValueExists("area_background_level") == false, 1);
  BOOST_CHECK_EQUAL(sub1.metaValueExists("noise_background_level") == false, 1);
  // BOOST_CHECK_CLOSE(static_cast<double>(sub1.getMetaValue("area_background_level")), 0.0, 1e-6);
  // BOOST_CHECK_CLOSE(static_cast<double>(sub1.getMetaValue("noise_background_level")), 0.0, 1e-6);

  // reduce the number of features for test purposes
  OpenMS::FeatureMap& m = rawDataHandler.getFeatureMap();
  m.erase(m.begin() + 1, m.end());

  BOOST_CHECK_EQUAL(m.size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 1);

  FitFeaturesEMG emg;
  emg.process(rawDataHandler, params_1, filenames);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 3);

  const OpenMS::Feature sub2 { rawDataHandler.getFeatureMap()[0].getSubordinates()[0] };
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getIntensity()), 758053.375, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("peak_apex_position")), 953.40699999999993, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("peak_apex_int")), 202893.72550713021, 1e-6);
  BOOST_CHECK_EQUAL(sub2.metaValueExists("area_background_level") == false, 0);
  BOOST_CHECK_EQUAL(sub2.metaValueExists("noise_background_level") == false, 0);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("area_background_level")), 0.0083339133585532514, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("noise_background_level")), 0.00012474754841647006, 1e-6);

  // test feature storing
  RawDataHandler rawDataHandler2;
  filenames.featureXML_o = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.featureXML");
  StoreFeatures storeFeatures;
  storeFeatures.process(rawDataHandler, params_1, filenames);

  filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.featureXML");
  LoadFeatures loadFeatures;
  loadFeatures.process(rawDataHandler2, params_1, filenames);

  const OpenMS::FeatureMap& m2 = rawDataHandler2.getFeatureMap();
  const OpenMS::Feature sub3 { m2[0].getSubordinates()[0] };

  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getIntensity()), static_cast<double>(sub3.getIntensity()), 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("peak_apex_position")), static_cast<double>(sub3.getMetaValue("peak_apex_position")), 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("peak_apex_int")), static_cast<double>(sub3.getMetaValue("peak_apex_int")), 1e-6);
  BOOST_CHECK_EQUAL(sub3.metaValueExists("area_background_level") == false, 0);
  BOOST_CHECK_EQUAL(sub3.metaValueExists("noise_background_level") == false, 0);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("area_background_level")), static_cast<double>(sub3.getMetaValue("area_background_level")), 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(sub2.getMetaValue("noise_background_level")), static_cast<double>(sub3.getMetaValue("noise_background_level")), 1e-6);

  std::remove(SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_mzML_1.featureXML"));
}

BOOST_AUTO_TEST_CASE(calculateMDVs)
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
        BOOST_CHECK_CLOSE((float)lactate_normalized_normsum.at(i).getSubordinates().at(j).getMetaValue("peak_apex_int"),
                          L1_norm_sum.at(j), 1e-6);
      }
      else if (i == 1) // lactate_2
      {
        BOOST_CHECK_CLOSE((float)lactate_normalized_normsum.at(i).getSubordinates().at(j).getMetaValue("peak_apex_int"),
                          L2_norm_sum.at(j), 1e-6);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(isotopicCorrections)
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
      BOOST_CHECK_CLOSE((double)lactate_1_corrected_featureMap.at(i).getSubordinates().at(j).getMetaValue("peak_apex_int"),
                        L1_corrected[j], 1e-3);
    }
  }
}

BOOST_AUTO_TEST_CASE(calculateIsotopicPurities)
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
    BOOST_CHECK_CLOSE((double)(lactate_1_with_isotopic_purity_featureMap.at(i).getMetaValue("1_2-13C_glucose_experiment")) * 100,
                      99.6469, 1e-4);
  }
}

BOOST_AUTO_TEST_CASE(calculateMDVAccuracies)
{
  // Pre-requisites: load the parameters and associated raw data
  ParameterSet params_1;
  ParameterSet params_2;
  load_data(params_1, params_2);
  RawDataHandler rawDataHandler;

  // for peptide SumFormula
  Filenames filenames;
  filenames.traML_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
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
    BOOST_CHECK_CLOSE( (float)featureMap_1.at(i).getMetaValue("average_accuracy"), Average_accuracy_groundtruth[0], 1e-1 );

    for (size_t feature_subordinate = 0; feature_subordinate < featureMap_1.at(i).getSubordinates().size(); ++feature_subordinate)
    {
      BOOST_CHECK_CLOSE( (float)featureMap_1.at(i).getSubordinates().at(feature_subordinate).getMetaValue("absolute_difference"),
                          accoa_C23H37N7O17P3S_abs_diff[feature_subordinate], 1e-2 );
    }
  }
}
///

BOOST_AUTO_TEST_SUITE_END()
