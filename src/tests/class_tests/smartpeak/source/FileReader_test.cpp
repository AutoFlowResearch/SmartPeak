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
  ParameterSet parameters;
  FileReader::parseOpenMSParams(pathname, parameters);
  BOOST_CHECK_EQUAL(parameters.size(), 3);

  const string func1 {"func1"};

  BOOST_REQUIRE(parameters.count(func1));
  auto function_parameters = parameters.at(func1);
  BOOST_REQUIRE(function_parameters.size() == 3);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "int");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");

  BOOST_CHECK_EQUAL(function_parameters[1].getName(), "param2");
  BOOST_CHECK_EQUAL(function_parameters[1].getType(), "float");
  BOOST_CHECK_EQUAL(function_parameters[1].getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(function_parameters[1].getDescription(), "a description");

  BOOST_CHECK_EQUAL(function_parameters[2].getName(), "param3");
  BOOST_CHECK_EQUAL(function_parameters[2].getType(), "float");
  BOOST_CHECK_EQUAL(function_parameters[2].getValueAsString(), "-1");
  BOOST_CHECK_EQUAL(function_parameters[2].getDescription(), "a quoted description, containing a comma");

  const string func2 {"func2"};

  BOOST_REQUIRE(parameters.count(func2));
  function_parameters = parameters.at(func2);
  BOOST_REQUIRE(function_parameters.size() == 2);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "float");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "0.5");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");

  BOOST_CHECK_EQUAL(function_parameters[1].getName(), "param2");
  BOOST_CHECK_EQUAL(function_parameters[1].getType(), "bool");
  BOOST_CHECK_EQUAL(function_parameters[1].getValueAsString(), "false");
  BOOST_CHECK_EQUAL(function_parameters[1].getDescription(), "");

  const string func3 {"func3"};

  BOOST_REQUIRE(parameters.count(func3));
  function_parameters = parameters.at(func3);
  BOOST_REQUIRE(function_parameters.size() == 1);
  BOOST_CHECK_EQUAL(function_parameters[0].getName(), "param1");
  BOOST_CHECK_EQUAL(function_parameters[0].getType(), "bool");
  BOOST_CHECK_EQUAL(function_parameters[0].getValueAsString(), "false");
  BOOST_CHECK_EQUAL(function_parameters[0].getDescription(), "");
}

BOOST_AUTO_TEST_SUITE_END()
