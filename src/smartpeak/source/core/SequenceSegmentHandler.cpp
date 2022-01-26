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

#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  SequenceSegmentHandler::SequenceSegmentHandler():
    quantitation_methods_(std::make_shared<std::vector<OpenMS::AbsoluteQuantitationMethod>>(std::vector<OpenMS::AbsoluteQuantitationMethod>())),
    feature_filter_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_qc_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_rsd_filter_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_rsd_qc_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_background_filter_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_background_qc_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_rsd_estimations_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC())),
    feature_background_estimations_(std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC()))
  {
  }

  void SequenceSegmentHandler::clear()
  {
    sequence_segment_name_.clear();
    sample_indices_.clear();
    standards_concentrations_.clear();
    if (quantitation_methods_!=nullptr) quantitation_methods_->clear();
    if (feature_filter_ != nullptr) feature_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_qc_ != nullptr) feature_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_rsd_filter_ != nullptr) feature_rsd_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_rsd_qc_ != nullptr) feature_rsd_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_background_filter_ != nullptr) feature_background_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_background_qc_ != nullptr) feature_background_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_rsd_estimations_ != nullptr) feature_rsd_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    if (feature_background_estimations_ != nullptr) feature_background_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC());
    components_to_concentrations_.clear();
    outlier_components_to_concentrations_.clear();
  }

  void SequenceSegmentHandler::setSequenceSegmentName(const std::string& sequence_segment_name)
  {
    sequence_segment_name_ = sequence_segment_name;
  }

  std::string& SequenceSegmentHandler::getSequenceSegmentName()
  {
    return sequence_segment_name_;
  }

  const std::string& SequenceSegmentHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name_;
  }

  void SequenceSegmentHandler::setSampleIndices(const std::vector<size_t>& sample_indices)
  {
    sample_indices_ = sample_indices;
  }

  std::vector<size_t>& SequenceSegmentHandler::getSampleIndices()
  {
    return sample_indices_;
  }

  const std::vector<size_t>& SequenceSegmentHandler::getSampleIndices() const
  {
    return sample_indices_;
  }

  void SequenceSegmentHandler::setStandardsConcentrations(const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& standards_concentrations)
  {
    standards_concentrations_ = standards_concentrations;
  }

  std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& SequenceSegmentHandler::getStandardsConcentrations()
  {
    return standards_concentrations_;
  }

  const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& SequenceSegmentHandler::getStandardsConcentrations() const
  {
    return standards_concentrations_;
  }

  void SequenceSegmentHandler::setQuantitationMethods(const std::vector<OpenMS::AbsoluteQuantitationMethod>& quantitation_methods)
  {
    quantitation_methods_.reset(new std::vector<OpenMS::AbsoluteQuantitationMethod>(quantitation_methods));
  }

  void SequenceSegmentHandler::setQuantitationMethods(std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& quantitation_methods)
  {
    quantitation_methods_ = quantitation_methods;
  }

  std::vector<OpenMS::AbsoluteQuantitationMethod>& SequenceSegmentHandler::getQuantitationMethods()
  {
    return *(quantitation_methods_.get());
  }

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& SequenceSegmentHandler::getQuantitationMethods() const
  {
    return *(quantitation_methods_.get());
  }

  std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& SequenceSegmentHandler::getQuantitationMethodsShared()
  {
    return quantitation_methods_;
  }

  void SequenceSegmentHandler::setFeatureFilter(const OpenMS::MRMFeatureQC& feature_filter)
  {
    feature_filter_.reset(new OpenMS::MRMFeatureQC(feature_filter));
  }

  void SequenceSegmentHandler::setFeatureFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_filter)
  {
    feature_filter_ = feature_filter;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureFilter()
  {
    return *(feature_filter_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureFilter() const
  {
    return *(feature_filter_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureFilterShared()
  {
    return feature_filter_;
  }

  void SequenceSegmentHandler::setFeatureQC(const OpenMS::MRMFeatureQC& feature_qc)
  {
    feature_qc_.reset(new OpenMS::MRMFeatureQC(feature_qc));
  }

  void SequenceSegmentHandler::setFeatureQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_qc)
  {
    feature_qc_ = feature_qc;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureQC()
  {
    return *(feature_qc_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureQC() const
  {
    return *(feature_qc_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureQCShared()
  {
    return feature_qc_;
  }

  void SequenceSegmentHandler::setFeatureRSDFilter(const OpenMS::MRMFeatureQC& feature_rsd_filter)
  {
    feature_rsd_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_rsd_filter));
  }

  void SequenceSegmentHandler::setFeatureRSDFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_filter)
  {
    feature_rsd_filter_ = feature_rsd_filter;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureRSDFilter()
  {
    return *(feature_rsd_filter_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureRSDFilter() const
  {
    return *(feature_rsd_filter_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureRSDFilterShared()
  {
    return feature_rsd_filter_;
  }

  void SequenceSegmentHandler::setFeatureRSDQC(const OpenMS::MRMFeatureQC& feature_rsd_qc)
  {
    feature_rsd_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_rsd_qc));
  }

  void SequenceSegmentHandler::setFeatureRSDQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_qc)
  {
    feature_rsd_qc_ = feature_rsd_qc;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureRSDQC()
  {
    return *(feature_rsd_qc_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureRSDQC() const
  {
    return *(feature_rsd_qc_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureRSDQCShared()
  {
    return feature_rsd_qc_;
  }

  void SequenceSegmentHandler::setFeatureBackgroundFilter(const OpenMS::MRMFeatureQC& feature_background_filter)
  {
    feature_background_filter_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_background_filter));
  }

  void SequenceSegmentHandler::setFeatureBackgroundFilter(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_filter)
  {
    feature_background_filter_ = feature_background_filter;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureBackgroundFilter()
  {
    return *(feature_background_filter_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureBackgroundFilter() const
  {
    return *(feature_background_filter_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureBackgroundFilterShared()
  {
    return feature_background_filter_;
  }

  void SequenceSegmentHandler::setFeatureBackgroundQC(const OpenMS::MRMFeatureQC& feature_background_qc)
  {
    feature_background_qc_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_background_qc));
  }

  void SequenceSegmentHandler::setFeatureBackgroundQC(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_qc)
  {
    feature_background_qc_ = feature_background_qc;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureBackgroundQC()
  {
    return *(feature_background_qc_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureBackgroundQC() const
  {
    return *(feature_background_qc_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureBackgroundQCShared()
  {
    return feature_background_qc_;
  }

  void SequenceSegmentHandler::setFeatureRSDEstimations(const OpenMS::MRMFeatureQC& feature_rsd_estimations)
  {
    feature_rsd_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_rsd_estimations));
  }

  void SequenceSegmentHandler::setFeatureRSDEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_rsd_estimations)
  {
    feature_rsd_estimations_ = feature_rsd_estimations;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureRSDEstimations()
  {
    return *(feature_rsd_estimations_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureRSDEstimations() const
  {
    return *(feature_rsd_estimations_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureRSDEstimationsShared()
  {
    return feature_rsd_estimations_;
  }

  void SequenceSegmentHandler::setFeatureBackgroundEstimations(const OpenMS::MRMFeatureQC& feature_background_estimations)
  {
    feature_background_estimations_ = std::make_shared<OpenMS::MRMFeatureQC>(OpenMS::MRMFeatureQC(feature_background_estimations));
  }

  void SequenceSegmentHandler::setFeatureBackgroundEstimations(std::shared_ptr<OpenMS::MRMFeatureQC>& feature_background_estimations)
  {
    feature_background_estimations_ = feature_background_estimations;
  }

  OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureBackgroundEstimations()
  {
    return *(feature_background_estimations_.get());
  }

  const OpenMS::MRMFeatureQC& SequenceSegmentHandler::getFeatureBackgroundEstimations() const
  {
    return *(feature_background_estimations_.get());
  }

  std::shared_ptr<OpenMS::MRMFeatureQC>& SequenceSegmentHandler::getFeatureBackgroundEstimationsShared()
  {
    return feature_background_estimations_;
  }

  void SequenceSegmentHandler::setComponentsToConcentrations(
    const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
  )
  {
    components_to_concentrations_ = components_to_concentrations;
  }

  std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
  SequenceSegmentHandler::getComponentsToConcentrations()
  {
    return components_to_concentrations_;
  }

  const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
  SequenceSegmentHandler::getComponentsToConcentrations() const
  {
    return components_to_concentrations_;
  }

  void SequenceSegmentHandler::setOutlierComponentsToConcentrations(
    const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations
  )
  {
    outlier_components_to_concentrations_ = components_to_concentrations;
  }

  std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    SequenceSegmentHandler::getOutlierComponentsToConcentrations()
  {
    return outlier_components_to_concentrations_;
  }

  const std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    SequenceSegmentHandler::getOutlierComponentsToConcentrations() const
  {
    return outlier_components_to_concentrations_;
  }
}
