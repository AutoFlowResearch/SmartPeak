// TODO: Add copyright

#pragma once

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleGroupHandler.h>
#include <SmartPeak/core/SampleHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  class SequenceHandler
  {
public:
    struct Filenames
    {
      std::string sequence_csv_i;
      std::string parameters_csv_i;
      std::string traML_csv_i;
      std::string featureFilterComponents_csv_i;
      std::string featureFilterComponentGroups_csv_i;
      std::string featureQCComponents_csv_i;
      std::string featureQCComponentGroups_csv_i;
      std::string quantitationMethods_csv_i;
      std::string standardsConcentrations_csv_i;
      std::string referenceData_csv_i;
      std::string mzML_i;
      std::string featureXML_o;
      std::string feature_csv_o;
      std::string featureXML_i;
      std::string features_pdf_o;
      std::string calibrators_pdf_o;
      std::string quantitationMethods_csv_o;
      std::string componentsToConcentrations_csv_o;
      std::string sequenceSummary_csv_o;
      std::string featureSummary_csv_o;

      static Filenames getDefaultStaticFilenames(
        const std::string& dir
      );

      static Filenames getDefaultDynamicFilenames(
        const std::string& dir,
        const std::string& sample_name
      );

      void clear();
    };

    SequenceHandler() = default;
    ~SequenceHandler() = default;
    SequenceHandler(const SequenceHandler&) = default;
    SequenceHandler& operator=(const SequenceHandler&) = default;
    SequenceHandler(SequenceHandler&&) = default;
    SequenceHandler& operator=(SequenceHandler&&) = default;

    void clear();

    void setDirDynamic(const std::string& dir_dynamic);
    std::string& getDirDynamic();
    std::string getDirDynamic() const;

    void setSequence(const std::vector<SampleHandler>& sequence);
    std::vector<SampleHandler>& getSequence();
    std::vector<SampleHandler> getSequence() const;

    void setSequenceSegments(const std::vector<SequenceSegmentHandler>& sequence_segments);
    std::vector<SequenceSegmentHandler>& getSequenceSegments();
    std::vector<SequenceSegmentHandler> getSequenceSegments() const;

    void setSampleGroups(const std::vector<SampleGroupHandler>& sample_groups);
    std::vector<SampleGroupHandler>& getSampleGroups();
    std::vector<SampleGroupHandler> getSampleGroups() const;

    static std::map<std::string, std::string> getDefaultStaticFilenames(const std::string& dir);

    static std::map<std::string, std::string> getDefaultDynamicFilenames(
      const std::string& dir,
      const std::string& sample_name
    );

    void addSampleToSequence(
      const MetaDataHandler& meta_data_I,
      const OpenMS::FeatureMap& featureMap_I
    );

    std::vector<SampleHandler> getSamplesInSequence(
      const std::vector<std::string>& sample_names
    ) const;

    static Utilities::CastValue getMetaValue(
      const OpenMS::Feature& feature,
      const OpenMS::Feature& subordinate,
      const std::string& meta_value
    );

    std::map<size_t, std::string> index_to_sample_;
    std::map<std::string, size_t> sample_to_index_;
private:
    std::vector<SampleHandler> sequence_;
    std::vector<SequenceSegmentHandler> sequence_segments_;
    std::vector<SampleGroupHandler> sample_groups_;

    std::string dir_dynamic_;
  };
}
