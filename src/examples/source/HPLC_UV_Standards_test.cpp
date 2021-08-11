// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/HPLC_UV_Standards_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

void test_main_HPLC_UV_Standards()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards");
  Filenames filenames_main;
  filenames_main.setTag("MAIN_DIR", main_dir);
  filenames_main.setTag("MZML_INPUT_PATH", main_dir + "/mzML/");
  filenames_main.setTag("FEATURES_INPUT_PATH", main_dir + "/features/");
  filenames_main.setTag("FEATURES_OUTPUT_PATH", main_dir + "/features/");

  example_HPLC_UV_Standards(main_dir, filenames_main, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  Filenames filenames;

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards/features/100ug_8_BatchName_1900-01-01_000000.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards/features/100ug_test.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  cout << "fm1.size(): " << fm1.size() << endl;
  cout << "fm2.size(): " << fm2.size() << endl;

  assert(fm1.size() == 8);
  assert(fm1.size() == fm2.size());

  cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
  cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;

  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());
  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];

  cout << "f1 0 0 native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "f1 0 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "f1 0 0 getRT: "         << f1->getRT() << endl;
  cout << "f2 0 0 native_id: "     << f2->getMetaValue("native_id") << endl;
  cout << "f2 0 0 peak_apex_int: " << f2->getMetaValue("peak_apex_int") << endl;
  cout << "f2 0 0 getRT: "         << f2->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.952283536328848e04));
  assert(Utilities::assert_close((double)f1->getRT(), 2.03216));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));


  cout << "fm1[6].getSubordinates().size(): " << fm1[6].getSubordinates().size() << endl;
  cout << "fm2[6].getSubordinates().size(): " << fm2[6].getSubordinates().size() << endl;

  assert(fm1[6].getSubordinates().size() == 2);
  assert(fm1[6].getSubordinates().size() == fm2[6].getSubordinates().size());
  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];

  cout << "f1 6 0 native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "f1 6 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "f1 6 0 getRT: "         << f1->getRT() << endl;
  cout << "f2 6 0 native_id: "     << f2->getMetaValue("native_id") << endl;
  cout << "f2 6 0 peak_apex_int: " << f2->getMetaValue("peak_apex_int") << endl;
  cout << "f2 6 0 getRT: "         << f2->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "Tryptamine");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 3.796930796956271e04));
  assert(Utilities::assert_close((double)f1->getRT(), 3.43251));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  // check quantitation methods
  const string quant_methods_pathname {main_dir + "/features/sequence1_quantitationMethods.csv"};
  const bool is_ok = Utilities::testStoredQuantitationMethods(quant_methods_pathname);
  assert(is_ok == true);
}

int main()
{
  test_main_HPLC_UV_Standards();
  return 0;
}
