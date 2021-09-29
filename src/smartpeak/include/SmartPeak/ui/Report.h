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

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <array>
#include <set>
#include <string>
#include <vector>

namespace SmartPeak
{
  class Report final : public Widget
  {
    std::array<bool, SampleTypeSize>      st_checks_;
    std::array<bool, FeatureMetadataSize> md_checks_;
    bool                                  all_st_checks_;
    bool                                  all_st_deactivated_;
    bool                                  all_md_checks_;
    bool                                  all_md_deactivated_;
    std::set<SampleType>                  summarySampleTypes_;
    std::vector<FeatureMetadata>          summaryMetaData_;
    ApplicationHandler&                   application_handler_;

    bool initializeMetadataAndSampleTypes();

    static void run_and_join(
      bool (*data_writer)(const SequenceHandler&, const std::filesystem::path&, const std::vector<FeatureMetadata>&, const std::set<SampleType>&),
      const std::string& data_writer_label,
      const SequenceHandler sequence,
      const std::filesystem::path& pathname,
      const std::vector<FeatureMetadata>& meta_data,
      const std::set<SampleType>& sample_types
    );

  public:
    Report(ApplicationHandler& application_handler);

    void draw() override;
  };
}
