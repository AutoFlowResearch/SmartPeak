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

#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_HPLC_UV_Standards(
  const std::string& dir_I,
  const Filenames& filenames,
  const std::string& delimiter_I = ","
)
{
  SequenceHandler sequenceHandler;

  CreateSequence cs(sequenceHandler);
  cs.filenames_        = filenames;
  cs.delimiter        = delimiter_I;
  cs.checkConsistency = true;
  cs.process();

  std::vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = {
    std::make_shared<LoadRawData>(),
    std::make_shared<MapChromatograms>(),
    std::make_shared<ExtractChromatogramWindows>(),
    std::make_shared<ZeroChromatogramBaseline>(),
    std::make_shared<PickMRMFeatures>(),
    std::make_shared<FilterFeatures>(),
    std::make_shared<SelectFeatures>(),
    std::make_shared<CheckFeatures>(),
    std::make_shared<StoreFeatures>()
  };

  Filenames methods_filenames;
  methods_filenames.setRootPaths(dir_I,
    dir_I + "/mzML/",
    dir_I + "/features/",
    dir_I + "/features/");
  std::map<std::string, Filenames> dynamic_filenames1;
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames1[key] = methods_filenames;
    dynamic_filenames1[key].setFileVariants(
      injection.getMetaData().getFilename(),
      key,
      key,
      injection.getMetaData().getSampleGroupName(),
      injection.getMetaData().getSampleGroupName()
    );
  }

  ProcessSequence ps(sequenceHandler);
  ps.filenames_                     = dynamic_filenames1;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process();

  const std::vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods = {
    std::make_shared<CalculateCalibration>(),
    std::make_shared<StoreQuantitationMethods>()
  };

  std::map<std::string, Filenames> dynamic_filenames2;
  for (const SequenceSegmentHandler& sequence_segment : sequenceHandler.getSequenceSegments()) {
    const std::string& key = sequence_segment.getSequenceSegmentName();
    dynamic_filenames2[key] = methods_filenames;
    dynamic_filenames2[key].setFileVariants(
      "",
      key,
      key,
      "",
      ""
    );
  }

  ProcessSequenceSegments pss(sequenceHandler);
  pss.filenames_                             = dynamic_filenames2;
  pss.sequence_segment_processing_methods_ = sequence_segment_processing_methods;
  pss.process();

  raw_data_processing_methods = {
    std::make_shared<QuantifyFeatures>(),
    std::make_shared<CheckFeatures>(),
    std::make_shared<StoreFeatures>()
  };

  std::map<std::string, Filenames> dynamic_filenames3;
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string& key = injection.getMetaData().getInjectionName();
    dynamic_filenames3[key] = methods_filenames;
    dynamic_filenames3[key].setFileVariants(
      injection.getMetaData().getFilename(),
      key,
      key,
      injection.getMetaData().getSampleGroupName(),
      injection.getMetaData().getSampleGroupName()
    );
  }

  ps.filenames_                     = dynamic_filenames3;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process();

  SequenceParser::writeDataMatrixFromMetaValue(
    sequenceHandler,
    filenames.getFullPathName("pivotTable_csv_o"),
    {FeatureMetadata::calculated_concentration},
    {SampleType::Standard}
  );

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    filenames.getFullPathName("featureDB_csv_o"),
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
