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
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

const std::string main_dir = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");

TEST(InputDataValidation, fileExists)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  EXPECT_TRUE(InputDataValidation::fileExists(pathname));
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("this_does_not_exist.csv");
  EXPECT_FALSE(InputDataValidation::fileExists(pathname));
}

TEST(InputDataValidation, isValidFilename)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  InputDataValidation::FilenameInfo v;
  v = InputDataValidation::isValidFilename(pathname, "some standards file");
  EXPECT_EQ(v.validity, InputDataValidation::FilenameInfo::valid);  // success
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("this_does_not_exist.csv");
  v = InputDataValidation::isValidFilename(pathname, "a file that does not exist");
  EXPECT_EQ(v.validity, InputDataValidation::FilenameInfo::invalid); // failure
  pathname.clear();
  v = InputDataValidation::isValidFilename(pathname, "an empty pathname");
  EXPECT_EQ(v.validity, InputDataValidation::FilenameInfo::not_provided);  // not provided
}

TEST(InputDataValidation, validateNamesInStructures)
{
  const set<string> s1 = {"1", "2", "3"};
  const set<string> s2 = {"1", "2", "4"};
  const set<string> s3 = {"1", "2", "3", "4"};
  EXPECT_TRUE(InputDataValidation::validateNamesInStructures(s1, s1, "", "", true)); // check itself
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s1, s2, "", "", false)); // s1 -> s2, "3" is missing
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s2, s1, "", "", false)); // s1 -> s2, "4" is missing
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s2, s1, "", "", true)); // s1 <-> s2, "3" and "4" are missing
  EXPECT_TRUE(InputDataValidation::validateNamesInStructures(s1, s3, "", "", false)); // s1 -> s3, no name is missing
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s1, s3, "", "", true)); // s1 <-> s3, "4" is missing
}

TEST(InputDataValidation, findMissingNames)
{
  const set<string> s1 = {"1", "2", "3"};
  const set<string> s2 = {"1", "2", "4"};
  const set<string> s3 = {"1", "2", "3", "4"};
  set<string> result;

  result = InputDataValidation::findMissingNames(s1, s1);
  EXPECT_EQ(result.size(), 0);

  result = InputDataValidation::findMissingNames(s1, s2);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.count("3"), 1);

  result = InputDataValidation::findMissingNames(s2, s1);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.count("4"), 1);

  result = InputDataValidation::findMissingNames(s1, s3);
  EXPECT_EQ(result.size(), 0);

  result = InputDataValidation::findMissingNames(s3, s1);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.count("4"), 1);
}

TEST(InputDataValidation, sampleNamesAreConsistent)
{
  SequenceHandler sequenceHandler;
  Filenames filenames;

  CreateSequence cs(sequenceHandler);
  cs.filenames_          = filenames;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  filenames.setFullPathName("sequence_csv_i", main_dir + "/sequence_missing.csv");
  sequenceHandler.clear();

  cs.filenames_ = filenames;
  cs.process();

  result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
  EXPECT_FALSE(result); // missing sample: fakeSample
}

TEST(InputDataValidation, componentNamesAreConsistent)
{
  SequenceHandler sequenceHandler;
  Filenames filenames;

  CreateSequence cs(sequenceHandler);
  cs.filenames_          = filenames;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::componentNamesAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  filenames.setFullPathName("traML_csv_i", main_dir + "/traML_missing.csv");
  // SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);
  RawDataHandler& rawData0 = sequenceHandler.getSequence().front().getRawData();
  LoadTransitions loadTransitions;
  loadTransitions.process(rawData0, {}, filenames);

  result = InputDataValidation::componentNamesAreConsistent(sequenceHandler);
  EXPECT_FALSE(result);
}

TEST(InputDataValidation, componentNameGroupsAreConsistent)
{
  SequenceHandler sequenceHandler;
  Filenames filenames;

  CreateSequence cs(sequenceHandler);
  cs.filenames_          = filenames;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  filenames.setFullPathName("traML_csv_i", main_dir + "/traML_missing.csv");
  //SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);
  RawDataHandler& rawData0 = sequenceHandler.getSequence().front().getRawData();
  LoadTransitions loadTransitions;
  loadTransitions.process(rawData0, {}, filenames);

  result = InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler);
  EXPECT_FALSE(result);
}

TEST(InputDataValidation, heavyComponentsAreConsistent)
{
  SequenceHandler sequenceHandler;
  Filenames filenames;

  CreateSequence cs(sequenceHandler);
  cs.filenames_          = filenames;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::heavyComponentsAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  filenames.setFullPathName("quantitationMethods_csv_i", main_dir + "/quantitationMethods_missing.csv");
  //SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);
  SequenceSegmentHandler& seqSeg0 = sequenceHandler.getSequenceSegments().front();
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(seqSeg0, SequenceHandler(), {}, filenames);

  result = InputDataValidation::heavyComponentsAreConsistent(sequenceHandler);
  EXPECT_FALSE(result); // g6p.g6p_2.Heavy is missing
}
