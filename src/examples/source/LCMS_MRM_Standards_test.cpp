// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/LCMS_MRM_Standards_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

void test_main_LCMS_MRM_Standards()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_Standards");
  Filenames filenames_main;
  filenames_main.setRootPaths(main_dir,
    main_dir + "/mzML/",
    main_dir + "/features/",
    main_dir + "/features/");

  example_LCMS_MRM_Standards(main_dir, filenames_main, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  Filenames filenames;

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_Standards/features/150516_CM1_Level1_1_BatchName_1900-01-01_000000.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_Standards/features/150516_CM1_Level1_test.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

cout << "fm1 size: " << fm1.size() << endl;
cout << "fm2 size: " << fm2.size() << endl;
  assert(fm1.size() == 96);
  assert(fm1.size() == fm2.size());

cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
  assert(fm1[0].getSubordinates().size() == 3);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());

  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];
cout << "native_id: " << f1->getMetaValue("native_id") << endl;
cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "23dpg.23dpg_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 1.598738552975021e04));
  assert(Utilities::assert_close((double)f1->getRT(), 951.709));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[50].getSubordinates()[0];
  f2 = &fm2[50].getSubordinates()[0];
cout << "native_id: " << f1->getMetaValue("native_id") << endl;
cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "glyc3p.glyc3p_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 1.090509051976222e05));
  assert(Utilities::assert_close((double)f1->getRT(), 443.224));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "native_id: " << f1->getMetaValue("native_id") << endl;
cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "Lcystin.Lcystin_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.17835510070436e06));
  assert(Utilities::assert_close((double)f1->getRT(), 58.6905));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  // check quantitation methods
  const string quant_methods_pathname {main_dir + "/features/segment1_quantitationMethods.csv"};
  const bool is_ok = Utilities::testStoredQuantitationMethods(quant_methods_pathname);
  assert(is_ok == true);
}

int main()
{
  test_main_LCMS_MRM_Standards();
  return 0;
}
