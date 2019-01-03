// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

void test_main_LCMS_MRM_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_Unknowns");
  const SequenceHandler::Filenames static_filenames = SequenceHandler::Filenames::getDefaultStaticFilenames(main_dir);

  example_LCMS_MRM_Unknowns(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_Unknowns/features/170808_Jonathan_yeast_Sacc1_1x.featureXML")
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_Unknowns/features/170808_Jonathan_yeast_Sacc1_1x_test.featureXML")
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

cout << "fm1 size: " << fm1.size() << endl;
cout << "fm2 size: " << fm2.size() << endl;
  assert(fm1.size() == 92);
  assert(fm1.size() == fm2.size());

cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;
  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());

  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];
cout << "0 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "0 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "0 0 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "23dpg.23dpg_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 249.465663036488));
  assert(Utilities::assert_close((double)f1->getRT(), 945.205));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
cout << "6 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "6 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "6 0 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "actp.actp_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 228.975008430023));
  assert(Utilities::assert_close((double)f1->getRT(), 745.612));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[50].getSubordinates()[0];
  f2 = &fm2[50].getSubordinates()[0];
cout << "50 0 native_id: " << f1->getMetaValue("native_id") << endl;
cout << "50 0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
cout << "50 0 getRT: " << f1->getRT() << endl;
  assert(f1->getMetaValue("native_id") == "gsn.gsn_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 65212.2196216507));
  assert(Utilities::assert_close((double)f1->getRT(), 84.7791));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main(int argc, char **argv)
{
  test_main_LCMS_MRM_Unknown();
  return 0;
}
