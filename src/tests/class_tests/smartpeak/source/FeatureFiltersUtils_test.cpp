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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/core/ApplicationProcessor.h>

using namespace SmartPeak;
using namespace std;

TEST(FeatureFiltersUtils, storeAndLoadFeatureFiltersInDB)
{
  const std::string filename = SMARTPEAK_GET_TEST_DATA_PATH("FeatureFilters_with_metadata.csv");
  OpenMS::MRMFeatureQC features_qc_1;

  Filenames filenames;
  filenames.setFullPath("test", filename);
  filenames.setFullPath("testgroup", "");
  FeatureFiltersUtils::loadFeatureFilters("test", "testgroup", filenames, features_qc_1, nullptr, nullptr);

  // Check the file loading succeeded
  ASSERT_EQ(features_qc_1.component_qcs.size(), 22);
  auto feature_qc_1 = features_qc_1.component_qcs[0];
  EXPECT_EQ(feature_qc_1.meta_value_qc.size(), 9);
  ASSERT_EQ(feature_qc_1.meta_value_qc.count("width_at_50"), 1);
  EXPECT_FLOAT_EQ(feature_qc_1.meta_value_qc.at("width_at_50").first, 0.001);
  EXPECT_FLOAT_EQ(feature_qc_1.meta_value_qc.at("width_at_50").second, 0.25);

  // Store in DB
  auto path_db = std::tmpnam(nullptr);
  filenames.getSessionDB().setDBFilePath(path_db);
  filenames.setEmbedded("test", true);
  FeatureFiltersUtils::storeFeatureFilters("test", "testgroup", filenames, features_qc_1);

  // Load from DB
  OpenMS::MRMFeatureQC features_qc_2;
  FeatureFiltersUtils::loadFeatureFilters("test", "testgroup", filenames, features_qc_2, nullptr, nullptr);

  // Check the loading from DB succeeded
  ASSERT_EQ(features_qc_2.component_qcs.size(), 22);
  auto feature_qc_2 = features_qc_2.component_qcs[0];
  EXPECT_EQ(feature_qc_2.meta_value_qc.size(), 9);
  ASSERT_EQ(feature_qc_2.meta_value_qc.count("width_at_50"), 1);
  EXPECT_FLOAT_EQ(feature_qc_2.meta_value_qc.at("width_at_50").first, 0.001);
  EXPECT_FLOAT_EQ(feature_qc_2.meta_value_qc.at("width_at_50").second, 0.25);
}

TEST(FeatureFiltersUtils, storeAndLoadFeatureFiltersGroupsInDB)
{
  const std::string filename = SMARTPEAK_GET_TEST_DATA_PATH("FeatureFiltersGroups_with_metadata.csv");
  OpenMS::MRMFeatureQC features_qc_1;

  Filenames filenames;
  filenames.setFullPath("test", "");
  filenames.setFullPath("testgroup", filename);
  FeatureFiltersUtils::loadFeatureFilters("test", "testgroup", filenames, features_qc_1, nullptr, nullptr);

  // Check the file loading succeeded
  ASSERT_EQ(features_qc_1.component_group_qcs.size(), 11);
  auto feature_qc_1 = features_qc_1.component_group_qcs[0];
  EXPECT_EQ(feature_qc_1.meta_value_qc.size(), 9);
  ASSERT_EQ(feature_qc_1.meta_value_qc.count("width_at_50"), 1);
  EXPECT_FLOAT_EQ(feature_qc_1.meta_value_qc.at("width_at_50").first, 0.001);
  EXPECT_FLOAT_EQ(feature_qc_1.meta_value_qc.at("width_at_50").second, 0.25);

  // Store in DB
  auto path_db = std::tmpnam(nullptr);
  filenames.getSessionDB().setDBFilePath(path_db);
  filenames.setEmbedded("testgroup", true);
  FeatureFiltersUtils::storeFeatureFilters("test", "testgroup", filenames, features_qc_1);

  // Load from DB
  OpenMS::MRMFeatureQC features_qc_2;
  FeatureFiltersUtils::loadFeatureFilters("test", "testgroup", filenames, features_qc_2, nullptr, nullptr);

  // Check the loading from DB succeeded
  ASSERT_EQ(features_qc_2.component_group_qcs.size(), 11);
  auto feature_qc_2 = features_qc_2.component_group_qcs[0];
  EXPECT_EQ(feature_qc_2.meta_value_qc.size(), 9);
  ASSERT_EQ(feature_qc_2.meta_value_qc.count("width_at_50"), 1);
  EXPECT_FLOAT_EQ(feature_qc_2.meta_value_qc.at("width_at_50").first, 0.001);
  EXPECT_FLOAT_EQ(feature_qc_2.meta_value_qc.at("width_at_50").second, 0.25);
}
