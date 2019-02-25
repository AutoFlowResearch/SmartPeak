// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE InputDataValidation test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(inputdatavalidation)

BOOST_AUTO_TEST_CASE(fileExists)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  BOOST_CHECK_EQUAL(InputDataValidation::fileExists(pathname), true);
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("this_does_not_exist.csv");
  BOOST_CHECK_EQUAL(InputDataValidation::fileExists(pathname), false);
}

BOOST_AUTO_TEST_CASE(isValidFilename)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  BOOST_CHECK_EQUAL(InputDataValidation::isValidFilename(pathname, "some standards file"), true);  // success
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("this_does_not_exist.csv");
  BOOST_CHECK_EQUAL(InputDataValidation::isValidFilename(pathname, "a file that does not exist"), false); // failure
  pathname.clear();
  BOOST_CHECK_EQUAL(InputDataValidation::isValidFilename(pathname, "an empty pathname"), true);  // not provided
}

BOOST_AUTO_TEST_CASE(validateNamesInStructures)
{
  const set<string> s1 = {"1", "2", "3"};
  const set<string> s2 = {"1", "2", "4"};
  const set<string> s3 = {"1", "2", "3", "4"};
  BOOST_CHECK_EQUAL(InputDataValidation::validateNamesInStructures(s1, s1, "", "", true), true); // check itself
  BOOST_CHECK_EQUAL(InputDataValidation::validateNamesInStructures(s1, s2, "", "", false), false); // s1 -> s2, "3" is missing
  BOOST_CHECK_EQUAL(InputDataValidation::validateNamesInStructures(s2, s1, "", "", false), false); // s1 -> s2, "4" is missing
  BOOST_CHECK_EQUAL(InputDataValidation::validateNamesInStructures(s2, s1, "", "", true), false); // s1 <-> s2, "3" and "4" are missing
  BOOST_CHECK_EQUAL(InputDataValidation::validateNamesInStructures(s1, s3, "", "", false), true); // s1 -> s3, no name is missing
  BOOST_CHECK_EQUAL(InputDataValidation::validateNamesInStructures(s1, s3, "", "", true), false); // s1 <-> s3, "4" is missing
}

BOOST_AUTO_TEST_CASE(findMissingNames)
{
  const set<string> s1 = {"1", "2", "3"};
  const set<string> s2 = {"1", "2", "4"};
  const set<string> s3 = {"1", "2", "3", "4"};
  set<string> result;

  result = InputDataValidation::findMissingNames(s1, s1);
  BOOST_CHECK_EQUAL(result.size(), 0);

  result = InputDataValidation::findMissingNames(s1, s2);
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.count("3"), 1);

  result = InputDataValidation::findMissingNames(s2, s1);
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.count("4"), 1);

  result = InputDataValidation::findMissingNames(s1, s3);
  BOOST_CHECK_EQUAL(result.size(), 0);

  result = InputDataValidation::findMissingNames(s3, s1);
  BOOST_CHECK_EQUAL(result.size(), 1);
  BOOST_CHECK_EQUAL(result.count("4"), 1);
}

BOOST_AUTO_TEST_CASE(sampleNamesAreConsistent)
{
  SequenceHandler sequenceHandler;
  const std::string main_dir = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");
  Filenames filenames = Filenames::getDefaultStaticFilenames(main_dir);
  SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);

  bool result;

  result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
  BOOST_CHECK_EQUAL(result, true);

  filenames.sequence_csv_i = main_dir + "/sequence_missing.csv";
  SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);

  result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
  BOOST_CHECK_EQUAL(result, false); // missing sample: 150516_CM3_Level900
}

// BOOST_AUTO_TEST_CASE(componentNamesAreConsistent)
// {
//   SequenceHandler sequenceHandler;
//   const std::string main_dir = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");
//   // Filenames filenames = Filenames::getDefaultStaticFilenames(main_dir + "/LCMS_MRM_Standards");
//   Filenames filenames = Filenames::getDefaultStaticFilenames(main_dir + "/Fake_workflow");
//   SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);

//   bool result;

//   result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
//   BOOST_CHECK_EQUAL(result, true);

//   result = InputDataValidation::componentNamesAreConsistent(sequenceHandler);
//   BOOST_CHECK_EQUAL(result, true);

//   result = InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler);
//   BOOST_CHECK_EQUAL(result, true);

//   result = InputDataValidation::heavyComponentsAreConsistent(sequenceHandler);
//   BOOST_CHECK_EQUAL(result, true); // g6p.g6p_2.Heavy is missing

//   std::vector<OpenMS::ReactionMonitoringTransition> transitions =
//     sequenceHandler.getSequence().front().getRawData().getTargetedExperiment().getTransitions();

//   // remove all missing component names and repeat the test
// }

BOOST_AUTO_TEST_SUITE_END()
