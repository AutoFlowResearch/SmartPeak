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

#define BOOST_TEST_MODULE MetaDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(metadatahandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  MetaDataHandler* ptr = nullptr;
  MetaDataHandler* nullPointer = nullptr;
  ptr = new MetaDataHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
  delete ptr;
}

BOOST_AUTO_TEST_CASE(getters_setters)
{
  MetaDataHandler metaDataHandler;

  // test defaults
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "");
  BOOST_CHECK_EQUAL(metaDataHandler.getAcquisitionDateAndTimeAsString(), "1900-01-01_000000");
  BOOST_CHECK_EQUAL(metaDataHandler.getInjectionName(), "_-1__1900-01-01_000000");

  // test setters
  metaDataHandler.setSampleName("this is a sample name");
  metaDataHandler.setSampleGroupName("this is a sample GROUP name");
  metaDataHandler.setSampleType(SampleType::QC);
  metaDataHandler.setSequenceSegmentName("this is a SEQUENCE segment name");
  metaDataHandler.setFilename("some filename");
  metaDataHandler.batch_name = "SomeBatchName";
  metaDataHandler.setAcquisitionDateAndTimeFromString("09-06-2015  17:14:00", "%m-%d-%Y %H:%M:%S");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleName(), "this is a sample name");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleGroupName(), "this is a sample GROUP name");
  BOOST_CHECK_EQUAL(metaDataHandler.getSampleType() == SampleType::QC, true);
  BOOST_CHECK_EQUAL(metaDataHandler.getSequenceSegmentName(), "this is a SEQUENCE segment name");
  BOOST_CHECK_EQUAL(metaDataHandler.getFilename(), "some filename");
  BOOST_CHECK_EQUAL(metaDataHandler.getAcquisitionDateAndTimeAsString(), "2015-09-06_171400");
  BOOST_CHECK_EQUAL(metaDataHandler.getInjectionName(), "this is a sample name_-1_SomeBatchName_2015-09-06_171400");
}

BOOST_AUTO_TEST_CASE(validateMetaData)
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
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);

  m.setSampleType(SampleType::Unknown);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(SampleType::Standard);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(SampleType::QC);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(SampleType::Blank);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(SampleType::DoubleBlank);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);
  m.setSampleType(SampleType::Solvent);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), true);

  m.setSampleType(SampleType::Unrecognized);
  BOOST_CHECK_EQUAL(MetaDataHandler::validateMetaData(m), false);
}

BOOST_AUTO_TEST_CASE(clear)
{
  MetaDataHandler m;

  m.setSampleName("1");
  m.setSampleGroupName("2");
  m.setSampleType(SampleType::QC);
  m.setSequenceSegmentName("4");
  m.setFilename("5");
  m.acq_method_name = "6";
  m.inj_volume = 7.0;
  m.inj_volume_units = "8";
  m.batch_name = "9";
  m.acquisition_date_and_time = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  m.dilution_factor = 10;
  m.scan_polarity = "negative";
  m.scan_mass_high = 1000;
  m.scan_mass_low = 100;

  m.clear();

  BOOST_CHECK_EQUAL(m.getSampleName(), "");
  BOOST_CHECK_EQUAL(m.getSampleGroupName(), "");
  BOOST_CHECK_EQUAL((int)m.getSampleType(), (int)SampleType::Unknown);
  BOOST_CHECK_EQUAL(m.getSequenceSegmentName(), "");
  BOOST_CHECK_EQUAL(m.getFilename(), "");
  BOOST_CHECK_EQUAL(m.acq_method_name, "");
  BOOST_CHECK_EQUAL(m.inj_volume, -1.0);
  BOOST_CHECK_EQUAL(m.inj_volume_units, "");
  BOOST_CHECK_EQUAL(m.batch_name, "");
  BOOST_CHECK_EQUAL(m.getAcquisitionDateAndTimeAsString(), "1900-01-01_000000");
  BOOST_CHECK_CLOSE(m.dilution_factor, 1.0, 1e-3);
  BOOST_CHECK_EQUAL(m.scan_polarity, "Unknown");
  BOOST_CHECK_CLOSE(m.scan_mass_high, 1e12, 1e-3);
  BOOST_CHECK_CLOSE(m.scan_mass_low, 0.0, 1e-3);
}

BOOST_AUTO_TEST_CASE(SampleTypeToString)
{
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::Unknown), "Unknown");
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::Standard), "Standard");
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::QC), "QC");
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::Blank), "Blank");
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::DoubleBlank), "Double Blank");
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::Solvent), "Solvent");
  BOOST_CHECK_EQUAL(sampleTypeToString.at(SampleType::Unrecognized), "Unrecognized");
  BOOST_CHECK_EQUAL(sampleTypeToString.size(), 7);
}

BOOST_AUTO_TEST_CASE(getInjectionName)
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

  BOOST_CHECK_EQUAL(m.getInjectionName(), "SampleName_14_BatchName_2019-01-31_154055");
}

BOOST_AUTO_TEST_SUITE_END()
