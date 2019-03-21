// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

void test_main_GCMS_FullScan_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("GCMS_FullScan_Unknowns");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  example_LCMS_MRM_Unknowns(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  Filenames filenames;

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("GCMS_FullScan_Unknowns/features/GCMS_FullScan_1_BatchName_1900-01-01_000000.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("GCMS_FullScan_Unknowns/features/GCMS_FullScan_test.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

cout << "fm1 size: " << fm1.size() << endl;
cout << "fm2 size: " << fm2.size() << endl;
  // assert(fm1.size() == 18);
  assert(fm1.size() == fm2.size());

cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;
  // assert(fm1[0].getSubordinates().size() == 5);
  // assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());

  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];
cout << "0 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "0 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "0 0 getRT: " << f1->getRT() << endl;
  // assert(f1->getMetaValue("native_id") == "3PG_459");
  // assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 4027970.41809005));
  // assert(Utilities::assert_close((double)f1->getRT(), 818.978));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "6 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "6 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "6 0 getRT: " << f1->getRT() << endl;
  // assert(f1->getMetaValue("native_id") == "Fumarate_245");
  // assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 28430716.6524279));
  // assert(Utilities::assert_close((double)f1->getRT(), 574.329));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[15].getSubordinates()[0];
  f2 = &fm2[15].getSubordinates()[0];
cout << "15 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "15 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "15 0 getRT: " << f1->getRT() << endl;
  // assert(f1->getMetaValue("native_id") == "S7P_358");
  // assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 1363866.62798859));
  // assert(Utilities::assert_close((double)f1->getRT(), 1067.84));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[50].getSubordinates()[0];
  f2 = &fm2[50].getSubordinates()[0];
cout << "50 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "50 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "50 0 getRT: " << f1->getRT() << endl;
  // assert(f1->getMetaValue("native_id") == "S7P_358");
  // assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 1363866.62798859));
  // assert(Utilities::assert_close((double)f1->getRT(), 1067.84));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main()
{
  test_main_GCMS_FullScan_Unknown();
  return 0;
}
