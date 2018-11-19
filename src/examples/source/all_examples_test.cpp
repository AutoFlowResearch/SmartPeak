// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

const std::string example_dir = "/home/pasdom/SmartPeak2/examples"; // TODO: redefine it


// test_main_LCMS_MRM_Unknown
{
  example_LCMS_MRM_Unknowns(example_dir + "LCMS_MRM_Unknowns", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "LCMS_MRM_Unknowns/features/170808_Jonathan_yeast_Sacc1_1x.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "LCMS_MRM_Unknowns/features/170808_Jonathan_yeast_Sacc1_1x_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 0); // TODO: 0 is supposed to fail
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature& f1 = fm1[50].getSubordinates()[0];
  const OpenMS::Feature& f2 = fm2[50].getSubordinates()[0];

  assert(f1.getMetaValue("native_id") == f2.getMetaValue("native_id"));
  assert(assert_close(f1.getMetaValue("peak_apex_int"), f2.getMetaValue("peak_apex_int"), 1e-6));
  assert(assert_close(f1.getRT(), f2.getRT(), 1e-6));
}

// test_main_GCMS_SIM_Unknown
{
  example_LCMS_MRM_Unknowns(example_dir + "GCMS_SIM_Unknowns", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "GCMS_SIM_Unknowns/features/GCMS_SIM.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "GCMS_SIM_Unknowns/features/GCMS_SIM_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 0);
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature& f1 = fm1[15].getSubordinates()[0];
  const OpenMS::Feature& f2 = fm2[15].getSubordinates()[0];

  assert(f1.getMetaValue("native_id") == f2.getMetaValue("native_id"));
  assert(assert_close(f1.getMetaValue("peak_apex_int"), f2.getMetaValue("peak_apex_int"), 1e-6));
  assert(assert_close(f1.getRT(), f2.getRT(), 1e-6));
}

// test_main_GCMS_FullScan_Unknown
{
  example_LCMS_MRM_Unknowns(example_dir + "GCMS_FullScan_Unknowns", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "GCMS_FullScan_Unknowns/features/GCMS_FullScan.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "GCMS_FullScan_Unknowns/features/GCMS_FullScan_test.featureXML"
  );

  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == 0);
  assert(fm1.size() == fm2.size());

  const OpenMS::Feature& f1 = fm1[15].getSubordinates()[0];
  const OpenMS::Feature& f2 = fm2[15].getSubordinates()[0];

  assert(f1.getMetaValue("native_id") == f2.getMetaValue("native_id"));
  assert(assert_close(f1.getMetaValue("peak_apex_int"), f2.getMetaValue("peak_apex_int"), 1e-6));
  assert(assert_close(f1.getRT(), f2.getRT(), 1e-6));
}

// test_main_HPLC_UV_Unknown
{
  example_LCMS_MRM_Unknowns(example_dir + "HPLC_UV_Unknowns", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_test.featureXML"
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

// test_main_LCMS_MRM_QCs
{
  example_LCMS_MRM_Unknowns(example_dir + "LCMS_MRM_QCs", ",");

  RawDataHandler rawDataHandler;

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1.featureXML"
  );

  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  OpenMSFile::loadFeatureMap(
    rawDataHandler,
    example_dir + "LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1_test.featureXML"
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
