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

BOOST_AUTO_TEST_CASE(constructor)
{
  SessionHandler* ptr = nullptr;
  SessionHandler* nullPointer = nullptr;
  ptr = new SessionHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
  delete ptr;
}

BOOST_AUTO_TEST_CASE(setInjectionExplorer1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setInjectionExplorer(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setTransitionExplorer1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setTransitionExplorer(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setFeatureExplorer1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setFeatureExplorer();
}

BOOST_AUTO_TEST_CASE(setSequenceTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setSequenceTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setTransitionsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setTransitionsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setWorkflowTable1)
{
  TestData testData;
  SessionHandler session_handler;
  //session_handler.setWorkflowTable();
}

BOOST_AUTO_TEST_CASE(setParametersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setParametersTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setQuantMethodTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setQuantMethodTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setStdsConcsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setStdsConcsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentFiltersTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentGroupFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupFiltersTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentQCsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setComponentGroupQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupQCsTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setFeatureTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setFeatureTable(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setFeatureMatrix1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setFeatureMatrix(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setChromatogramScatterPlot1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setChromatogramScatterPlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setCalibratorsScatterLinePlot1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setCalibratorsScatterLinePlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_SUITE_END()
