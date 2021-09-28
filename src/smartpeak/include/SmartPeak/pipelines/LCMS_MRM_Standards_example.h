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

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Standards(
  const std::string& dir_I,
  Filenames& filenames_I,
  const std::string& delimiter_I = ","
)
{
  ApplicationHandler application_handler;
  LoadSession cs(application_handler);
  auto& sequenceHandler = application_handler.sequenceHandler_;
  cs.filenames_          = filenames_I;
  cs.delimiter          = delimiter_I;
  cs.checkConsistency   = true;
  cs.process();

  std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::make_shared<LoadRawData>(),
    std::make_shared<MapChromatograms>(),
    std::make_shared<PickMRMFeatures>(),
    std::make_shared<FilterFeatures>(),
    std::make_shared<FilterFeatures>(),
    std::make_shared<SelectFeatures>(),
    std::make_shared<CheckFeatures>(),
    std::make_shared<StoreFeatures>()
  };

  Filenames methods_filenames;
  methods_filenames.setTag(Filenames::Tag::MAIN_DIR, dir_I);
  methods_filenames.setTag(Filenames::Tag::MZML_INPUT_PATH, dir_I + "/mzML/");
  methods_filenames.setTag(Filenames::Tag::FEATURES_INPUT_PATH, dir_I + "/features/");
  methods_filenames.setTag(Filenames::Tag::FEATURES_OUTPUT_PATH, dir_I + "/features/");
  std::map<std::string, Filenames> dynamic_filenames1;
  for (const InjectionHandler& injection : application_handler.sequenceHandler_.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames1[key] = methods_filenames;
    dynamic_filenames1[key].setTag(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
    dynamic_filenames1[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
    dynamic_filenames1[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
    dynamic_filenames1[key].setTag(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
    dynamic_filenames1[key].setTag(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
  }

  ProcessSequence ps(application_handler.sequenceHandler_);
  ps.filenames_                     = dynamic_filenames1;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process(methods_filenames);

  const std::vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods = {
    std::make_shared<CalculateCalibration>(),
    std::make_shared<StoreQuantitationMethods>()
  };

  std::map<std::string, Filenames> dynamic_filenames2;
  for (const SequenceSegmentHandler& sequence_segment : application_handler.sequenceHandler_.getSequenceSegments()) {
    const std::string& key = sequence_segment.getSequenceSegmentName();
    dynamic_filenames2[key] = methods_filenames;
    dynamic_filenames2[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
    dynamic_filenames2[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
  }

  ProcessSequenceSegments pss(application_handler.sequenceHandler_);
  pss.filenames_                             = dynamic_filenames2;
  pss.sequence_segment_processing_methods_ = sequence_segment_processing_methods;
  pss.process(methods_filenames);

  raw_data_processing_methods = {
    std::make_shared<QuantifyFeatures>(),
    std::make_shared<CheckFeatures>(),
    std::make_shared<StoreFeatures>()
  };

  std::map<std::string, Filenames> dynamic_filenames3;
  for (const InjectionHandler& injection : application_handler.sequenceHandler_.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames3[key] = methods_filenames;
    dynamic_filenames3[key].setTag(Filenames::Tag::INPUT_MZML_FILENAME, injection.getMetaData().getFilename());
    dynamic_filenames3[key].setTag(Filenames::Tag::INPUT_INJECTION_NAME, key);
    dynamic_filenames3[key].setTag(Filenames::Tag::OUTPUT_INJECTION_NAME, key);
    dynamic_filenames3[key].setTag(Filenames::Tag::INPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
    dynamic_filenames3[key].setTag(Filenames::Tag::OUTPUT_GROUP_NAME, injection.getMetaData().getSampleGroupName());
  }

  ps.filenames_                     = dynamic_filenames3;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process(methods_filenames);

  Filenames filenames = filenames_I;
  filenames.setFullPath("pivotTable", dir_I + "/PivotTable.csv");
  filenames.setFullPath("featureDB", dir_I + "/FeatureDB.csv");

  SequenceParser::writeDataMatrixFromMetaValue(
    sequenceHandler,
    filenames.getFullPath("pivotTable"),
    {FeatureMetadata::calculated_concentration},
    {SampleType::Standard}
  );

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    filenames.getFullPath("featureDB"),
    {
      FeatureMetadata::peak_apex_intensity,
      FeatureMetadata::total_width,
      FeatureMetadata::width_at_50_peak_height,
      FeatureMetadata::tailing_factor,
      FeatureMetadata::asymmetry_factor,
      FeatureMetadata::baseline_delta_to_height,
      FeatureMetadata::points_across_baseline,
      FeatureMetadata::points_across_half_height,
      FeatureMetadata::log_signal_to_noise,
      FeatureMetadata::calculated_concentration,
      FeatureMetadata::qc_transition_message,
      FeatureMetadata::qc_transition_pass,
      FeatureMetadata::qc_transition_score,
      FeatureMetadata::qc_transition_group_message,
      FeatureMetadata::qc_transition_group_score
    },
    {SampleType::Standard}
  );
}
