// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

static const std::string example_data_dir = "/home/pasdom/SmartPeak2/src/examples/data/"; // TODO: redefine it

void test_main_LCMS_MRM_QCs()
{
  example_LCMS_MRM_Unknowns(example_data_dir + "LCMS_MRM_QCs", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_data_dir + "LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_data_dir + "LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 0);
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature& f1 = fm1[50].getSubordinates()[0];
  const OpenMS::Feature& f2 = fm2[50].getSubordinates()[0];

  assert(f1.getMetaValue("native_id") == f2.getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1.getMetaValue("peak_apex_int"), (double)f2.getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1.getRT(), (double)f2.getRT()));
}

int main(int argc, char **argv)
{
  test_main_LCMS_MRM_QCs();
  return 0;
}
