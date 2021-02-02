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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE FileReader test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/FileReader.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(filereader)

BOOST_AUTO_TEST_CASE(parseopenmsparams)
{
  const string pathname = SMARTPEAK_GET_TEST_DATA_PATH("FileReader_parameters.csv");
  map<string,vector<map<string,string>>> parameters;
  FileReader::parseOpenMSParams(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);

  const string func1 {"func1"};

  BOOST_CHECK_EQUAL(parameters[func1].size(), 3);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][0]["name"], "param1");
  BOOST_CHECK_EQUAL(parameters[func1][0]["type"], "int");
  BOOST_CHECK_EQUAL(parameters[func1][0]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][0]["description"], "");

  BOOST_CHECK_EQUAL(parameters[func1][1].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][1]["name"], "param2");
  BOOST_CHECK_EQUAL(parameters[func1][1]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func1][1]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][1]["description"], "a description");

  BOOST_CHECK_EQUAL(parameters[func1][2].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2]["name"], "param3");
  BOOST_CHECK_EQUAL(parameters[func1][2]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func1][2]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][2]["description"], "a quoted description, containing a comma");

  BOOST_CHECK_EQUAL(parameters[func1][2].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func1][2]["name"], "param3");
  BOOST_CHECK_EQUAL(parameters[func1][2]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func1][2]["value"], "-1");
  BOOST_CHECK_EQUAL(parameters[func1][2]["description"], "a quoted description, containing a comma");

  const string func2 {"func2"};

  BOOST_CHECK_EQUAL(parameters[func2].size(), 2);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][0]["name"], "param1");
  BOOST_CHECK_EQUAL(parameters[func2][0]["type"], "float");
  BOOST_CHECK_EQUAL(parameters[func2][0]["value"], "0.5");
  BOOST_CHECK_EQUAL(parameters[func2][0]["description"], "");

  BOOST_CHECK_EQUAL(parameters[func2][1].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func2][1]["name"], "param2");
  BOOST_CHECK_EQUAL(parameters[func2][1]["type"], "bool");
  BOOST_CHECK_EQUAL(parameters[func2][1]["value"], "FALSE");
  BOOST_CHECK_EQUAL(parameters[func2][1]["description"], "");

  const string func3 {"func3"};

  BOOST_CHECK_EQUAL(parameters[func3].size(), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0].count("description"), 1);
  BOOST_CHECK_EQUAL(parameters[func3][0]["name"], "param1");
  BOOST_CHECK_EQUAL(parameters[func3][0]["type"], "bool");
  BOOST_CHECK_EQUAL(parameters[func3][0]["value"], "FALSE");
  BOOST_CHECK_EQUAL(parameters[func3][0]["description"], "");
}

BOOST_AUTO_TEST_SUITE_END()
