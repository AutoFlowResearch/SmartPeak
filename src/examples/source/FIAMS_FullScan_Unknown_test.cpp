// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/FIAMS_FullScan_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

void test_main_FIAMS_FullScan_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  example_FIAMS_FullScan_Unknowns(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  LoadParameters loadParameters;
  Filenames filenames;

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_1_BatchName_1900-01-01_000000.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_test.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();
  
  rawDataHandler.clear();
  
  filenames.parameters_csv_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/parameters.csv");
  loadParameters.process(rawDataHandler,{}, filenames);
  
  std::map<std::string, std::vector<std::map<std::string, std::string>>>* params;
  params = &rawDataHandler.getParameters();
  assert(params->count("SequenceProcessor")==1 && "None or more 'SequenceProcessor' found");

cout << "fm1 size: " << fm1.size() << endl;
cout << "fm2 size: " << fm2.size() << endl;
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature* f1 = &fm1[0];
  const OpenMS::Feature* f2 = &fm2[0];
cout << "0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "0 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "23dpg.23dpg_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 249.465663036488));
  assert(Utilities::assert_close((double)f1->getIntensity(), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getRT(), 945.205));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getIntensity(), (double)f2->getIntensity()));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6];
  f2 = &fm2[6];
cout << "6 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "6 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "6 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "2mcit.2mcit_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 96.775498183555868));
  assert(Utilities::assert_close((double)f1->getIntensity(), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getRT(), 852.951));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getIntensity(), (double)f2->getIntensity()));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[50];
  f2 = &fm2[50];
cout << "50 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "50 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "50 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "actp.actp_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getIntensity(), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getRT(), 745.612));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getIntensity(), (double)f2->getIntensity()));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main()
{
  test_main_FIAMS_FullScan_Unknown();
  return 0;
}
