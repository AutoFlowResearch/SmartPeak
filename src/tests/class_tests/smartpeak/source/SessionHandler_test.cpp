// TODO: Add copyright

#include <SmartPeak/test_config.h>
#define BOOST_TEST_MODULE SessionHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(SessionHandler1)

struct TestData {
  TestData(){
    const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");
    Filenames filenames;
    filenames = Filenames::getDefaultStaticFilenames(pathname);
    Filenames::updateDefaultDynamicFilenames(
      pathname + "/mzML",
      pathname + "/features",
      pathname + "/features",
      filenames);
    CreateSequence cs(sequenceHandler);
    cs.filenames = filenames;
    cs.delimiter = ",";
    cs.checkConsistency = false;
    cs.process();
  }
  SequenceHandler sequenceHandler;
};

BOOST_AUTO_TEST_CASE(setInjectionExplorer1)
{
  TestData testData;
  SessionHandler::getInstance().setInjectionExplorer();
}

BOOST_AUTO_TEST_CASE(setTransitionExplorer1)
{
  TestData testData;
  SessionHandler::getInstance().setTransitionExplorer();
}

BOOST_AUTO_TEST_CASE(setFeatureExplorer1)
{
  TestData testData;
  SessionHandler::getInstance().setFeatureExplorer();
}

BOOST_AUTO_TEST_CASE(setSequenceTable1)
{
  TestData testData;
  SessionHandler::getInstance().setSequenceTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setTransitionsTable1)
{
  TestData testData;
  SessionHandler::getInstance().setTransitionsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setWorkflowTable1)
{
  TestData testData;
  //SessionHandler::getInstance().setWorkflowTable();
}

BOOST_AUTO_TEST_CASE(setParametersTable1)
{
  TestData testData;
  SessionHandler::getInstance().setParametersTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setQuantMethodTable1)
{
  TestData testData;
  SessionHandler::getInstance().setQuantMethodTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setStdsConcsTable1)
{
  TestData testData;
  SessionHandler::getInstance().setStdsConcsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentFiltersTable1)
{
  TestData testData;
  SessionHandler::getInstance().setComponentFiltersTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentGroupFiltersTable1)
{
  TestData testData;
  SessionHandler::getInstance().setComponentGroupFiltersTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentQCsTable1)
{
  TestData testData;
  SessionHandler::getInstance().setComponentQCsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentGroupQCsTable1)
{
  TestData testData;
  SessionHandler::getInstance().setComponentGroupQCsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setFeatureTable1)
{
  TestData testData;
  SessionHandler::getInstance().setFeatureTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setFeatureMatrix1)
{
  TestData testData;
  SessionHandler::getInstance().setFeatureMatrix(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setChromatogramScatterPlot1)
{
  TestData testData;
  SessionHandler::getInstance().setChromatogramScatterPlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setCalibratorsScatterLinePlot1)
{
  TestData testData;
  SessionHandler::getInstance().setCalibratorsScatterLinePlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setSequenceDataAndFilters1)
{
  TestData testData;
  SessionHandler::getInstance().setSequenceDataAndFilters(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setTransitionDataAndFilters1)
{
  TestData testData;
  SessionHandler::getInstance().setTransitionsDataAndFilters(testData.sequenceHandler);
}

BOOST_AUTO_TEST_SUITE_END()
