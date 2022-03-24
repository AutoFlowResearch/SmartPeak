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
  SetRawDataPathname cmd(&ah_.filenames_);
  EXPECT_STREQ(ah_.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH).c_str(), "");
  EXPECT_TRUE(cmd.onFilePicked("test", &ah_));
  EXPECT_STREQ(ah_.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH).c_str(), "test");
}

/* SetInputFeaturesPathname */
TEST_F(ApplicationHandlerFixture, SetInputFeaturesPathname_ProcessSetsPath)
{
  SetInputFeaturesPathname cmd(&ah_.filenames_);
  EXPECT_STREQ(ah_.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH).c_str(), "");
  EXPECT_TRUE(cmd.onFilePicked("test", &ah_));
  EXPECT_STREQ(ah_.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH).c_str(), "test");
}

/* SetOutputFeaturesPathname */
TEST_F(ApplicationHandlerFixture, SetOutputFeaturesPathname_ProcessSetsPath)
{
  SetOutputFeaturesPathname cmd(&ah_.filenames_);
  EXPECT_STREQ(ah_.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH).c_str(), "");
  EXPECT_TRUE(cmd.onFilePicked("test", &ah_));
  EXPECT_STREQ(ah_.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH).c_str(), "test");
}

TEST_F(ApplicationHandlerFixture, sessionIsOpen)
{
  EXPECT_FALSE(ah_.sessionIsOpened());
  ah_.filenames_.setFullPath("test", "test");
  EXPECT_TRUE(ah_.sessionIsOpened());
}

TEST_F(ApplicationHandlerFixture, sessionIsSaved)
{
  EXPECT_TRUE(ah_.sessionIsSaved());
  ah_.setFileSavedState("test", false);
  EXPECT_FALSE(ah_.sessionIsSaved());
  ah_.setFileSavedState("test", true);
  EXPECT_TRUE(ah_.sessionIsSaved());
}

TEST_F(ApplicationHandlerFixture, setFileSavedState)
{
  EXPECT_TRUE(ah_.isFileSaved("test"));
  ah_.setFileSavedState("test", false);
  EXPECT_FALSE(ah_.isFileSaved("test"));
  ah_.setFileSavedState("test", true);
  EXPECT_TRUE(ah_.isFileSaved("test"));
}

TEST_F(ApplicationHandlerFixture, getWorkflowParameterSchema)
{
  ah_.sequenceHandler_.setWorkflow({ "LOAD_RAW_DATA", "CALCULATE_MDV_ACCURACIES" });
  auto schema_params = ah_.getWorkflowParameterSchema();
  EXPECT_EQ(schema_params.size(), 3);
}