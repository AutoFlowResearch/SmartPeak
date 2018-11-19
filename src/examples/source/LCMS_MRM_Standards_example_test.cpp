// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

const std::string example_dir = "/home/pasdom/SmartPeak2/examples"; // TODO: redefine it

// test_main_HPLC_UV_Standards
{
  example_LCMS_MRM_Standards(example_dir + "HPLC_UV_Standards", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "HPLC_UV_Standards/features/100ug.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "HPLC_UV_Standards/features/100ug_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 0);
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature& f1 = fm1[0].getSubordinates()[0];
  const OpenMS::Feature& f2 = fm2[0].getSubordinates()[0];

  assert(f1.getMetaValue("native_id") == f2.getMetaValue("native_id"));
  assert(assert_close(f1.getMetaValue("peak_apex_int"), f2.getMetaValue("peak_apex_int"), 1e-6));
  assert(assert_close(f1.getRT(), f2.getRT(), 1e-6));
}

// test_main_LCMS_MRM_Standards
{
  example_LCMS_MRM_Standards(example_dir + "LCMS_MRM_Standards", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "LCMS_MRM_Standards/features/150516_CM1_Level1.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "LCMS_MRM_Standards/features/150516_CM1_Level1_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 0);
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature& f1 = fm1[50].getSubordinates()[0];
  const OpenMS::Feature& f2 = fm2[50].getSubordinates()[0];

  assert(f1.getMetaValue("native_id") == f2.getMetaValue("native_id"));
  assert(assert_close(f1.getMetaValue("peak_apex_int"), f2.getMetaValue("peak_apex_int"), 1e-6));
  assert(assert_close(f1.getRT(), f2.getRT(), 1e-6));
}
