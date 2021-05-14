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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/SampleType.h>

#include <string>
#include <ctime>

namespace SmartPeak {
  class MetaDataHandler {
public:
    void setSampleName(const std::string& sample_name_I);
    std::string& getSampleName();
    const std::string& getSampleName() const;

    void setSampleGroupName(const std::string& sample_group_name_I);
    std::string& getSampleGroupName();
    const std::string& getSampleGroupName() const;

    void setSequenceSegmentName(const std::string& sequence_segment_name_I);
    std::string& getSequenceSegmentName();
    const std::string& getSequenceSegmentName() const;

    void setReplicateGroupName(const std::string& replicate_group_name_I);
    std::string& getReplicateGroupName();
    const std::string& getReplicateGroupName() const;

    void setFilename(const std::string& filename_I);
    std::string& getFilename();
    const std::string& getFilename() const;

    void setSampleType(SampleType sample_type_I);
    SampleType& getSampleType();
    const SampleType& getSampleType() const;
    const std::string getSampleTypeAsString() const;

    static bool validateMetaData(const MetaDataHandler& meta_data);

    void clear();

    std::string getInjectionName() const;

    void setAcquisitionDateAndTimeFromString(
      const std::string& acquisition_date_and_time,
      const std::string& format = "%Y-%m-%d_%H%M%S" // from less specific to more specific.
      // NOTE: Assuming Excel defaults as "%m-%d-%Y %H:%M" and "%m-%d-%Y %H:%M:%S" (locale-dependent)
    );

    std::string getAcquisitionDateAndTimeAsString(const std::string& format = "%Y-%m-%d_%H%M%S") const;

    // required
    std::string acq_method_name;
    float inj_volume = -1.0;
    std::string inj_volume_units;
    std::string batch_name;

    // optional
    int rack_number = -1;
    int plate_number = -1;
    int pos_number = -1;
    int inj_number = -1; // default: row number (starts from 1)
    float dilution_factor = 1.0;
    std::string instrument;
    std::string operator_name;
    std::string proc_method_name;
    std::tm acquisition_date_and_time = { 0, 0, 0, 1, 0, 0, 0, 0, 0 }; // Need to start at Day 1 of the month
    std::string scan_polarity = "Unknown";
    float scan_mass_low = 0.0; // in Da
    float scan_mass_high = 1e12; // in Da

  protected:
    // required
    std::string sample_name;
    std::string sample_group_name;
    std::string sequence_segment_name;
    std::string replicate_group_name;
    std::string original_filename;
    SampleType sample_type = SampleType::Unknown;
  };

}
