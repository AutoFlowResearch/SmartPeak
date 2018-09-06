// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE OpenMSFile test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/OpenMSFile.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(fileopenms)

BOOST_AUTO_TEST_CASE(loadStandardsConcentrations)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  SequenceSegmentHandler ssh;
  OpenMSFile::loadStandardsConcentrations(ssh, pathname);
  const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& rc = ssh.getStandardsConcentrations();

  BOOST_CHECK_EQUAL(rc.size(), 8);

  BOOST_CHECK_EQUAL(rc[0].sample_name, "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(rc[0].component_name, "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(rc[0].IS_component_name, "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(rc[0].actual_concentration, 0.0, 1e-6);
  BOOST_CHECK_CLOSE(rc[0].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[0].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[0].dilution_factor, 1.0, 1e-6);

  BOOST_CHECK_EQUAL(rc[4].sample_name, "150516_CM3_Level9");
  BOOST_CHECK_EQUAL(rc[4].component_name, "ump.ump_2.Light");
  BOOST_CHECK_EQUAL(rc[4].IS_component_name, "ump.ump_1.Heavy");
  BOOST_CHECK_CLOSE(rc[4].actual_concentration, 0.016, 1e-6);
  BOOST_CHECK_CLOSE(rc[4].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[4].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[4].dilution_factor, 1.0, 1e-6);

  BOOST_CHECK_EQUAL(rc[7].sample_name, "150516_CM3_Level9");
  BOOST_CHECK_EQUAL(rc[7].component_name, "utp.utp_2.Light");
  BOOST_CHECK_EQUAL(rc[7].IS_component_name, "utp.utp_1.Heavy");
  BOOST_CHECK_CLOSE(rc[7].actual_concentration, 0.0, 1e-6);
  BOOST_CHECK_CLOSE(rc[7].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[7].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[7].dilution_factor, 1.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadQuantitationMethods)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler ssh;
  OpenMSFile::loadQuantitationMethods(ssh, pathname);
  const std::vector<OpenMS::AbsoluteQuantitationMethod>& aqm = ssh.getQuantitationMethods();

  BOOST_CHECK_EQUAL(aqm.size(), 107);

  BOOST_CHECK_EQUAL(aqm[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(aqm[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_EQUAL(aqm[0].getISName(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(aqm[0].getConcentrationUnits(), "uM");
  BOOST_CHECK_EQUAL(aqm[0].getTransformationModel(), "linear");
  BOOST_CHECK_CLOSE(aqm[0].getLLOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getULOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getLLOQ(), 0.25, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getULOQ(), 2.5, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getCorrelationCoefficient(), 0.983846949, 1e-6);
  BOOST_CHECK_EQUAL(aqm[0].getNPoints(), 4);
  const OpenMS::Param params1 = aqm[0].getTransformationModelParams();
  BOOST_CHECK_CLOSE(static_cast<double>(params1.getValue("slope")), 2.429728323, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(params1.getValue("intercept")), -0.091856745, 1e-6);

  BOOST_CHECK_EQUAL(aqm[106].getComponentName(), "xan.xan_1.Light");
  BOOST_CHECK_EQUAL(aqm[106].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_EQUAL(aqm[106].getISName(), "xan.xan_1.Heavy");
  BOOST_CHECK_EQUAL(aqm[106].getConcentrationUnits(), "uM");
  BOOST_CHECK_EQUAL(aqm[106].getTransformationModel(), "linear");
  BOOST_CHECK_CLOSE(aqm[106].getLLOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getULOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getLLOQ(), 0.004, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getULOQ(), 0.16, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getCorrelationCoefficient(), 0.994348761, 1e-6);
  BOOST_CHECK_EQUAL(aqm[106].getNPoints(), 6);
  const OpenMS::Param params2 = aqm[106].getTransformationModelParams();
  BOOST_CHECK_CLOSE(static_cast<double>(params2.getValue("slope")), 1.084995619, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(params2.getValue("intercept")), -0.00224781, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadTraML)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_traML_1.csv");
  RawDataHandler rawDataHandler;
  OpenMSFile::loadTraML(rawDataHandler, pathname, "csv");
  const std::vector<OpenMS::ReactionMonitoringTransition>& t = rawDataHandler.getTargetedExperiment().getTransitions();

  BOOST_CHECK_EQUAL(t.size(), 20);

  BOOST_CHECK_EQUAL(t[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_CLOSE(t[0].getPrecursorMZ(), 179.0, 1e-6);
  BOOST_CHECK_CLOSE(t[0].getProductMZ(), 136.0, 1e-6);

  BOOST_CHECK_EQUAL(t[10].getPeptideRef(), "citr-L");
  BOOST_CHECK_CLOSE(t[10].getPrecursorMZ(), 180.0, 1e-6);
  BOOST_CHECK_CLOSE(t[10].getProductMZ(), 136.0, 1e-6);

  BOOST_CHECK_EQUAL(t[19].getPeptideRef(), "Lcystin");
  BOOST_CHECK_CLOSE(t[19].getPrecursorMZ(), 239.0, 1e-6);
  BOOST_CHECK_CLOSE(t[19].getProductMZ(), 120.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadMSExperiment)
{
}

BOOST_AUTO_TEST_CASE(loadFeatureMap)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_test_1_io_FileReaderOpenMS.featureXML");
  RawDataHandler rawDataHandler;
  OpenMSFile::loadFeatureMap(rawDataHandler, pathname);
  const OpenMS::FeatureMap& fm = rawDataHandler.getFeatureMap();

  BOOST_CHECK_EQUAL(fm.size(), 481);

  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getSubordinates()[0].getMetaValue("peak_apex_int")), 266403.0, 1e-6);
  BOOST_CHECK_EQUAL(fm[0].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fm[0].getSubordinates()[0].getRT()), 15.8944563381195, 1e-6);

  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getSubordinates()[0].getMetaValue("peak_apex_int")), 3436.0, 1e-6);
  BOOST_CHECK_EQUAL(fm[1].getSubordinates()[0].getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<double>(fm[1].getSubordinates()[0].getRT()), 16.2997193464915, 1e-6);
}

BOOST_AUTO_TEST_CASE(loadFeatureFilter)
{
}

BOOST_AUTO_TEST_CASE(loadFeatureQC)
{
}

BOOST_AUTO_TEST_CASE(readRawDataProcessingParameters)
{
}

BOOST_AUTO_TEST_CASE(parse_rawDataProcessingParameters)
{
}

BOOST_AUTO_TEST_CASE(storeQuantitationMethods)
{
}

BOOST_AUTO_TEST_CASE(storeFeatureMap)
{
}

BOOST_AUTO_TEST_CASE(storeMzML)
{
}

BOOST_AUTO_TEST_SUITE_END()
