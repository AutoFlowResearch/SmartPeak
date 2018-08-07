/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE CSVWriter test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/CSVWriter.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(CSVWriter1)

BOOST_AUTO_TEST_CASE(constructor) 
{
  CSVWriter* ptr = nullptr;
  CSVWriter* nullPointer = nullptr;
  ptr = new CSVWriter();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  CSVWriter* ptr = nullptr;
	ptr = new CSVWriter();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(constructor2) 
{
  CSVWriter csvwriter("filename1", ";");

  BOOST_CHECK_EQUAL(csvwriter.getFilename(), "filename1");
  BOOST_CHECK_EQUAL(csvwriter.getDelimeter(), ";");
  BOOST_CHECK_EQUAL(csvwriter.getLineCount(), 0);
}

BOOST_AUTO_TEST_CASE(gettersAndSetters) 
{
  CSVWriter csvwriter;
  csvwriter.setFilename("filename1");
  csvwriter.setDelimeter(";");
  csvwriter.setLineCount(1);

  BOOST_CHECK_EQUAL(csvwriter.getFilename(), "filename1");
  BOOST_CHECK_EQUAL(csvwriter.getDelimeter(), ";");
  BOOST_CHECK_EQUAL(csvwriter.getLineCount(), 1);
}

BOOST_AUTO_TEST_CASE(writeDataInRow) 
{
  std::string filename = "CSVWriterTest.csv";
  std::vector<std::string> headers, line;  
  CSVWriter csvwriter(filename);

  // Write the data to file
  headers = {"Column1", "Column2", "Column3"};
	csvwriter.writeDataInRow(headers.begin(), headers.end());
  line = {"a", "b", "c" };
	csvwriter.writeDataInRow(line.begin(), line.end());
  line = {"1", "2", "3" };
	csvwriter.writeDataInRow(line.begin(), line.end());

  // Read the data back in
  io::CSVReader<3> test_in(filename);
  test_in.read_header(io::ignore_extra_column, 
    "Column1", "Column2", "Column3");
  std::string col1, col2, col3;

  int cnt = 0;
  while(test_in.read_row(col1, col2, col3))
  {
    if (cnt == 0)
    {
      BOOST_CHECK_EQUAL(col1, "a");
      BOOST_CHECK_EQUAL(col2, "b");
      BOOST_CHECK_EQUAL(col3, "c");
    }
    else if (cnt == 1)
    {
      BOOST_CHECK_EQUAL(col1, "1");
      BOOST_CHECK_EQUAL(col2, "2");
      BOOST_CHECK_EQUAL(col3, "3");
    }
    cnt += 1;
  }
}

BOOST_AUTO_TEST_SUITE_END()