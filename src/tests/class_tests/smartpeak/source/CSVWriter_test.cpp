// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/CSVWriter.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>

using namespace SmartPeak;
using namespace std;

CSVWriter* ptr = nullptr;
CSVWriter* const nullPointer = nullptr;

TEST(CSVWriter, constructor)
{
  ptr = new CSVWriter();
  EXPECT_NE(ptr, nullPointer);
  delete ptr;
}

TEST(CSVWriter, constructor2)
{
  CSVWriter csvwriter("filename1", ";");

  EXPECT_STREQ(csvwriter.getFilename().c_str(), "filename1");
  EXPECT_STREQ(csvwriter.getDelimeter().c_str(), ";");
  EXPECT_EQ(csvwriter.getLineCount(), 0);
}

TEST(CSVWriter, gettersAndSetters)
{
  CSVWriter csvwriter;
  csvwriter.setFilename("filename1");
  csvwriter.setDelimeter(";");
  csvwriter.setLineCount(1);

  EXPECT_STREQ(csvwriter.getFilename().c_str(), "filename1");
  EXPECT_STREQ(csvwriter.getDelimeter().c_str(), ";");
  EXPECT_EQ(csvwriter.getLineCount(), 1);
}

TEST(CSVWriter, writeDataInRow)
{
  const std::string filename = SMARTPEAK_GET_TEST_DATA_PATH("output/CSVWriterTest.csv");
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
  test_in.read_header(io::ignore_extra_column, "Column1", "Column2", "Column3");
  std::string col1, col2, col3;

  test_in.read_row(col1, col2, col3);
  EXPECT_STREQ(col1.c_str(), "a");
  EXPECT_STREQ(col2.c_str(), "b");
  EXPECT_STREQ(col3.c_str(), "c");

  test_in.read_row(col1, col2, col3);
  EXPECT_STREQ(col1.c_str(), "1");
  EXPECT_STREQ(col2.c_str(), "2");
  EXPECT_STREQ(col3.c_str(), "3");
}
