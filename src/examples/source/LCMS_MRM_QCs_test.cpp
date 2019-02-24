// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void test_main_LCMS_MRM_QCs()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  example_LCMS_MRM_Unknowns(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1_1_BatchName_1900-01-01_000000.featureXML")
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1_test.featureXML")
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();
// TODO: these tests currently fail
cout << "fm1 size: " << fm1.size() << endl; // 95
cout << "fm2 size: " << fm2.size() << endl; // 97
  // assert(fm1.size() == 0); // TODO: update to a valid value
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
  assert(f1->getMetaValue("native_id") == "23dpg.23dpg_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 193842.035732929));
  assert(Utilities::assert_close((double)f1->getRT(), 953.754));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "6 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "6 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "6 0 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "accoa.accoa_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 1323907.96670512));
  assert(Utilities::assert_close((double)f1->getRT(), 1011.55));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[15].getSubordinates()[0];
  f2 = &fm2[15].getSubordinates()[0];
cout << "15 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "15 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "15 0 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "asn-L.asn-L_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 55413.6543420461));
  assert(Utilities::assert_close((double)f1->getRT(), 57.4743));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main(int argc, char **argv)
{
  test_main_LCMS_MRM_QCs();
  return 0;
}
