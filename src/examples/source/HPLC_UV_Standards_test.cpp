// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Standards_example.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void test_main_HPLC_UV_Standards()
{
  example_LCMS_MRM_Standards(SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards"), ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards/features/100ug.featureXML")
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Standards/features/100ug_test.featureXML")
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 8);
  assert(fm1.size() == fm2.size());

// cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
// cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;
  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());
  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];
// cout << "f1 0 0 native_id: " << f1->getMetaValue("native_id") << endl;
// cout << "f1 0 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
// cout << "f1 0 0 getRT: " << f1->getRT() << endl;
// cout << "f2 0 0 native_id: " << f2->getMetaValue("native_id") << endl;
// cout << "f2 0 0 peak_apex_int: " << f2->getMetaValue("peak_apex_int") << endl;
// cout << "f2 0 0 getRT: " << f2->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 29.5228353632885));
  assert(Utilities::assert_close((double)f1->getRT(), 2.03216));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  assert(fm1[6].getSubordinates().size() == 2);
  assert(fm1[6].getSubordinates().size() == fm2[6].getSubordinates().size());
  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
// cout << "f1 6 0 native_id: " << f1->getMetaValue("native_id") << endl;
// cout << "f1 6 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
// cout << "f1 6 0 getRT: " << f1->getRT() << endl;
// cout << "f2 6 0 native_id: " << f2->getMetaValue("native_id") << endl;
// cout << "f2 6 0 peak_apex_int: " << f2->getMetaValue("peak_apex_int") << endl;
// cout << "f2 6 0 getRT: " << f2->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "Tryptamine");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 37.9693079695627));
  assert(Utilities::assert_close((double)f1->getRT(), 3.43251));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main(int argc, char **argv)
{
  test_main_HPLC_UV_Standards();
  return 0;
}
