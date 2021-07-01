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
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>

using namespace SmartPeak;
using namespace std;

TEST(MetaDataHandler, constructor)
{
  MetaDataHandler* ptr = nullptr;
  MetaDataHandler* nullPointer = nullptr;
  ptr = new MetaDataHandler();
  EXPECT_NE(ptr, nullPointer);
  delete ptr;
}

TEST(MetaDataHandler, getters_setters)
{
  MetaDataHandler metaDataHandler;

  // test defaults
  EXPECT_STREQ(((std::string)metaDataHandler.getSampleName()).c_str(), "");
  EXPECT_STREQ(((std::string)metaDataHandler.getSampleGroupName()).c_str(), "");
  EXPECT_TRUE(metaDataHandler.getSampleType() == SampleType::Unknown);
  EXPECT_STREQ(((std::string)metaDataHandler.getSequenceSegmentName()).c_str(), "");
  EXPECT_STREQ(((std::string)metaDataHandler.getReplicateGroupName()).c_str(), "");
  EXPECT_STREQ(((std::string)metaDataHandler.getFilename()).c_str(), "");
  EXPECT_STREQ(((std::string)metaDataHandler.getAcquisitionDateAndTimeAsString()).c_str(), "1900-01-01_000000");
  EXPECT_STREQ(((std::string)metaDataHandler.getInjectionName()).c_str(), "_-1__1900-01-01_000000");

  // test setters
  metaDataHandler.setSampleName("this is a sample name");
  metaDataHandler.setSampleGroupName("this is a sample GROUP name");
  metaDataHandler.setSampleType(SampleType::QC);
  metaDataHandler.setSequenceSegmentName("this is a SEQUENCE segment name");
  metaDataHandler.setReplicateGroupName("this is a REPLICATE GROUP name");
  metaDataHandler.setFilename("some filename");
  metaDataHandler.batch_name = "SomeBatchName";
  metaDataHandler.setAcquisitionDateAndTimeFromString("09-06-2015  17:14:00", "%m-%d-%Y %H:%M:%S");
  EXPECT_STREQ(((std::string)metaDataHandler.getSampleName()).c_str(), "this is a sample name");
  EXPECT_STREQ(((std::string)metaDataHandler.getSampleGroupName()).c_str(), "this is a sample GROUP name");
  EXPECT_TRUE(metaDataHandler.getSampleType() == SampleType::QC);
  EXPECT_STREQ(((std::string)metaDataHandler.getSequenceSegmentName()).c_str(), "this is a SEQUENCE segment name");
  EXPECT_STREQ(((std::string)metaDataHandler.getReplicateGroupName()).c_str(), "this is a REPLICATE GROUP name");
  EXPECT_STREQ(((std::string)metaDataHandler.getFilename()).c_str(), "some filename");
  EXPECT_STREQ(((std::string)metaDataHandler.getAcquisitionDateAndTimeAsString()).c_str(), "2015-09-06_171400");
  EXPECT_STREQ(((std::string)metaDataHandler.getInjectionName()).c_str(), "this is a sample name_-1_SomeBatchName_2015-09-06_171400");
}

TEST(MetaDataHandler, validateMetaData)
{
  MetaDataHandler m;

  m.setSampleName("1");
  m.setSampleGroupName("2");
  // m.setSampleType(SampleType::Unknown);
  m.setSequenceSegmentName("4");
  m.setFilename("5");
  m.acq_method_name = "6";
  m.inj_volume = 7.0;
  m.inj_volume_units = "8";
  m.batch_name = "9";
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));

  m.setSampleType(SampleType::Unknown);
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));
  m.setSampleType(SampleType::Standard);
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));
  m.setSampleType(SampleType::QC);
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));
  m.setSampleType(SampleType::Blank);
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));
  m.setSampleType(SampleType::DoubleBlank);
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));
  m.setSampleType(SampleType::Solvent);
  EXPECT_TRUE(MetaDataHandler::validateMetaData(m));

  m.setSampleType(SampleType::Unrecognized);
  EXPECT_FALSE(MetaDataHandler::validateMetaData(m));
}

TEST(MetaDataHandler, clear)
{
  MetaDataHandler m;

  m.setSampleName("1");
  m.setSampleGroupName("2");
  m.setSampleType(SampleType::QC);
  m.setSequenceSegmentName("4");
  m.setReplicateGroupName("5");
  m.setFilename("6");
  m.acq_method_name = "7";
  m.inj_volume = 8.0;
  m.inj_volume_units = "9";
  m.batch_name = "10";
  m.acquisition_date_and_time = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  m.dilution_factor = 11;
  m.scan_polarity = "negative";
  m.scan_mass_high = 1000;
  m.scan_mass_low = 100;

  m.clear();

  EXPECT_STREQ(m.getSampleName().c_str(), "");
  EXPECT_STREQ(m.getSampleGroupName().c_str(), "");
  EXPECT_EQ((int)m.getSampleType(), (int)SampleType::Unknown);
  EXPECT_STREQ(m.getSequenceSegmentName().c_str(), "");
  EXPECT_STREQ(m.getReplicateGroupName().c_str(), "");
  EXPECT_STREQ(m.getFilename().c_str(), "");
  EXPECT_STREQ(m.acq_method_name.c_str(), "");
  EXPECT_EQ(m.inj_volume, -1.0);
  EXPECT_STREQ(m.inj_volume_units.c_str(), "");
  EXPECT_STREQ(m.batch_name.c_str(), "");
  EXPECT_STREQ(m.getAcquisitionDateAndTimeAsString().c_str(), "1900-01-01_000000");
  EXPECT_NEAR(m.dilution_factor, 1.0, 1e-3);
  EXPECT_STREQ(m.scan_polarity.c_str(), "Unknown");
  EXPECT_NEAR(m.scan_mass_low, 0.0, 1e-3);
}

TEST(MetaDataHandler, SampleTypeToString)
{
  EXPECT_STREQ(sampleTypeToString.at(SampleType::Unknown).c_str(), "Unknown");
  EXPECT_STREQ(sampleTypeToString.at(SampleType::Standard).c_str(), "Standard");
  EXPECT_STREQ(sampleTypeToString.at(SampleType::QC).c_str(), "QC");
  EXPECT_STREQ(sampleTypeToString.at(SampleType::Blank).c_str(), "Blank");
  EXPECT_STREQ(sampleTypeToString.at(SampleType::DoubleBlank).c_str(), "Double Blank");
  EXPECT_STREQ(sampleTypeToString.at(SampleType::Solvent).c_str(), "Solvent");
  EXPECT_STREQ(sampleTypeToString.at(SampleType::Unrecognized).c_str(), "Unrecognized");
  EXPECT_EQ(sampleTypeToString.size(), 7);
}

TEST(MetaDataHandler, getInjectionName)
{
  MetaDataHandler m;

  m.setSampleName("SampleName");
  m.inj_number = 14;
  m.batch_name = "BatchName";
  m.acquisition_date_and_time.tm_sec = 55;
  m.acquisition_date_and_time.tm_min = 40;
  m.acquisition_date_and_time.tm_hour = 15;
  m.acquisition_date_and_time.tm_mday = 31;
  m.acquisition_date_and_time.tm_mon = 0; // months since January
  m.acquisition_date_and_time.tm_year = 119; // years since 1900

  EXPECT_STREQ(m.getInjectionName().c_str(), "SampleName_14_BatchName_2019-01-31_154055");
}

TEST(MetaDataHandler, getRackNumberAsString)
{
  using namespace std::string_literals;
  MetaDataHandler m;
  EXPECT_STREQ(m.getRackNumberAsString().c_str(), "-1");
  EXPECT_STREQ(m.getRackNumberAsString("invalid"s).c_str(), "invalid");
  m.rack_number = 42;
  EXPECT_STREQ(m.getRackNumberAsString().c_str(), "42");
  EXPECT_STREQ(m.getRackNumberAsString("invalid"s).c_str(), "42");
}

TEST(MetaDataHandler, getPlateNumberAsString)
{
  using namespace std::string_literals;
  MetaDataHandler m;
  EXPECT_STREQ(m.getPlateNumberAsString().c_str(), "-1");
  EXPECT_STREQ(m.getPlateNumberAsString("invalid"s).c_str(), "invalid");
  m.plate_number = 42;
  EXPECT_STREQ(m.getPlateNumberAsString().c_str(), "42");
  EXPECT_STREQ(m.getPlateNumberAsString("invalid"s).c_str(), "42");
}

TEST(MetaDataHandler, getPosNumberAsString)
{
  using namespace std::string_literals;
  MetaDataHandler m;
  EXPECT_STREQ(m.getPosNumberAsString().c_str(), "-1");
  EXPECT_STREQ(m.getPosNumberAsString("invalid"s).c_str(), "invalid");
  m.pos_number = 42;
  EXPECT_STREQ(m.getPosNumberAsString().c_str(), "42");
  EXPECT_STREQ(m.getPosNumberAsString("invalid"s).c_str(), "42");
}

TEST(MetaDataHandler, getInjectionNumberAsString)
{
  using namespace std::string_literals;
  MetaDataHandler m;
  EXPECT_STREQ(m.getInjectionNumberAsString().c_str(), "-1");
  EXPECT_STREQ(m.getInjectionNumberAsString("invalid"s).c_str(), "invalid");
  m.inj_number = 42;
  EXPECT_STREQ(m.getInjectionNumberAsString().c_str(), "42");
  EXPECT_STREQ(m.getInjectionNumberAsString("invalid"s).c_str(), "42");
}

TEST(MetaDataHandler, getScanMassLowAsString)
{
  using namespace std::string_literals;
  MetaDataHandler m;
  EXPECT_STREQ(m.getScanMassLowAsString().c_str(), "0.000000");
  EXPECT_STREQ(m.getScanMassLowAsString("invalid"s).c_str(), "invalid");
  m.scan_mass_low = 42;
  EXPECT_STREQ(m.getScanMassLowAsString().c_str(), "42.000000");
  EXPECT_STREQ(m.getScanMassLowAsString("invalid"s).c_str(), "42.000000");
}

TEST(MetaDataHandler, getScanMassHighAsString)
{
  using namespace std::string_literals;
  MetaDataHandler m;
  EXPECT_STREQ(m.getScanMassHighAsString().c_str(), "999999995904.000000");
  EXPECT_STREQ(m.getScanMassHighAsString("invalid"s).c_str(), "invalid");
  m.scan_mass_high = 42;
  EXPECT_STREQ(m.getScanMassHighAsString().c_str(), "42.000000");
  EXPECT_STREQ(m.getScanMassHighAsString("invalid"s).c_str(), "42.000000");
}
