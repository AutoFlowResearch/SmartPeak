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

#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>

#include <plog/Log.h>

namespace SmartPeak
{

  std::vector<std::string> EstimateFeatureRSDs::getRequirements() const
  {
    return { "sequence", "traML" };
  }

  ParameterSet EstimateFeatureRSDs::getParameterSchema() const
  {
    return ParameterSet();
  }

  void EstimateFeatureRSDs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    LOGD << "START EstimateFeatureRSDs";
    getFilenames(filenames_I);

    // get all QCs
    std::vector<size_t> qcs_indices;
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::QC,
      qcs_indices
    );

    // check if there are any standards or QCs to estimate the feature filter parameters from
    if (qcs_indices.empty()) {
      throw std::invalid_argument("qcs_indices argument is empty.");
    }

    std::vector<OpenMS::FeatureMap> qcs_featureMaps;
    for (const size_t index : qcs_indices) {
      qcs_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    OpenMS::MRMFeatureFilter featureFilter;
    OpenMS::MRMFeatureQC rsd_estimations = sequenceSegmentHandler_IO.getFeatureRSDFilter();
    featureFilter.EstimatePercRSD(
      qcs_featureMaps, 
      rsd_estimations,
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment() // Targeted experiment used by all injections in the sequence
    );
    sequenceSegmentHandler_IO.getFeatureRSDEstimations() = rsd_estimations; // Transfer over the estimations

    LOGD << "END EstimateFeatureRSDs";
  }

}
