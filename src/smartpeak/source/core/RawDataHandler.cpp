/**TODO:  Add copyright*/

#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  RawDataHandler::RawDataHandler()
  {        
  }

  RawDataHandler::~RawDataHandler()
  {
  }
    
  void RawDataHandler::setFeatureMap(const OpenMS::FeatureMap& featureMap_I)
  {
    featureMap_ = featureMap_I;
  }

  OpenMS::FeatureMap RawDataHandler::getFeatureMap() const
  {
    return featureMap_;
  }

  void RawDataHandler::setMetaData(const std::map<std::string, std::string>& meta_data_I)
  {
    meta_data_ = meta_data_I;
  }

  std::map<std::string, std::string> RawDataHandler::getMetaData() const
  {
    return meta_data_;
  }

  void RawDataHandler::setParameters(
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_I)
  {
    parameters_ = parameters_I;
  }

  std::map<std::string, std::vector<std::map<std::string, std::string>>> RawDataHandler::getParameters() const
  {
    return parameters_;
  }

  void RawDataHandler::setTargeted(const OpenMS::TargetedExperiment& targeted_I)
  {
    targeted_ = targeted_I;
  }
    
  OpenMS::TargetedExperiment RawDataHandler::getTargeted() const
  {
    return targeted_;
  }

  // RawDataHandler::setReferenceData(, reference_data_I)
  // {
  //   """Set the reference_data"""
  //   .reference_data = reference_data_I
  // }
    
  // RawDataHandler::getReferenceData()
  // {
  //   """Return the reference_data"""
  //   return .reference_data
  // }

  void RawDataHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods_I)
  {
    quantitation_methods_ = quantitation_methods_I;
  }
    
  std::vector<OpenMS::AbsoluteQuantitationMethod> RawDataHandler::getQuantitationMethods() const
  {
    return quantitation_methods_;
  }

  void RawDataHandler::setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter_I)
  {
    feature_filter_ = feature_filter_I;
  }
    
  OpenMS::MRMFeatureQC RawDataHandler::getFeatureFilter() const
  {
    return feature_filter_;
  }

  void RawDataHandler::setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc_I)
  {
    feature_qc_ = feature_qc_I;
  }
    
  OpenMS::MRMFeatureQC RawDataHandler::getFeatureQC() const
  {
    return feature_qc_;
  }
    
  std::vector<OpenMS::FeatureMap>  RawDataHandler::getFeatureMapHistory() const
  {
    return featureMapHistory_;
  }
}