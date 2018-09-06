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

// BOOST_AUTO_TEST_CASE(loadStandardsConcentrations)
// {
//   const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
//   SequenceSegmentHandler ssh;
//   OpenMSFile::loadStandardsConcentrations(ssh, pathname);
//   const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& rc = ssh.getStandardsConcentrations();

//   BOOST_CHECK_EQUAL(rc.size(), 8);
// }

BOOST_AUTO_TEST_SUITE_END()
