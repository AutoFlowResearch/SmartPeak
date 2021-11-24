// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/HPLC_UV_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>

using namespace SmartPeak;
using namespace std;

void test_main_HPLC_UV_Unknown()
{
  ApplicationHandler application_handler;
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns");
  Filenames filenames_main = Utilities::buildFilenamesFromDirectory(application_handler, main_dir);
  filenames_main.setTagValue(Filenames::Tag::MAIN_DIR, main_dir);
  filenames_main.setTagValue(Filenames::Tag::MZML_INPUT_PATH, main_dir + "/mzML/");
  filenames_main.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, main_dir + "/features/");
  filenames_main.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, main_dir + "/features/");

  example_HPLC_UV_Unknowns(main_dir, filenames_main, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  Filenames filenames;

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_1_BatchName_1900-01-01_000000.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_test.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  cout << "fm1.size(): " << fm1.size() << endl;
  cout << "fm2.size(): " << fm2.size() << endl;

  assert(fm1.size() == 11);
  assert(fm1.size() == fm2.size());

  cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
  cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;

  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());
  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];

  cout << "native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "getRT: "         << f1->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.078419296154379e04));
  assert(Utilities::assert_close((double)f1->getRT(), 2.02693));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  cout << "fm1[6].getSubordinates().size(): " << fm1[6].getSubordinates().size() << endl;
  cout << "fm2[6].getSubordinates().size(): " << fm2[6].getSubordinates().size() << endl;

  assert(fm1[6].getSubordinates().size() == 2);
  assert(fm1[6].getSubordinates().size() == fm2[6].getSubordinates().size());
  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];

  cout << "native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "getRT: "         << f1->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "Melatonin");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 9.535993535243859e04));
  assert(Utilities::assert_close((double)f1->getRT(), 6.96495));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main()
{
  test_main_HPLC_UV_Unknown();
  return 0;
}
