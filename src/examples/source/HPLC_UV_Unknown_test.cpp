// TODO: Add copyright

// #include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

static const std::string example_data_dir = "/home/pasdom/SmartPeak2/src/examples/data/"; // TODO: redefine it

void test_main_HPLC_UV_Unknown()
{
  example_LCMS_MRM_Unknowns(example_data_dir + "HPLC_UV_Unknowns", ",", true);

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_data_dir + "HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_data_dir + "HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

cout << "fm1 size: " << fm1.size() << endl;
cout << "fm2 size: " << fm2.size() << endl;

  assert(fm1.size() == 9);
  assert(fm1.size() == fm2.size());

  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());
  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];

cout << "fm1sub size: " << fm1[0].getSubordinates().size() << endl;
cout << "fm2sub size: " << fm2[0].getSubordinates().size() << endl;

cout << "native_id: " << f1->getMetaValue("native_id") << endl;
cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 19.8419150936099));
  assert(Utilities::assert_close((double)f1->getRT(), 2.02693));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "native_id: " << f1->getMetaValue("native_id") << endl;
cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "Serotonin");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 1.12071600181173));
  assert(Utilities::assert_close((double)f1->getRT(), 1.88778));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main(int argc, char **argv)
{
  test_main_HPLC_UV_Unknown();
  return 0;
}
