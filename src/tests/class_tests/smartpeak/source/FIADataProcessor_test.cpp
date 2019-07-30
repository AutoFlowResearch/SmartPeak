// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE FIADataProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/algorithm/FIADataProcessor.h>

#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <SmartPeak/io/FileReader.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>

using namespace SmartPeak;
using namespace std;

void load_data(
  std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_1,
  std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_2
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

BOOST_AUTO_TEST_SUITE(fiadataprocessor)

BOOST_AUTO_TEST_CASE(integrate_by_time)
{
  Filenames filenames;
  filenames.parameters_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core_tmpFix.csv");
  filenames.mzML_i = SMARTPEAK_GET_TEST_DATA_PATH("20190621_metk_SerumQC9_FS_NoColumn_LowMass_Res30000_POS.mzML");

  map<string, vector<map<string, string>>> params;

  RawDataHandler rawDataHandler;

  LoadParameters loadParameters;
  loadParameters.process(rawDataHandler, {}, filenames);
  params = rawDataHandler.getParameters();

  LoadRawData loadRawData;
  loadRawData.process(rawDataHandler, params, filenames);

  std::cout << "chromatograms " << rawDataHandler.getExperiment().getChromatograms().size() << std::endl;

  BOOST_CHECK_EQUAL(179, 179);

//   MRMFeatureValidator::validate_MRMFeatures(
//     ref_data,
//     featureMap,
//     mdh.getInjectionName(),
//     output_validated,
//     validation_metrics,
//     Tr_window
//   );

//   BOOST_CHECK_CLOSE(validation_metrics.at("accuracy"), 0.987096786, 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
