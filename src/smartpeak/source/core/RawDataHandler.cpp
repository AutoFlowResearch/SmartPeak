/**TODO:  Add copyright*/

#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  void RawDataHandler::setFeatureMap(const OpenMS::FeatureMap& featureMap)
  {
    featureMap_ = featureMap;
  }

  OpenMS::FeatureMap& RawDataHandler::getFeatureMap()
  {
    return featureMap_;
  }

  OpenMS::FeatureMap RawDataHandler::getFeatureMap() const
  {
    return featureMap_;
  }

  void RawDataHandler::setMetaData(const MetaDataHandler& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& RawDataHandler::getMetaData()
  {
    return meta_data_;
  }

  MetaDataHandler RawDataHandler::getMetaData() const
  {
    return meta_data_;
  }

  void RawDataHandler::setParameters(
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& parameters)
  {
    parameters_ = parameters;
  }

  std::map<std::string, std::vector<std::map<std::string, std::string>>>& RawDataHandler::getParameters()
  {
    return parameters_;
  }

  std::map<std::string, std::vector<std::map<std::string, std::string>>> RawDataHandler::getParameters() const
  {
    return parameters_;
  }

  void RawDataHandler::setTargetedExperiment(const OpenMS::TargetedExperiment& targeted_exp)
  {
    targeted_exp_ = targeted_exp;
  }

  OpenMS::TargetedExperiment& RawDataHandler::getTargetedExperiment()
  {
    return targeted_exp_;
  }

  OpenMS::TargetedExperiment RawDataHandler::getTargetedExperiment() const
  {
    return targeted_exp_;
  }

  // RawDataHandler::setReferenceData(, reference_data)
  // {
  //   """Set the reference_data"""
  //   .reference_data = reference_data
  // }

  // RawDataHandler::getReferenceData()
  // {
  //   """Return the reference_data"""
  //   return .reference_data
  // }

  void RawDataHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods()
  {
    return quantitation_methods_;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod> RawDataHandler::getQuantitationMethods() const
  {
    return quantitation_methods_;
  }

  void RawDataHandler::setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter)
  {
    feature_filter_ = feature_filter;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureFilter()
  {
    return feature_filter_;
  }

  OpenMS::MRMFeatureQC RawDataHandler::getFeatureFilter() const
  {
    return feature_filter_;
  }

  void RawDataHandler::setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc)
  {
    feature_qc_ = feature_qc;
  }

  OpenMS::MRMFeatureQC& RawDataHandler::getFeatureQC()
  {
    return feature_qc_;
  }

  OpenMS::MRMFeatureQC RawDataHandler::getFeatureQC() const
  {
    return feature_qc_;
  }

  void RawDataHandler::setFeatureMapHistory(const std::vector<OpenMS::FeatureMap>& feature_maps)
  {
    featureMapHistory_ = feature_maps;
  }

  std::vector<OpenMS::FeatureMap>& RawDataHandler::getFeatureMapHistory()
  {
    return featureMapHistory_;
  }

  std::vector<OpenMS::FeatureMap> RawDataHandler::getFeatureMapHistory() const
  {
    return featureMapHistory_;
  }
}