// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Standards_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void test_main_HPLC_UV_Standards()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  example_LCMS_MRM_Standards(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards/features/100ug_8_BatchName_1900-01-01_000000.featureXML")
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards/features/100ug_test.featureXML")
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

cout << "fm1.size(): " << fm1.size() << endl;
  assert(fm1.size() == 88);
  assert(fm1.size() == fm2.size());

cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;
  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());
  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];
cout << "f1 0 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "f1 0 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "f1 0 0 getRT: " << f1->getRT() << endl;
cout << "f2 0 0 native_id: " << f2->getMetaValue("native_id") << endl;
cout << "f2 0 0 peak_apex_int: " << f2->getMetaValue("peak_apex_int") << endl;
cout << "f2 0 0 getRT: " << f2->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 32.407596803989946));
  assert(Utilities::assert_close((double)f1->getRT(), 1.12343));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

cout << "fm1[6].getSubordinates().size(): " << fm1[6].getSubordinates().size() << endl;
  assert(fm1[6].getSubordinates().size() == 2);
  assert(fm1[6].getSubordinates().size() == fm2[6].getSubordinates().size());
  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "f1 6 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "f1 6 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "f1 6 0 getRT: " << f1->getRT() << endl;
cout << "f2 6 0 native_id: " << f2->getMetaValue("native_id") << endl;
cout << "f2 6 0 peak_apex_int: " << f2->getMetaValue("peak_apex_int") << endl;
cout << "f2 6 0 getRT: " << f2->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 28.432575392802832));
  assert(Utilities::assert_close((double)f1->getRT(), 6.96528));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main(int argc, char **argv)
{
  test_main_HPLC_UV_Standards();
  return 0;
}
