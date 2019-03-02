// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void test_main_HPLC_UV_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  example_LCMS_MRM_Unknowns(main_dir, static_filenames, ",", true);

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_1_BatchName_1900-01-01_000000.featureXML")
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_test.featureXML")
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

cout << "fm1 size: " << fm1.size() << endl;
cout << "fm2 size: " << fm2.size() << endl;

  assert(fm1.size() == 341);
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
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 152.331401728448043));
  assert(Utilities::assert_close((double)f1->getRT(), 3.42282));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "native_id: " << f1->getMetaValue("native_id") << endl;
cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.646960722065395));
  assert(Utilities::assert_close((double)f1->getRT(), 5.28569));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  // cout << "\n\nSTART ENTIRE OUTPUT FOR DEBUGGING" << endl;
  // for (const OpenMS::Feature& f : fm1) {
  //   // cout << f.getMetaValue("native_id") << "\t" << f.getMetaValue("peak_apex_int") << "\t" << f.getRT() << "\n";
  //   for (const OpenMS::Feature& sub : f.getSubordinates()) {
  //     cout << sub.getMetaValue("native_id") << "\t\t" << sub.getMetaValue("peak_apex_int") << "\t\t" << sub.getRT() << "\n";
  //   }
  //   cout << "\n";
  // }
  // cout << "END ENTIRE OUTPUT FOR DEBUGGING" << endl;
}

int main(int argc, char **argv)
{
  test_main_HPLC_UV_Unknown();
  return 0;
}
