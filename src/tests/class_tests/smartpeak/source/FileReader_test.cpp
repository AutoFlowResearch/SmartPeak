// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE FileReader test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/FileReader.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(filereader)

// constructor and destructor are deleted

// BOOST_AUTO_TEST_CASE(constructor)
// {
//   FileReader* ptr = nullptr;
//   FileReader* nullPointer = nullptr;
//   ptr = new FileReader();
//   BOOST_CHECK_NE(ptr, nullPointer);
// }

// BOOST_AUTO_TEST_CASE(destructor)
// {
//   FileReader* ptr = nullptr;
//   ptr = new FileReader();
//   delete ptr;
// }

BOOST_AUTO_TEST_CASE(parseopenmsparams)
{
  const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  std::map<std::string,std::vector<std::map<std::string,std::string>>> parameters;
  FileReader::parse_OpenMSParams(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()
