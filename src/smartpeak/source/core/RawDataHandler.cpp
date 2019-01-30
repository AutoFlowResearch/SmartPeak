/**TODO:  Add copyright*/

#include <SmartPeak/core/RawDataHandler.h>

namespace SmartPeak
{
  void RawDataHandler::setFeatureMap(const OpenMS::FeatureMap& feature_map)
  {
    feature_map_ = feature_map;
  }

  OpenMS::FeatureMap& RawDataHandler::getFeatureMap()
  {
    return feature_map_;
  }

  const OpenMS::FeatureMap& RawDataHandler::getFeatureMap() const
  {
    return feature_map_;
  }

  void RawDataHandler::setMetaData(const MetaDataHandler& meta_data)
  {
    meta_data_ = meta_data;
  }

  MetaDataHandler& RawDataHandler::getMetaData()
  {
    return meta_data_;
  }

  const MetaDataHandler& RawDataHandler::getMetaData() const
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

  const std::map<std::string, std::vector<std::map<std::string, std::string>>>& RawDataHandler::getParameters() const
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

  const OpenMS::TargetedExperiment& RawDataHandler::getTargetedExperiment() const
  {
    return targeted_exp_;
  }

  void RawDataHandler::setReferenceData(const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data)
  {
    reference_data_ = reference_data;
  }

  std::vector<std::map<std::string, Utilities::CastValue>>& RawDataHandler::getReferenceData()
  {
    return reference_data_;
  }

  const std::vector<std::map<std::string, Utilities::CastValue>>& RawDataHandler::getReferenceData() const
  {
    return reference_data_;
  }

  void RawDataHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods()
  {
    return quantitation_methods_;
  }

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& RawDataHandler::getQuantitationMethods() const
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

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureFilter() const
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

  const OpenMS::MRMFeatureQC& RawDataHandler::getFeatureQC() const
  {
    return feature_qc_;
  }

  void RawDataHandler::setFeatureMapHistory(const std::vector<OpenMS::FeatureMap>& feature_maps)
  {
    feature_map_history_ = feature_maps;
  }

  std::vector<OpenMS::FeatureMap>& RawDataHandler::getFeatureMapHistory()
  {
    return feature_map_history_;
  }

  const std::vector<OpenMS::FeatureMap>& RawDataHandler::getFeatureMapHistory() const
  {
    return feature_map_history_;
  }

  void RawDataHandler::setExperiment(const OpenMS::MSExperiment& experiment)
  {
    experiment_ = experiment;
  }

  OpenMS::MSExperiment& RawDataHandler::getExperiment()
  {
    return experiment_;
  }

  const OpenMS::MSExperiment& RawDataHandler::getExperiment() const
  {
    return experiment_;
  }

  void RawDataHandler::setChromatogramMap(const OpenMS::MSExperiment& chromatogram_map)
  {
    chromatogram_map_ = chromatogram_map;
  }

  OpenMS::MSExperiment& RawDataHandler::getChromatogramMap()
  {
    return chromatogram_map_;
  }

  const OpenMS::MSExperiment& RawDataHandler::getChromatogramMap() const
  {
    return chromatogram_map_;
  }

  void RawDataHandler::setTransformationDescription(const OpenMS::TransformationDescription& trafo)
  {
    trafo_ = trafo;
  }

  OpenMS::TransformationDescription& RawDataHandler::getTransformationDescription()
  {
    return trafo_;
  }

  const OpenMS::TransformationDescription& RawDataHandler::getTransformationDescription() const
  {
    return trafo_;
  }

  void RawDataHandler::setSWATH(const OpenMS::MSExperiment& swath)
  {
    swath_ = swath;
  }

  OpenMS::MSExperiment& RawDataHandler::getSWATH()
  {
    return swath_;
  }

  const OpenMS::MSExperiment& RawDataHandler::getSWATH() const
  {
    return swath_;
  }

  void RawDataHandler::setValidationMetrics(const std::map<std::string, float>& validation_metrics)
  {
    validation_metrics_ = validation_metrics;
  }

  std::map<std::string, float>& RawDataHandler::getValidationMetrics()
  {
    return validation_metrics_;
  }

  const std::map<std::string, float>& RawDataHandler::getValidationMetrics() const
  {
    return validation_metrics_;
  }

  void RawDataHandler::clear()
  {
    experiment_.clear(true);
    chromatogram_map_.clear(true);
    trafo_ = OpenMS::TransformationDescription();
    swath_.clear(true);
    feature_map_.clear(true);
    meta_data_.clear();
    validation_metrics_.clear();
    feature_map_history_.clear();
    parameters_.clear();
    targeted_exp_.clear(true);
    quantitation_methods_.clear();
    feature_filter_ = OpenMS::MRMFeatureQC();
    feature_qc_ = OpenMS::MRMFeatureQC();
  }
}
