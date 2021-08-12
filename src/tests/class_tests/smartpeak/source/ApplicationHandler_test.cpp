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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud, Ahmed Khalil $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <filesystem>

using namespace SmartPeak;
using namespace std;

struct ApplicationHandlerFixture : public ::testing::Test
{
  /* ctor/dtor */
  ApplicationHandlerFixture() {}

  ~ApplicationHandlerFixture() {}

public:
  ApplicationHandler ah_;
};

/* SetRawDataPathname */
TEST_F(ApplicationHandlerFixture, SetRawDataPathname_ProcessSetsPath)
{
  SetRawDataPathname cmd;
  EXPECT_STREQ(ah_.mzML_dir_.generic_string().c_str(), "");
  EXPECT_TRUE(cmd.onFilePicked("test", &ah_));
  EXPECT_STREQ(ah_.mzML_dir_.generic_string().c_str(), "test");
}

/* SetInputFeaturesPathname */
TEST_F(ApplicationHandlerFixture, SetInputFeaturesPathname_ProcessSetsPath)
{
  SetInputFeaturesPathname cmd;
  EXPECT_STREQ(ah_.features_in_dir_.generic_string().c_str(), "");
  EXPECT_TRUE(cmd.onFilePicked("test", &ah_));
  EXPECT_STREQ(ah_.features_in_dir_.generic_string().c_str(), "test");
}

/* SetOutputFeaturesPathname */
TEST_F(ApplicationHandlerFixture, SetOutputFeaturesPathname_ProcessSetsPath)
{
  SetOutputFeaturesPathname cmd;
  EXPECT_STREQ(ah_.features_out_dir_.generic_string().c_str(), "");
  EXPECT_TRUE(cmd.onFilePicked("test", &ah_));
  EXPECT_STREQ(ah_.features_out_dir_.generic_string().c_str(), "test");
}
