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
    """Set the featureMap"""
    featureMap_ = featureMap_I;
  }

  OpenMS::FeatureMap RawDataHandler::getFeatureMap() const
  {
    """Return the featureMap"""
    return featureMap_;
  }

  void RawDataHandler::setMetaData(const std::map<std::string, std::string>& meta_data_I)
  {
    """Set the meta_data"""
    meta_data_ = meta_data_I;
  }

  std::map<std::string, std::string> RawDataHandler::getMetaData() const
  {
    """Return the meta_data"""
    return meta_data_;
  }

  void RawDataHandler::setParameters(
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters_I)
  {
    """Set the parameters"""
    parameters_ = parameters_I;
  }

  std::map<std::string, std::vector<std::map<std::string, std::string>>> RawDataHandler::getParameters() const
  {
    """Return the parameters"""
    return parameters_;
  }

  void RawDataHandler::setTargeted(const OpenMS::TargetedExperiment& targeted_I)
  {
    """Set the targeted"""
    targeted_ = targeted_I;
  }
    
  OpenMS::TargetedExperiment RawDataHandler::getTargeted() const
  {
    """Return the targeted"""
    return targeted_  
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
    """Set the quantitation_methods"""
    quantitation_methods_ = quantitation_methods_I;
  }
    
  std::vector<OpenMS::AbsoluteQuantitationMethod> RawDataHandler::getQuantitationMethods() const
  {
    """Return the quantitation_methods"""
    return quantitation_methods_;
  }

  void RawDataHandler::setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter_I)
  {
    """Set the feature_filter"""
    feature_filter_ = feature_filter_I;
  }
    
  OpenMS::MRMFeatureQC RawDataHandler::getFeatureFilter() const
  {
    """Return the feature_filter"""
    return feature_filter_;
  }

  RawDataHandler::setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc_I)
  {
    """Set the feature_qc"""
    feature_qc_ = feature_qc_I;
  }
    
  OpenMS::MRMFeatureQC RawDataHandler::getFeatureQC() const
  {
    """Return the feature_qc"""
    return feature_qc_;
  }
    
  std::vector<OpenMS::FeatureMap>  RawDataHandler::getFeatureMapHistory() const
  {
    """the the featureMap to the history"""
    return featureMapHistory_;
  }
}