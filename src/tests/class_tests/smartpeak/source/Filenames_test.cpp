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
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

TEST(Filenames, constructor)
{
  Filenames filenames;
  EXPECT_EQ(filenames.getFileIds().size(), 0);
}

TEST(Filenames, addFileName)
{
  Filenames filenames;
  filenames.addFileName("my_file", "${MAIN_DIR}/file.txt");
  const auto& file_ids = filenames.getFileIds();
  ASSERT_EQ(file_ids.size(), 1);
}

TEST(Filenames, addFileNameOverwrite)
{
  Filenames filenames;
  filenames.addFileName("my_file", "file.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file").generic_string().c_str(), "file.txt");
  filenames.addFileName("my_file", "file_non_overwrite.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file").generic_string().c_str(), "file.txt");
  filenames.addFileName("my_file", "file_overwrite.txt", "description", false, false, true);
  EXPECT_STREQ(filenames.getFullPath("my_file").generic_string().c_str(), "file_overwrite.txt");
}

TEST(Filenames, getFullPath)
{
  Filenames filenames;
  filenames.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/file_main.txt");
}

TEST(Filenames, getFullPath_non_existing)
{
  Filenames filenames;
  EXPECT_STREQ(filenames.getFullPath("test").generic_string().c_str(), "");
}

TEST(Filenames, setFullPath)
{
  Filenames filenames;
  filenames.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames.setFullPath("my_file_main", "/file/to/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/file/to/file_main.txt");
  // setting variant or root has no effect
  filenames.setTagValue(Filenames::Tag::MAIN_DIR, "/main");
  filenames.setTagValue(Filenames::Tag::MZML_INPUT_PATH, "/mzml");
  filenames.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, "/feat_input");
  filenames.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, "/feat_output");
  filenames.setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, "variant_mzml_");
  filenames.setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, "variant_input_injection_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, "variant_output_injection_");
  filenames.setTagValue(Filenames::Tag::INPUT_GROUP_NAME, "variant_input_sample_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, "variant_output_sample_");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/file/to/file_main.txt");
}

TEST(Filenames, setRootPath_setVariant_after)
{
  Filenames filenames;
  filenames.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames.addFileName("my_file_injection_input", "${FEATURES_INPUT_PATH}/${INPUT_INJECTION_NAME}file_injection_input.txt");
  filenames.addFileName("my_file_injection_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}file_injection_output.txt");
  filenames.addFileName("my_file_sequence_segment_input", "${FEATURES_INPUT_PATH}/${INPUT_SEQUENCE_SEGMENT_NAME}file_sequence_segment_input.txt");
  filenames.addFileName("my_file_sequence_segment_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_SEQUENCE_SEGMENT_NAME}file_sequence_segment_output.txt");
  filenames.addFileName("my_file_group_input", "${FEATURES_INPUT_PATH}/${INPUT_GROUP_NAME}file_group_input.txt");
  filenames.addFileName("my_file_group_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_GROUP_NAME}file_group_output.txt");
  filenames.setTagValue(Filenames::Tag::MAIN_DIR, "/main");
  filenames.setTagValue(Filenames::Tag::MZML_INPUT_PATH, "/mzml");
  filenames.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, "/feat_input");
  filenames.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, "/feat_output");
  filenames.setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, "variant_mzml_");
  filenames.setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, "variant_input_injection_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, "variant_output_injection_");
  filenames.setTagValue(Filenames::Tag::INPUT_SEQUENCE_SEGMENT_NAME, "variant_input_sequence_segment_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_SEQUENCE_SEGMENT_NAME, "variant_output_segment_segment_");
  filenames.setTagValue(Filenames::Tag::INPUT_GROUP_NAME, "variant_input_sample_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, "variant_output_sample_");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/main/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_input").generic_string().c_str(), "/feat_input/variant_input_injection_file_injection_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_output").generic_string().c_str(), "/feat_output/variant_output_injection_file_injection_output.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_sequence_segment_input").generic_string().c_str(), "/feat_input/variant_input_sequence_segment_file_sequence_segment_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_sequence_segment_output").generic_string().c_str(), "/feat_output/variant_output_segment_segment_file_sequence_segment_output.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_input").generic_string().c_str(), "/feat_input/variant_input_sample_file_group_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_output").generic_string().c_str(), "/feat_output/variant_output_sample_file_group_output.txt");
}

TEST(Filenames, setRootPath_setVariant_before)
{
  Filenames filenames;
  filenames.setTagValue(Filenames::Tag::MAIN_DIR, "/main");
  filenames.setTagValue(Filenames::Tag::MZML_INPUT_PATH, "/mzml");
  filenames.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, "/feat_input");
  filenames.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, "/feat_output");
  filenames.setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, "variant_mzml_");
  filenames.setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, "variant_input_injection_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, "variant_output_injection_");
  filenames.setTagValue(Filenames::Tag::INPUT_GROUP_NAME, "variant_input_sample_");
  filenames.setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, "variant_output_sample_");
  filenames.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames.addFileName("my_file_injection_input", "${FEATURES_INPUT_PATH}/${INPUT_INJECTION_NAME}file_injection_input.txt");
  filenames.addFileName("my_file_injection_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}file_injection_output.txt");
  filenames.addFileName("my_file_group_input", "${FEATURES_INPUT_PATH}/${INPUT_GROUP_NAME}file_group_input.txt");
  filenames.addFileName("my_file_group_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_GROUP_NAME}file_group_output.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/main/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_input").generic_string().c_str(), "/feat_input/variant_input_injection_file_injection_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_output").generic_string().c_str(), "/feat_output/variant_output_injection_file_injection_output.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_input").generic_string().c_str(), "/feat_input/variant_input_sample_file_group_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_output").generic_string().c_str(), "/feat_output/variant_output_sample_file_group_output.txt");
}

TEST(Filenames, merge)
{
  Filenames filenames1;
  filenames1.setTagValue(Filenames::Tag::MAIN_DIR, "/main");
  filenames1.setTagValue(Filenames::Tag::MZML_INPUT_PATH, "/mzml");
  filenames1.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, "/feat_input");
  filenames1.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, "/feat_output");
  filenames1.setTagValue(Filenames::Tag::INPUT_MZML_FILENAME, "variant_mzml_");
  filenames1.setTagValue(Filenames::Tag::INPUT_INJECTION_NAME, "variant_input_injection_");
  filenames1.setTagValue(Filenames::Tag::OUTPUT_INJECTION_NAME, "variant_output_injection_");
  filenames1.setTagValue(Filenames::Tag::INPUT_GROUP_NAME, "variant_input_sample_");
  filenames1.setTagValue(Filenames::Tag::OUTPUT_GROUP_NAME, "variant_output_sample_");
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames1.addFileName("my_file_injection_input", "${FEATURES_INPUT_PATH}/${INPUT_INJECTION_NAME}file_injection_input.txt");

  Filenames filenames2;
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main_again.txt");
  filenames2.addFileName("my_file_injection_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_INJECTION_NAME}file_injection_output.txt");
  filenames2.addFileName("my_file_group_input", "${FEATURES_INPUT_PATH}/${INPUT_GROUP_NAME}file_group_input.txt");
  filenames2.addFileName("my_file_group_output", "${FEATURES_OUTPUT_PATH}/${OUTPUT_GROUP_NAME}file_group_output.txt");

  filenames1.merge(filenames2);
  EXPECT_STREQ(filenames1.getFullPath("my_file_main").generic_string().c_str(), "/main/file_main.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_injection_input").generic_string().c_str(), "/feat_input/variant_input_injection_file_injection_input.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_injection_output").generic_string().c_str(), "/feat_output/variant_output_injection_file_injection_output.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_group_input").generic_string().c_str(), "/feat_input/variant_input_sample_file_group_input.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_group_output").generic_string().c_str(), "/feat_output/variant_output_sample_file_group_output.txt");
}

TEST(Filenames, getNamePattern)
{
  Filenames filenames1;
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  EXPECT_EQ(filenames1.getNamePattern("my_file_main"), "${MAIN_DIR}/file_main.txt");
  EXPECT_EQ(filenames1.getNamePattern("non_existing_id"), "");
}

TEST(Filenames, isEmbeddable)
{
  Filenames filenames1;
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames1.addFileName("my_file_main_2", "${MAIN_DIR}/file_main_2.txt", "description", true);
  EXPECT_FALSE(filenames1.isEmbeddable("my_file_main"));
  EXPECT_TRUE(filenames1.isEmbeddable("my_file_main_2"));
}

TEST(Filenames, isEmbedded)
{
  Filenames filenames1;
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames1.addFileName("my_file_main_2", "${MAIN_DIR}/file_main_2.txt", "description", true, true);
  EXPECT_FALSE(filenames1.isEmbedded("my_file_main"));
  EXPECT_TRUE(filenames1.isEmbedded("my_file_main_2"));
}

TEST(Filenames, setEmbedded)
{
  Filenames filenames1;
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  EXPECT_FALSE(filenames1.isEmbedded("my_file_main"));
  filenames1.setEmbedded("my_file_main", true);
  EXPECT_TRUE(filenames1.isEmbedded("my_file_main"));
  filenames1.setEmbedded("my_file_main", false);
  EXPECT_FALSE(filenames1.isEmbedded("my_file_main"));
}

TEST(Filenames, getDescription)
{
  Filenames filenames1;
  filenames1.addFileName("my_file_main", "${MAIN_DIR}/file_main.txt");
  filenames1.addFileName("my_file_main_2", "${MAIN_DIR}/file_main_2.txt", "description");
  EXPECT_EQ(filenames1.getDescription("my_file_main"), "");
  EXPECT_EQ(filenames1.getDescription("my_file_main_2"), "description");
}

TEST(Filenames, getTag)
{
  Filenames filenames1;
  EXPECT_EQ(filenames1.getTagValue(Filenames::Tag::MAIN_DIR), "");
  EXPECT_EQ(filenames1.getTagValue(Filenames::Tag::MZML_INPUT_PATH), "");
  filenames1.setTagValue(Filenames::Tag::MAIN_DIR, "/main");
  filenames1.setTagValue(Filenames::Tag::MZML_INPUT_PATH, "/mzml");
  EXPECT_EQ(filenames1.getTagValue(Filenames::Tag::MAIN_DIR), "/main");
  EXPECT_EQ(filenames1.getTagValue(Filenames::Tag::MZML_INPUT_PATH), "/mzml");
}
