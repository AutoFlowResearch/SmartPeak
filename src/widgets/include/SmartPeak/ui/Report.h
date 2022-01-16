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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/FilePicker.h>
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
      const std::set<SampleType>& sample_types,
      std::optional<std::string>& result_message
    );

  public:
    Report(ApplicationHandler& application_handler);

    void draw() override;

  protected:

    void drawResultMessage();

    std::optional<std::string> result_message_;

    struct ReportFilePickerHandler : IFilePickerHandler
    {
      typedef  bool (*WriterMethod)(const SequenceHandler&, 
                                       const std::filesystem::path&,
                                       const std::vector<FeatureMetadata>&,
                                       const std::set<SampleType>&);
      ReportFilePickerHandler(Report& report, const std::string title, WriterMethod writer_method) :
        report_(report), title_(title), writer_method_(writer_method) { };
      /**
      IFilePickerHandler
      */
      bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;
    protected:
      Report& report_;
      const std::string title_;
      WriterMethod writer_method_;
    };

    std::shared_ptr<ReportFilePickerHandler> feature_db_file_picker_handler_;
    std::shared_ptr<ReportFilePickerHandler> pivot_table_file_picker_handler_;
    std::shared_ptr<ReportFilePickerHandler> group_feature_db_file_picker_handler_;
    std::shared_ptr<ReportFilePickerHandler> group_pivot_table_file_picker_handler_;
    FilePicker report_file_picker_;

    void reportButton(const std::string& title,
                      std::shared_ptr<ReportFilePickerHandler> report_file_picker_handler,
                      const std::string& default_file_name);
  };
}
