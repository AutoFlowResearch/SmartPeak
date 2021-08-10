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
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

TEST(Filenames, constructor)
{
  Filenames filenames;
  EXPECT_EQ(filenames.getFileNames().size(), 0);
}

TEST(Filenames, addFileName)
{
  Filenames filenames;
  filenames.addFileName("my_file", "file.txt", Filenames::FileScope::EFileScopeMain);
  const auto& filenames_list = filenames.getFileNames();
  ASSERT_EQ(filenames_list.size(), 1);
  const auto& filename = filenames_list.at("my_file");
  EXPECT_STREQ(filename.default_name_.c_str(), "file.txt");
  EXPECT_EQ(filename.file_scope_, Filenames::FileScope::EFileScopeMain);
  EXPECT_STREQ(filename.file_variant_.c_str(), "");
  EXPECT_STREQ(filename.root_path_.generic_string().c_str(), "");
  EXPECT_STREQ(filename.full_path_.generic_string().c_str(), "file.txt");
  EXPECT_FALSE(filename.full_path_override_);
}

TEST(Filenames, getFullPath)
{
  Filenames filenames;
  filenames.addFileName("my_file_main", "file_main.txt", Filenames::FileScope::EFileScopeMain);
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "file_main.txt");
}

TEST(Filenames, getFullPath_non_existing)
{
  Filenames filenames;
  try {
    filenames.getFullPath("test");
    FAIL() << "Expected std::out_of_range";
  }
  catch (std::out_of_range const& err) {
  }
  catch (...) {
    FAIL() << "Expected std::out_of_range";
  }
}

TEST(Filenames, setFullPath)
{
  Filenames filenames;
  filenames.addFileName("my_file_main", "file_main.txt", Filenames::FileScope::EFileScopeMain);
  filenames.setFullPath("my_file_main", "/file/to/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/file/to/file_main.txt");
  // setting variant or root has no effect
  filenames.setRootPaths("/main", "/mzml", "/feat_input", "/feat_output");
  filenames.setFileVariants( "variant_mzml_", "variant_input_injection_", "variant_output_injection_", "variant_input_sample_", "variant_output_sample_");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/file/to/file_main.txt");
}

TEST(Filenames, setRootPath_setVariant_after)
{
  Filenames filenames;
  filenames.addFileName("my_file_main", "file_main.txt", Filenames::FileScope::EFileScopeMain);
  filenames.addFileName("my_file_injection_input", "file_injection_input.txt", Filenames::FileScope::EFileScopeInjectionInput);
  filenames.addFileName("my_file_injection_output", "file_injection_output.txt", Filenames::FileScope::EFileScopeInjectionOutput);
  filenames.addFileName("my_file_group_input", "file_group_input.txt", Filenames::FileScope::EFileScopeSampleGroupInput);
  filenames.addFileName("my_file_group_output", "file_group_output.txt", Filenames::FileScope::EFileScopeSampleGroupOutput);
  filenames.setRootPaths("/main", "/mzml", "/feat_input", "/feat_output");
  filenames.setFileVariants("variant_mzml_", "variant_input_injection_", "variant_output_injection_", "variant_input_sample_", "variant_output_sample_");
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/main/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_input").generic_string().c_str(), "/feat_input/variant_input_injection_file_injection_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_output").generic_string().c_str(), "/feat_output/variant_output_injection_file_injection_output.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_input").generic_string().c_str(), "/feat_input/variant_input_sample_file_group_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_output").generic_string().c_str(), "/feat_output/variant_output_sample_file_group_output.txt");
}

TEST(Filenames, setRootPath_setVariant_before)
{
  Filenames filenames;
  filenames.setRootPaths("/main", "/mzml", "/feat_input", "/feat_output");
  filenames.setFileVariants("variant_mzml_", "variant_input_injection_", "variant_output_injection_", "variant_input_sample_", "variant_output_sample_");
  filenames.addFileName("my_file_main", "file_main.txt", Filenames::FileScope::EFileScopeMain);
  filenames.addFileName("my_file_injection_input", "file_injection_input.txt", Filenames::FileScope::EFileScopeInjectionInput);
  filenames.addFileName("my_file_injection_output", "file_injection_output.txt", Filenames::FileScope::EFileScopeInjectionOutput);
  filenames.addFileName("my_file_group_input", "file_group_input.txt", Filenames::FileScope::EFileScopeSampleGroupInput);
  filenames.addFileName("my_file_group_output", "file_group_output.txt", Filenames::FileScope::EFileScopeSampleGroupOutput);
  EXPECT_STREQ(filenames.getFullPath("my_file_main").generic_string().c_str(), "/main/file_main.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_input").generic_string().c_str(), "/feat_input/variant_input_injection_file_injection_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_injection_output").generic_string().c_str(), "/feat_output/variant_output_injection_file_injection_output.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_input").generic_string().c_str(), "/feat_input/variant_input_sample_file_group_input.txt");
  EXPECT_STREQ(filenames.getFullPath("my_file_group_output").generic_string().c_str(), "/feat_output/variant_output_sample_file_group_output.txt");
}

TEST(Filenames, merge)
{
  Filenames filenames1;
  filenames1.setRootPaths("/main", "/mzml", "/feat_input", "/feat_output");
  filenames1.setFileVariants("variant_mzml_", "variant_input_injection_", "variant_output_injection_", "variant_input_sample_", "variant_output_sample_");
  filenames1.addFileName("my_file_main", "file_main.txt", Filenames::FileScope::EFileScopeMain);
  filenames1.addFileName("my_file_injection_input", "file_injection_input.txt", Filenames::FileScope::EFileScopeInjectionInput);

  Filenames filenames2;
  filenames1.addFileName("my_file_main", "file_main_again.txt", Filenames::FileScope::EFileScopeMain);
  filenames2.addFileName("my_file_injection_output", "file_injection_output.txt", Filenames::FileScope::EFileScopeInjectionOutput);
  filenames2.addFileName("my_file_group_input", "file_group_input.txt", Filenames::FileScope::EFileScopeSampleGroupInput);
  filenames2.addFileName("my_file_group_output", "file_group_output.txt", Filenames::FileScope::EFileScopeSampleGroupOutput);

  filenames1.merge(filenames2);
  EXPECT_STREQ(filenames1.getFullPath("my_file_main").generic_string().c_str(), "/main/file_main.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_injection_input").generic_string().c_str(), "/feat_input/variant_input_injection_file_injection_input.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_injection_output").generic_string().c_str(), "/feat_output/variant_output_injection_file_injection_output.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_group_input").generic_string().c_str(), "/feat_input/variant_input_sample_file_group_input.txt");
  EXPECT_STREQ(filenames1.getFullPath("my_file_group_output").generic_string().c_str(), "/feat_output/variant_output_sample_file_group_output.txt");
}
