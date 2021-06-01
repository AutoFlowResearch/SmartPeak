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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE ApplicationProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/ApplicationHandler.h>
#include <filesystem>

using namespace SmartPeak;
using namespace std;

struct ApplicationHandlerFixture 
{
  /* ctor/dtor */
  ApplicationHandlerFixture() {}

  ~ApplicationHandlerFixture() {}

public:
  ApplicationHandler ah_;
};

BOOST_FIXTURE_TEST_SUITE(ApplicationHandler, ApplicationHandlerFixture)

/* SetRawDataPathname */
BOOST_AUTO_TEST_CASE(SetRawDataPathname_ProcessSetsPath)
{
  SetRawDataPathname cmd;
  BOOST_CHECK_EQUAL(ah_.mzML_dir_, "");
  BOOST_CHECK(cmd.onFilePicked("test", &ah_));
  BOOST_CHECK_EQUAL(ah_.mzML_dir_, "test");
}

/* SetInputFeaturesPathname */
BOOST_AUTO_TEST_CASE(SetInputFeaturesPathname_ProcessSetsPath)
{
  SetInputFeaturesPathname cmd;
  BOOST_CHECK_EQUAL(ah_.features_in_dir_, "");
  BOOST_CHECK(cmd.onFilePicked("test", &ah_));
  BOOST_CHECK_EQUAL(ah_.features_in_dir_, "test");
}

/* SetOutputFeaturesPathname */
BOOST_AUTO_TEST_CASE(SetOutputFeaturesPathname_ProcessSetsPath)
{
  SetOutputFeaturesPathname cmd;
  BOOST_CHECK_EQUAL(ah_.features_out_dir_, "");
  BOOST_CHECK(cmd.onFilePicked("test", &ah_));
  BOOST_CHECK_EQUAL(ah_.features_out_dir_, "test");
}

BOOST_AUTO_TEST_SUITE_END()
