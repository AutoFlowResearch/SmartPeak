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
#include <SmartPeak/io/SelectiveDilutionsParser.h>

using namespace SmartPeak;
using namespace std;

TEST(SelectiveDilutionsParser, read)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SampleGroupProcessor_selectiveDilutions.csv");
  std::map<std::string, int> dilution_map;
  SelectiveDilutionsParser::read(pathname, dilution_map);
  ASSERT_EQ(dilution_map.size(), 2);
  ASSERT_EQ(dilution_map.count("test1_1"), 1);
  EXPECT_EQ(dilution_map["test1_1"], 10);
  ASSERT_EQ(dilution_map.count("test1_2"), 1);
  EXPECT_EQ(dilution_map["test1_2"], 1);
}

TEST(SelectiveDilutionsParser, read_invalid)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SelectiveDilutionsParser_invalid.csv");
  try {
    std::map<std::string, int> dilution_map;
    SelectiveDilutionsParser::read(pathname, dilution_map);
    FAIL() << "Expected std::exception";
  }
  catch (std::exception const& err) {
  }
  catch (...) {
    FAIL() << "Expected std::exception";
  }
}


TEST(SelectiveDilutionsParser, read_missing_columns)
{
  std::map<std::string, int> dilution_map;
  try {
    SelectiveDilutionsParser::read(SMARTPEAK_GET_TEST_DATA_PATH("SelectiveDilutionsParser_missing_compound.csv"), dilution_map);
    FAIL() << "Expected std::invalid_argument";
  }
  catch (std::invalid_argument const& err) {
    EXPECT_STREQ(err.what(), "Missing required column 'compound'");
    EXPECT_EQ(dilution_map.size(), 0);
  }
  catch (...) {
    FAIL() << "Expected std::invalid_argument";
  }

  try {
    SelectiveDilutionsParser::read(SMARTPEAK_GET_TEST_DATA_PATH("SelectiveDilutionsParser_missing_dilution_factor.csv"), dilution_map);
    FAIL() << "Expected std::invalid_argument";
  }
  catch (std::invalid_argument const& err) {
    EXPECT_STREQ(err.what(), "Missing required column 'dilution_factor'");
    EXPECT_EQ(dilution_map.size(), 0);
  }
  catch (...) {
    FAIL() << "Expected std::invalid_argument";
  }
}
