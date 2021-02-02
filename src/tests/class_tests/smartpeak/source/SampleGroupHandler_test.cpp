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

#define BOOST_TEST_MODULE SampleGroupHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleGroupHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(samplegrouphandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  SampleGroupHandler* ptr = nullptr;
  SampleGroupHandler* nullPointer = nullptr;
  ptr = new SampleGroupHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
  delete ptr;
}

BOOST_AUTO_TEST_CASE(set_get_SampleGroupName)
{
  SampleGroupHandler sg;
  const string foo {"foo"};

  sg.setSampleGroupName(foo);

  const string& name1 = sg.getSampleGroupName(); // testing const getter
  BOOST_CHECK_EQUAL(name1, foo);

  const string bar {"bar"};
  sg.getSampleGroupName() = bar;

  string& name2 = sg.getSampleGroupName(); // testing non-const getter
  BOOST_CHECK_EQUAL(name2, bar);
}

BOOST_AUTO_TEST_CASE(set_get_SampleIndices)
{
  SampleGroupHandler sg;
  vector<size_t> si1 {1, 3, 5};

  sg.setSampleIndices(si1);

  const vector<size_t>& si2 = sg.getSampleIndices(); // testing const getter
  BOOST_CHECK_EQUAL(si2.size(), 3);
  BOOST_CHECK_EQUAL(si2[0], 1);
  BOOST_CHECK_EQUAL(si2[1], 3);
  BOOST_CHECK_EQUAL(si2[2], 5);

  sg.getSampleIndices()[1] = 2;

  const vector<size_t>& si3 = sg.getSampleIndices(); // testing non-const getter
  BOOST_CHECK_EQUAL(si3.size(), 3);
  BOOST_CHECK_EQUAL(si3[1], 2);
}

BOOST_AUTO_TEST_SUITE_END()
