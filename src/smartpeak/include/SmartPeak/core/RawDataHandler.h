/**TODO:  Add copyright*/

#ifndef SMARTPEAK_RAWDATAHANDLER_H
#define SMARTPEAK_RAWDATAHANDLER_H

#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureQC.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitationMethod.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>

#include <map>
#include <vector>

namespace SmartPeak
{

  class RawDataHandler
  {
public:    
    RawDataHandler();  ///< Default constructor
    ~RawDataHandler();  ///< Default destructor    
    
    void setFeatureMap(const OpenMS::FeatureMap& featureMap_I);
    OpenMS::FeatureMap getFeatureMap() const;

    void setMetaData(const std::map<std::string, std::string>& meta_data_I);
    std::map<std::string, std::string> getMetaData() const;

    void setParameters(
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_I);
    std::map<std::string, std::vector<std::map<std::string, std::string>>> getParameters() const;

    void setTargeted(const OpenMS::TargetedExperiment& targeted_I);      
    OpenMS::TargetedExperiment getTargeted() const;

    // setReferenceData(, reference_data_I);      
    // getReferenceData();

    void setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods_I);      
    std::vector<OpenMS::AbsoluteQuantitationMethod> getQuantitationMethods() const;

    void setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter_I);      
    OpenMS::MRMFeatureQC getFeatureFilter() const;

    void setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc_I);      
    OpenMS::MRMFeatureQC getFeatureQC() const;
      
    std::vector<OpenMS::FeatureMap> getFeatureMapHistory() const;

private:    
    // input
    OpenMS::MSExperiment msExperiment_;
    OpenMS::MSExperiment chromatogram_map_;
    OpenMS::TransformationDescription trafo_;
    OpenMS::MSExperiment swath_;

    // output
    OpenMS::FeatureMap featureMap_;
    std::map<std::string, std::string> meta_data_;
    std::map<std::string, float> validation_metrics_;
    std::vector<OpenMS::FeatureMap> featureMapHistory_;

    // input (reused between RawDataHandlers)
    std::map<std::string, std::vector<std::map<std::string, std::string>>> parameters_;
    OpenMS::TargetedExperiment targeted_;
    // self.reference_data = None
    std::vector<OpenMS::AbsoluteQuantitationMethod> quantitation_methods_;
    OpenMS::MRMFeatureQC feature_filter_;
    OpenMS::MRMFeatureQC feature_qc_;
  };
}

#endif //SMARTPEAK_RAWDATAHANDLER_H