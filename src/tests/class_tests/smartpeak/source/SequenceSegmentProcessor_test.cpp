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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SequenceSegmentProcessors/OptimizeCalibration.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureBackgroundInterferences.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureFilterValues.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureQCValues.h>
#include <SmartPeak/core/SequenceSegmentProcessors/EstimateFeatureRSDs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureBackgroundQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureRSDEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureRSDFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadFeatureRSDQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadQuantitationMethods.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadStandardsConcentrations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreQuantitationMethods.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreStandardsConcentrations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/TransferLOQToFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/TransferLOQToFeatureQCs.h>

using namespace SmartPeak;
using namespace std;

void makeStandardsFeaturesAndConcentrations(SequenceHandler& sequenceHandler_IO, vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& runs, std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>>& absQuantMethods_ptr){
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    2.32e4, 2.45e4, 1.78e4, 2.11e4, 1.91e4,
    2.06e4, 1.85e4, 1.53e4, 1.40e4, 1.03e4,
    1.07e4, 6.68e3, 5.27e3, 2.83e3
  };
  const vector<double> y1 = {
    4.94e3, 6.55e3, 7.37e3, 1.54e4, 2.87e4,
    5.41e4, 1.16e5, 1.85e5, 3.41e5, 7.54e5,
    9.76e5, 1.42e6, 1.93e6, 2.23e6
  };
  const vector<double> z1 = {
    1.00e-2, 2.00e-2, 4.00e-2, 1.00e-1, 2.00e-1,
    4.00e-1, 1.00e0, 2.00e0, 4.00e0, 1.00e1,
    2.00e1, 4.00e1, 1.00e2, 2.00e2
  };

  // amp.amp_1.Light
  const vector<double> x2 = {
    2.15e5, 2.32e5, 2.69e5, 2.53e5, 2.50e5,
    2.75e5, 2.67e5, 3.31e5, 3.15e5, 3.04e5,
    3.45e5, 3.91e5, 4.62e5, 3.18e5
  };
  const vector<double> y2 = {
      4.40e2, 1.15e3, 1.53e3, 2.01e3, 4.47e3,
      7.36e3, 2.18e4, 4.46e4, 8.50e4, 2.33e5,
      5.04e5, 1.09e6, 2.54e6, 3.64e6
  };
  const vector<double> z2 = {
      2.00e-3, 4.00e-3, 8.00e-3, 2.00e-2, 4.00e-2,
      8.00e-2, 2.00e-1, 4.00e-1, 8.00e-1, 2.00e0,
      4.00e0, 8.00e0, 2.00e1, 4.00e1
  };
  
  // atp.atp_1.Light
  const vector<double> x3 = {
      8.28e2, 1.32e3, 1.57e3, 1.63e3, 1.48e3,
      2.43e3, 4.44e3, 1.03e4, 1.75e4, 6.92e4,
      1.97e5, 2.69e5, 3.20e5, 3.22e5
  };
  const vector<double> y3 = {
      2.21e2, 4.41e2, 3.31e2, 2.21e2, 3.09e2,
      5.96e2, 1.26e3, 2.49e3, 1.12e4, 8.79e4,
      4.68e5, 1.38e6, 3.46e6, 4.19e6
  };
  const vector<double> z3 = {
      2.00e-3, 4.00e-3, 8.00e-3, 2.00e-2, 4.00e-2,
      8.00e-2, 2.00e-1, 4.00e-1, 8.00e-1, 2.00e0,
      4.00e0, 8.00e0, 2.00e1, 4.00e1
  };

  runs.clear();

  for (size_t i = 0; i < x1.size(); ++i) {
    const string sample_name = "level" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::MRMFeature mrm_feature;
    OpenMS::Feature component;
    OpenMS::Feature IS_component;
    OpenMS::AbsoluteQuantitationStandards::runConcentration run;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", y1[i]);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    mrm_feature.setSubordinates({component, IS_component});
    feature_map.push_back(mrm_feature);

    run.sample_name = sample_name;
    run.component_name = "ser-L.ser-L_1.Light";
    run.IS_component_name = "ser-L.ser-L_1.Heavy";
    run.actual_concentration = z1[i];
    run.IS_actual_concentration = 1.0;
    run.concentration_units = "uM";
    run.dilution_factor = 1.0;
    runs.push_back(run);

    // # amp.amp_1.Light
    component.setMetaValue("native_id", "amp.amp_1.Light");
    component.setMetaValue("peak_apex_int", y2[i]);
    IS_component.setMetaValue("native_id", "amp.amp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x2[i]);
    mrm_feature.setSubordinates({component, IS_component});
    feature_map.push_back(mrm_feature);

    run.sample_name = sample_name;
    run.component_name = "amp.amp_1.Light";
    run.IS_component_name = "amp.amp_1.Heavy";
    run.actual_concentration = z2[i];
    run.IS_actual_concentration = 1.0;
    run.concentration_units = "uM";
    run.dilution_factor = 1.0;
    runs.push_back(run);

    // atp.atp_1.Light
    component.setMetaValue("native_id", "atp.atp_1.Light");
    component.setMetaValue("peak_apex_int", y3[i]);
    IS_component.setMetaValue("native_id", "atp.atp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x3[i]);
    mrm_feature.setSubordinates({component, IS_component});
    feature_map.push_back(mrm_feature);

    run.sample_name = sample_name;
    run.component_name = "atp.atp_1.Light";
    run.IS_component_name = "atp.atp_1.Heavy";
    run.actual_concentration = z3[i];
    run.IS_actual_concentration = 1.0;
    run.concentration_units = "uM";
    run.dilution_factor = 1.0;
    runs.push_back(run);

    feature_map.setPrimaryMSRunPath({sample_name});

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::Standard);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    sequenceHandler_IO.addSampleToSequence(meta_data, OpenMS::FeatureMap());

    RawDataHandler rawDataHandler;
    rawDataHandler.setFeatureMap(feature_map);
    rawDataHandler.setQuantitationMethods(absQuantMethods_ptr);
    sequenceHandler_IO.getSequence().at(i).setRawData(rawDataHandler);
  }
}

void addStandardsFeatures(SequenceHandler& sequenceHandler_IO){
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    2.32e4, 2.45e4, 1.78e4, 2.11e4, 1.91e4,
    2.06e4, 1.85e4, 1.53e4, 1.40e4, 1.03e4,
    1.07e4, 6.68e3, 5.27e3, 2.83e3
  };
  const vector<double> y1 = {
    4.94e3, 6.55e3, 7.37e3, 1.54e4, 2.87e4,
    5.41e4, 1.16e5, 1.85e5, 3.41e5, 7.54e5,
    9.76e5, 1.42e6, 1.93e6, 2.23e6
  };

  // amp.amp_1.Light
  const vector<double> x2 = {
    2.15e5, 2.32e5, 2.69e5, 2.53e5, 2.50e5,
    2.75e5, 2.67e5, 3.31e5, 3.15e5, 3.04e5,
    3.45e5, 3.91e5, 4.62e5, 3.18e5
  };
  const vector<double> y2 = {
      4.40e2, 1.15e3, 1.53e3, 2.01e3, 4.47e3,
      7.36e3, 2.18e4, 4.46e4, 8.50e4, 2.33e5,
      5.04e5, 1.09e6, 2.54e6, 3.64e6
  };

  // atp.atp_1.Light
  const vector<double> x3 = {
      8.28e2, 1.32e3, 1.57e3, 1.63e3, 1.48e3,
      2.43e3, 4.44e3, 1.03e4, 1.75e4, 6.92e4,
      1.97e5, 2.69e5, 3.20e5, 3.22e5
  };
  const vector<double> y3 = {
      2.21e2, 4.41e2, 3.31e2, 2.21e2, 3.09e2,
      5.96e2, 1.26e3, 2.49e3, 1.12e4, 8.79e4,
      4.68e5, 1.38e6, 3.46e6, 4.19e6
  };
  for (size_t i = 0; i < x1.size(); ++i) {
    const string sample_name = "StandardLevel" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::MRMFeature mrm_feature;
    OpenMS::Feature component;
    OpenMS::Feature IS_component;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", y1[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y1[i]);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x1[i]);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setIntensity(x1[i] + y1[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // # amp.amp_1.Light
    component.setMetaValue("native_id", "amp.amp_1.Light");
    component.setMetaValue("peak_apex_int", y2[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y2[i]);
    IS_component.setMetaValue("native_id", "amp.amp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x2[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x2[i]);
    mrm_feature.setMetaValue("PeptideRef", "amp");
    mrm_feature.setIntensity(x2[i] + y2[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // atp.atp_1.Light
    component.setMetaValue("native_id", "atp.atp_1.Light");
    component.setMetaValue("peak_apex_int", y3[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y3[i]);
    IS_component.setMetaValue("native_id", "atp.atp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x3[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x3[i]);
    mrm_feature.setMetaValue("PeptideRef", "atp");
    mrm_feature.setIntensity(x3[i] + y3[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    feature_map.setPrimaryMSRunPath({ sample_name });

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::Standard);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    sequenceHandler_IO.addSampleToSequence(meta_data, feature_map);
  }
}

void addBlanksFeatures(SequenceHandler& sequenceHandler_IO) {
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    2.32e2, 2.45e2, 1.78e2, 2.11e2, 1.91e2,
    2.06e2, 1.85e2, 1.53e2, 1.40e2, 1.03e2,
    1.07e2, 6.68e1, 5.27e1, 2.83e1
  };
  const vector<double> y1 = {
    4.94e1, 6.55e1, 7.37e1, 1.54e2, 2.87e2,
    5.41e2, 1.16e3, 1.85e3, 3.41e3, 7.54e3,
    9.76e3, 1.42e6, 1.93e6, 2.23e6
  };

  // amp.amp_1.Light
  const vector<double> x2 = {
    2.15e3, 2.32e3, 2.69e3, 2.53e3, 2.50e3,
    2.75e3, 2.67e3, 3.31e3, 3.15e3, 3.04e3,
    3.45e3, 3.91e3, 4.62e3, 3.18e3
  };
  const vector<double> y2 = {
      4.40e0, 1.15e1, 1.53e1, 2.01e1, 4.47e1,
      7.36e1, 2.18e2, 4.46e2, 8.50e2, 2.33e3,
      5.04e3, 1.09e4, 2.54e4, 3.64e4
  };

  // atp.atp_1.Light
  const vector<double> x3 = {
      8.28e0, 1.32e1, 1.57e1, 1.63e1, 1.48e1,
      2.43e1, 4.44e1, 1.03e2, 1.75e2, 6.92e2,
      1.97e3, 2.69e3, 3.20e3, 3.22e3
  };
  const vector<double> y3 = {
      2.21e0, 4.41e0, 3.31e0, 2.21e0, 3.09e0,
      5.96e0, 1.26e1, 2.49e1, 1.12e2, 8.79e2,
      4.68e3, 1.38e4, 3.46e4, 4.19e4
  };

  for (size_t i = 0; i < x1.size(); ++i) {
    const string sample_name = "BlankLevel" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::MRMFeature mrm_feature;
    OpenMS::Feature component;
    OpenMS::Feature IS_component;
    OpenMS::AbsoluteQuantitationStandards::runConcentration run;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", y1[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y1[i]);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x1[i]);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setIntensity(x1[i] + y1[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // # amp.amp_1.Light
    component.setMetaValue("native_id", "amp.amp_1.Light");
    component.setMetaValue("peak_apex_int", y2[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y2[i]);
    IS_component.setMetaValue("native_id", "amp.amp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x2[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x2[i]);
    mrm_feature.setMetaValue("PeptideRef", "amp");
    mrm_feature.setIntensity(x2[i] + y2[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // atp.atp_1.Light
    component.setMetaValue("native_id", "atp.atp_1.Light");
    component.setMetaValue("peak_apex_int", y3[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y3[i]);
    IS_component.setMetaValue("native_id", "atp.atp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x3[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x3[i]);
    mrm_feature.setMetaValue("PeptideRef", "atp");
    mrm_feature.setIntensity(x3[i] + y3[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    feature_map.setPrimaryMSRunPath({ sample_name });

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::Blank);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    sequenceHandler_IO.addSampleToSequence(meta_data, feature_map);
  }
}

void addQCFeatures(SequenceHandler& sequenceHandler_IO) {
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    2.32e4, 2.45e4, 1.78e4, 2.11e4, 1.91e4,
    2.06e4, 1.85e4, 1.53e4, 1.40e4, 1.03e4,
    1.07e4, 6.68e3, 5.27e3, 2.83e3
  };
  const vector<double> y1 = {
    2.32e4, 2.45e4, 1.78e4, 2.11e4, 1.91e4,
    2.06e4, 1.85e4, 1.53e4, 1.40e4, 1.03e4,
    1.07e4, 6.68e3, 5.27e3, 2.83e3
  };

  // amp.amp_1.Light
  const vector<double> x2 = {
    2.15e5, 2.32e5, 2.69e5, 2.53e5, 2.50e5,
    2.75e5, 2.67e5, 3.31e5, 3.15e5, 3.04e5,
    3.45e5, 3.91e5, 4.62e5, 3.18e5
  };
  const vector<double> y2 = {
    2.15e5, 2.32e5, 2.69e5, 2.53e5, 2.50e5,
    2.75e5, 2.67e5, 3.31e5, 3.15e5, 3.04e5,
    3.45e5, 3.91e5, 4.62e5, 3.18e5
  };

  // atp.atp_1.Light
  const vector<double> x3 = {
    8.28e2, 1.32e3, 1.57e3, 1.63e3, 1.48e3,
    2.43e3, 4.44e3, 1.03e4, 1.75e4, 6.92e4,
    1.97e5, 2.69e5, 3.20e5, 3.22e5
  };
  const vector<double> y3 = {
    8.28e2, 1.32e3, 1.57e3, 1.63e3, 1.48e3,
    2.43e3, 4.44e3, 1.03e4, 1.75e4, 6.92e4,
    1.97e5, 2.69e5, 3.20e5, 3.22e5
  };

  for (size_t i = 0; i < x1.size(); ++i) {
    const string sample_name = "QCLevel" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::MRMFeature mrm_feature;
    OpenMS::Feature component;
    OpenMS::Feature IS_component;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", y1[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y1[i]);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x1[i]);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setSubordinates({ component, IS_component });
    mrm_feature.setIntensity(x1[i] + y1[i]);
    feature_map.push_back(mrm_feature);

    // # amp.amp_1.Light
    component.setMetaValue("native_id", "amp.amp_1.Light");
    component.setMetaValue("peak_apex_int", y2[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y2[i]);
    IS_component.setMetaValue("native_id", "amp.amp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x2[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x2[i]);
    mrm_feature.setMetaValue("PeptideRef", "amp");
    mrm_feature.setIntensity(x2[i] + y2[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // atp.atp_1.Light
    component.setMetaValue("native_id", "atp.atp_1.Light");
    component.setMetaValue("peak_apex_int", y3[i]);
    component.setMetaValue("LabelType", "Light");
    component.setIntensity(y3[i]);
    IS_component.setMetaValue("native_id", "atp.atp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x3[i]);
    IS_component.setMetaValue("LabelType", "Heavy");
    IS_component.setIntensity(x3[i]);
    mrm_feature.setMetaValue("PeptideRef", "atp");
    mrm_feature.setIntensity(x3[i] + y3[i]);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    feature_map.setPrimaryMSRunPath({ sample_name });

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::QC);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    sequenceHandler_IO.addSampleToSequence(meta_data, feature_map);
  }
}

TEST(OptimizeCalibration, constructorOptimizeCalibration)
{
  OptimizeCalibration* ptrOptimizeCalibration = nullptr;
  OptimizeCalibration* nullPointerOptimizeCalibration = nullptr;
  EXPECT_EQ(ptrOptimizeCalibration, nullPointerOptimizeCalibration);
}

TEST(OptimizeCalibration, destructorOptimizeCalibration)
{
  OptimizeCalibration* ptrOptimizeCalibration = nullptr;
  ptrOptimizeCalibration = new OptimizeCalibration();
  delete ptrOptimizeCalibration;
}

TEST(OptimizeCalibration, gettersOptimizeCalibration)
{
  OptimizeCalibration processor;
  EXPECT_EQ(processor.getName(), "OPTIMIZE_CALIBRATION");
}

TEST(SequenceSegmentProcessor, getSampleIndicesBySampleType)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment");
  meta_data1.setSampleType(SampleType::Unknown);
  meta_data1.acq_method_name = "6";
  meta_data1.inj_volume = 7.0;
  meta_data1.inj_volume_units = "8";
  meta_data1.batch_name = "9";

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment");
  meta_data2.setSampleType(SampleType::Standard);
  meta_data2.acq_method_name = "6";
  meta_data2.inj_volume = 7.0;
  meta_data2.inj_volume_units = "8";
  meta_data2.batch_name = "9";

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment");
  meta_data3.setSampleType(SampleType::Unknown);
  meta_data3.acq_method_name = "6";
  meta_data3.inj_volume = 7.0;
  meta_data3.inj_volume_units = "8";
  meta_data3.batch_name = "9";

  SequenceHandler sequenceHandler;
  sequenceHandler.addSampleToSequence(meta_data1, OpenMS::FeatureMap());
  sequenceHandler.addSampleToSequence(meta_data2, OpenMS::FeatureMap());
  sequenceHandler.addSampleToSequence(meta_data3, OpenMS::FeatureMap());

  SequenceSegmentHandler sequenceSegmentHandler;

  sequenceSegmentHandler.setSampleIndices({0, 1, 2});

  std::vector<size_t> sample_indices;
  SequenceSegmentProcessor::getSampleIndicesBySampleType(
    sequenceSegmentHandler,
    sequenceHandler,
    SampleType::Unknown,
    sample_indices
  );

  EXPECT_EQ(sample_indices.size(), 2);
  EXPECT_EQ(sample_indices[0], 0);
  EXPECT_EQ(sample_indices[1], 2);
}

TEST(SequenceSegmentProcessor, processOptimizeCalibration)
{
  // Pre-requisites: set up the parameters and data structures for testing
  const map<string, vector<map<string, string>>> absquant_params = {{"AbsoluteQuantitation", {
    {
      {"name", "min_points"},
      {"value", "4"}
    },
    {
      {"name", "max_bias"},
      {"value", "30.0"}
    },
    {
      {"name", "min_correlation_coefficient"},
      {"value", "0.9"}
    },
    {
      {"name", "max_iters"},
      {"value", "100"}
    },
    {
      {"name", "outlier_detection_method"},
      {"value", "iter_jackknife"}
    },
    {
      {"name", "use_chauvenet"},
      {"value", "false"}
    }
  }}};

  const string feature_name = "peak_apex_int";
  const string transformation_model = "linear";
  OpenMS::Param param;
  param.setValue("slope", 1.0);
  param.setValue("intercept", 0.0);
  param.setValue("x_weight", "ln(x)");
  param.setValue("y_weight", "ln(y)");
  param.setValue("x_datum_min", -1e12);
  param.setValue("x_datum_max", 1e12);
  param.setValue("y_datum_min", -1e12);
  param.setValue("y_datum_max", 1e12);

  vector<OpenMS::AbsoluteQuantitationMethod> quant_methods;

  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("ser-L.ser-L_1.Light");
  aqm.setISName("ser-L.ser-L_1.Heavy");
  aqm.setFeatureName(feature_name);
  aqm.setConcentrationUnits("uM");
  aqm.setTransformationModel(transformation_model);
  aqm.setTransformationModelParams(param);
  quant_methods.push_back(aqm);

  aqm.setComponentName("amp.amp_1.Light");
  aqm.setISName("amp.amp_1.Heavy");
  quant_methods.push_back(aqm);

  aqm.setComponentName("atp.atp_1.Light");
  aqm.setISName("atp.atp_1.Heavy");
  quant_methods.push_back(aqm);

  SequenceSegmentHandler sequenceSegmentHandler;

  sequenceSegmentHandler.setQuantitationMethods(quant_methods);
  std::shared_ptr<std::vector<OpenMS::AbsoluteQuantitationMethod>> absQuantMethods_ptr = sequenceSegmentHandler.getQuantitationMethodsShared();

  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs;
  SequenceHandler sequenceHandler;
  makeStandardsFeaturesAndConcentrations(sequenceHandler, runs, absQuantMethods_ptr);
  sequenceSegmentHandler.setStandardsConcentrations(runs);

  vector<size_t> indices(sequenceHandler.getSequence().size());
  std::iota(indices.begin(), indices.end(), 0);

  sequenceSegmentHandler.setSampleIndices(indices);

  // Test calculate calibration
  OptimizeCalibration optimizeCalibration;
  Filenames filenames;
  optimizeCalibration.process(sequenceSegmentHandler, sequenceHandler, absquant_params, filenames);

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs = sequenceSegmentHandler.getQuantitationMethods();

  EXPECT_EQ(AQMs.size(), 3);

  EXPECT_EQ(AQMs[0].getComponentName(), "amp.amp_1.Light");
  EXPECT_EQ(AQMs[0].getISName(), "amp.amp_1.Heavy");
  EXPECT_EQ(AQMs[0].getFeatureName(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("slope")), 0.957996830126945, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("intercept")), -1.0475433871941753, 1e-6);
  EXPECT_EQ(AQMs[0].getNPoints(), 11);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getCorrelationCoefficient()), 0.9991692616730385, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getLLOQ()), 0.02, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getULOQ()), 40.0, 1e-6);

  EXPECT_EQ(AQMs[1].getComponentName(), "atp.atp_1.Light");
  EXPECT_EQ(AQMs[1].getISName(), "atp.atp_1.Heavy");
  EXPECT_EQ(AQMs[1].getFeatureName(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("slope")), 0.6230408240794582, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("intercept")), 0.36130172586029285, 1e-6);
  EXPECT_EQ(AQMs[1].getNPoints(), 6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getCorrelationCoefficient()), 0.9982084021849695, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getLLOQ()), 0.02, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getULOQ()), 40.0, 1e-6);

  EXPECT_EQ(AQMs[2].getComponentName(), "ser-L.ser-L_1.Light");
  EXPECT_EQ(AQMs[2].getISName(), "ser-L.ser-L_1.Heavy");
  EXPECT_EQ(AQMs[2].getFeatureName(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("slope")), 0.9011392589148208, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("intercept")), 1.8701850759567624, 1e-6);
  EXPECT_EQ(AQMs[2].getNPoints(), 11);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getCorrelationCoefficient()), 0.9993200722867581, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getLLOQ()), 0.04, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getULOQ()), 200.0, 1e-6);

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs_rdh = sequenceHandler.getSequence()[0].getRawData().getQuantitationMethods();
  EXPECT_EQ(AQMs_rdh.size(), 3);

  EXPECT_EQ(AQMs_rdh[0].getComponentName(), "amp.amp_1.Light");
  EXPECT_EQ(AQMs_rdh[0].getISName(), "amp.amp_1.Heavy");
  EXPECT_EQ(AQMs_rdh[0].getFeatureName(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[0].getTransformationModelParams().getValue("slope")), 0.957996830126945, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[0].getTransformationModelParams().getValue("intercept")), -1.0475433871941753, 1e-6);
  EXPECT_EQ(AQMs_rdh[0].getNPoints(), 11);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[0].getCorrelationCoefficient()), 0.9991692616730385, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[0].getLLOQ()), 0.02, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[0].getULOQ()), 40.0, 1e-6);

  EXPECT_EQ(AQMs_rdh[1].getComponentName(), "atp.atp_1.Light");
  EXPECT_EQ(AQMs_rdh[1].getISName(), "atp.atp_1.Heavy");
  EXPECT_EQ(AQMs_rdh[1].getFeatureName(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[1].getTransformationModelParams().getValue("slope")), 0.6230408240794582, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[1].getTransformationModelParams().getValue("intercept")), 0.36130172586029285, 1e-6);
  EXPECT_EQ(AQMs_rdh[1].getNPoints(), 6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[1].getCorrelationCoefficient()), 0.9982084021849695, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[1].getLLOQ()), 0.02, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[1].getULOQ()), 40.0, 1e-6);

  EXPECT_EQ(AQMs_rdh[2].getComponentName(), "ser-L.ser-L_1.Light");
  EXPECT_EQ(AQMs_rdh[2].getISName(), "ser-L.ser-L_1.Heavy");
  EXPECT_EQ(AQMs_rdh[2].getFeatureName(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[2].getTransformationModelParams().getValue("slope")), 0.9011392589148208, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[2].getTransformationModelParams().getValue("intercept")), 1.8701850759567624, 1e-6);
  EXPECT_EQ(AQMs_rdh[2].getNPoints(), 11);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[2].getCorrelationCoefficient()), 0.9993200722867581, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[2].getLLOQ()), 0.04, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs_rdh[2].getULOQ()), 200.0, 1e-6);

  const auto& component_to_concentrations = sequenceSegmentHandler.getComponentsToConcentrations();
  EXPECT_EQ(component_to_concentrations.size(), 3);
  ASSERT_EQ(component_to_concentrations.count("ser-L.ser-L_1.Light"), 1);
  const auto& component_to_concentration = component_to_concentrations.at("ser-L.ser-L_1.Light");
  ASSERT_EQ(component_to_concentration.size(), 11);
  EXPECT_FLOAT_EQ(component_to_concentration[0].actual_concentration, 0.039999999);
  EXPECT_EQ(component_to_concentration[0].concentration_units, std::string("uM"));
  EXPECT_FLOAT_EQ(component_to_concentration[0].dilution_factor, 1);
  EXPECT_FLOAT_EQ(component_to_concentration[0].IS_actual_concentration, 1);
/*
  const auto& outlier_component_to_concentrations = sequenceSegmentHandler.getOutlierComponentsToConcentrations();
  ASSERT_EQ(outlier_component_to_concentrations.size(), 3);
  ASSERT_EQ(outlier_component_to_concentrations.count("ser-L.ser-L_1.Light"), 1);
  const auto& outlier_component_to_concentration = outlier_component_to_concentrations.at("ser-L.ser-L_1.Light");
  ASSERT_EQ(outlier_component_to_concentration.size(), 3);
  EXPECT_FLOAT_EQ(outlier_component_to_concentration[0].actual_concentration, 0.0099999998);
  EXPECT_EQ(outlier_component_to_concentration[0].concentration_units, std::string("uM"));
  EXPECT_FLOAT_EQ(outlier_component_to_concentration[0].dilution_factor, 1);
  EXPECT_FLOAT_EQ(outlier_component_to_concentration[0].IS_actual_concentration, 1);
*/
}

/**
  LoadStandardsConcentrations Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadStandardsConcentrations)
{
  LoadStandardsConcentrations* ptrLoadStandardsConcentrations = nullptr;
  LoadStandardsConcentrations* nullPointerLoadStandardsConcentrations = nullptr;
  EXPECT_EQ(ptrLoadStandardsConcentrations, nullPointerLoadStandardsConcentrations);
}

TEST(SequenceSegmentProcessor, destructorLoadStandardsConcentrations)
{
  LoadStandardsConcentrations* ptrLoadStandardsConcentrations = nullptr;
  ptrLoadStandardsConcentrations = new LoadStandardsConcentrations();
  delete ptrLoadStandardsConcentrations;
}

TEST(SequenceSegmentProcessor, gettersLoadStandardsConcentrations)
{
  LoadStandardsConcentrations processor;
  EXPECT_EQ(processor.getName(), "LOAD_STANDARDS_CONCENTRATIONS");
}

TEST(SequenceSegmentProcessor, processLoadStandardsConcentrations)
{
  Filenames filenames;
  filenames.setFullPath("standardsConcentrations", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv"));
  SequenceSegmentHandler ssh;
  LoadStandardsConcentrations loadStandardsConcentrations;
  loadStandardsConcentrations.process(ssh, SequenceHandler(), {}, filenames);
  const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& rc = ssh.getStandardsConcentrations();

  EXPECT_EQ(rc.size(), 8);

  EXPECT_EQ(rc[0].sample_name, "150516_CM1_Level1");
  EXPECT_EQ(rc[0].component_name, "23dpg.23dpg_1.Light");
  EXPECT_EQ(rc[0].IS_component_name, "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(rc[0].actual_concentration, 0.0, 1e-6);
  EXPECT_NEAR(rc[0].IS_actual_concentration, 1.0, 1e-6);
  EXPECT_EQ(rc[0].concentration_units, "uM");
  EXPECT_NEAR(rc[0].dilution_factor, 1.0, 1e-6);

  EXPECT_EQ(rc[4].sample_name, "150516_CM3_Level9");
  EXPECT_EQ(rc[4].component_name, "ump.ump_2.Light");
  EXPECT_EQ(rc[4].IS_component_name, "ump.ump_1.Heavy");
  EXPECT_NEAR(rc[4].actual_concentration, 0.016, 1e-6);
  EXPECT_NEAR(rc[4].IS_actual_concentration, 1.0, 1e-6);
  EXPECT_EQ(rc[4].concentration_units, "uM");
  EXPECT_NEAR(rc[4].dilution_factor, 1.0, 1e-6);

  EXPECT_EQ(rc[7].sample_name, "150516_CM3_Level9");
  EXPECT_EQ(rc[7].component_name, "utp.utp_2.Light");
  EXPECT_EQ(rc[7].IS_component_name, "utp.utp_1.Heavy");
  EXPECT_NEAR(rc[7].actual_concentration, 0.0, 1e-6);
  EXPECT_NEAR(rc[7].IS_actual_concentration, 1.0, 1e-6);
  EXPECT_EQ(rc[7].concentration_units, "uM");
  EXPECT_NEAR(rc[7].dilution_factor, 1.0, 1e-6);
}

TEST(SequenceSegmentProcessor, processLoadStandardsConcentrationsWrongHeaders)
{
  Filenames filenames;
  filenames.setFullPath("standardsConcentrations", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_wrong.csv"));
  SequenceSegmentHandler ssh;
  LoadStandardsConcentrations loadStandardsConcentrations;
  try
  {
    loadStandardsConcentrations.process(ssh, SequenceHandler(), {}, filenames);
    FAIL() << "loadStandardsConcentrations() should throw an error\n";
  }
  catch (const std::invalid_argument& exception)
  {
    std::ostringstream expected_message;
    expected_message << "Missing headers in file \"" << SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_wrong.csv") << "\"";
    EXPECT_EQ(std::string(exception.what()), expected_message.str());
  }
  catch (...)
  {
    FAIL() << "ERROR: Unexpected exception thrown: " << std::current_exception << std::endl;
  }
}

/**
  LoadQuantitationMethods Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadQuantitationMethods)
{
  LoadQuantitationMethods* ptrLoadQuantitationMethods = nullptr;
  LoadQuantitationMethods* nullPointerLoadQuantitationMethods = nullptr;
  EXPECT_EQ(ptrLoadQuantitationMethods, nullPointerLoadQuantitationMethods);
}

TEST(SequenceSegmentProcessor, destructorLoadQuantitationMethods)
{
  LoadQuantitationMethods* ptrLoadQuantitationMethods = nullptr;
  ptrLoadQuantitationMethods = new LoadQuantitationMethods();
  delete ptrLoadQuantitationMethods;
}

TEST(SequenceSegmentProcessor, gettersLoadQuantitationMethods)
{
  LoadQuantitationMethods processor;
  EXPECT_EQ(processor.getName(), "LOAD_QUANTITATION_METHODS");
}

TEST(SequenceSegmentProcessor, processLoadQuantitationMethods)
{
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv"));
  SequenceSegmentHandler ssh;
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(ssh, SequenceHandler(), {}, filenames);
  const std::vector<OpenMS::AbsoluteQuantitationMethod>& aqm = ssh.getQuantitationMethods();

  EXPECT_EQ(aqm.size(), 107);

  EXPECT_EQ(aqm[0].getComponentName(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(aqm[0].getFeatureName(), "peak_apex_int");
  EXPECT_EQ(aqm[0].getISName(), "23dpg.23dpg_1.Heavy");
  EXPECT_EQ(aqm[0].getConcentrationUnits(), "uM");
  EXPECT_EQ(aqm[0].getTransformationModel(), "linear");
  EXPECT_NEAR(aqm[0].getLLOD(), 0.0, 1e-6);
  EXPECT_NEAR(aqm[0].getULOD(), 0.0, 1e-6);
  EXPECT_NEAR(aqm[0].getLLOQ(), 0.25, 1e-6);
  EXPECT_NEAR(aqm[0].getULOQ(), 2.5, 1e-6);
  EXPECT_NEAR(aqm[0].getCorrelationCoefficient(), 0.983846949, 1e-6);
  EXPECT_EQ(aqm[0].getNPoints(), 4);
  const OpenMS::Param params1 = aqm[0].getTransformationModelParams();
  EXPECT_NEAR(static_cast<double>(params1.getValue("slope")), 2.429728323, 1e-6);
  EXPECT_NEAR(static_cast<double>(params1.getValue("intercept")), -0.091856745, 1e-6);

  EXPECT_EQ(aqm[106].getComponentName(), "xan.xan_1.Light");
  EXPECT_EQ(aqm[106].getFeatureName(), "peak_apex_int");
  EXPECT_EQ(aqm[106].getISName(), "xan.xan_1.Heavy");
  EXPECT_EQ(aqm[106].getConcentrationUnits(), "uM");
  EXPECT_EQ(aqm[106].getTransformationModel(), "linear");
  EXPECT_NEAR(aqm[106].getLLOD(), 0.0, 1e-6);
  EXPECT_NEAR(aqm[106].getULOD(), 0.0, 1e-6);
  EXPECT_NEAR(aqm[106].getLLOQ(), 0.004, 1e-6);
  EXPECT_NEAR(aqm[106].getULOQ(), 0.16, 1e-6);
  EXPECT_NEAR(aqm[106].getCorrelationCoefficient(), 0.994348761, 1e-6);
  EXPECT_EQ(aqm[106].getNPoints(), 6);
  const OpenMS::Param params2 = aqm[106].getTransformationModelParams();
  EXPECT_NEAR(static_cast<double>(params2.getValue("slope")), 1.084995619, 1e-6);
  EXPECT_NEAR(static_cast<double>(params2.getValue("intercept")), -0.00224781, 1e-6);
}

TEST(SequenceSegmentProcessor, processLoadQuantitationMethodsWrongHeaders)
{
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_wrong.csv"));
  SequenceSegmentHandler ssh;
  LoadQuantitationMethods loadQuantitationMethods;
  try
  {
    loadQuantitationMethods.process(ssh, SequenceHandler(), {}, filenames);
    FAIL() << "loadQuantitationMethods() should throw an error\n";
  }
  catch (const std::invalid_argument& exception)
  {
    std::ostringstream expected_message;
    expected_message << "Missing headers in file \"" << SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_wrong.csv") << "\"";
    EXPECT_EQ(std::string(exception.what()), expected_message.str());
  }
  catch (...)
  {
    FAIL() << "ERROR: Unexpected exception thrown: " << std::current_exception << std::endl;
  }
}

/**
  StoreQuantitationMethods Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreQuantitationMethods)
{
  StoreQuantitationMethods* ptrStoreQuantitationMethods = nullptr;
  StoreQuantitationMethods* nullPointerStoreQuantitationMethods = nullptr;
  EXPECT_EQ(ptrStoreQuantitationMethods, nullPointerStoreQuantitationMethods);
}

TEST(SequenceSegmentProcessor, destructorStoreQuantitationMethods)
{
  StoreQuantitationMethods* ptrStoreQuantitationMethods = nullptr;
  ptrStoreQuantitationMethods = new StoreQuantitationMethods();
  delete ptrStoreQuantitationMethods;
}

TEST(SequenceSegmentProcessor, gettersStoreQuantitationMethods)
{
  StoreQuantitationMethods processor;
  EXPECT_EQ(processor.getName(), "STORE_QUANTITATION_METHODS");
}

TEST(SequenceSegmentProcessor, processStoreQuantitationMethods)
{
  // no tests, it wraps OpenMS store function
}

/**
  LoadFeatureFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureFilters)
{
  LoadFeatureFilters* ptrLoadFeatureFilters = nullptr;
  LoadFeatureFilters* nullPointerLoadFeatureFilters = nullptr;
  EXPECT_EQ(ptrLoadFeatureFilters, nullPointerLoadFeatureFilters);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureFilters)
{
  LoadFeatureFilters* ptrLoadFeatureFilters = nullptr;
  ptrLoadFeatureFilters = new LoadFeatureFilters();
  delete ptrLoadFeatureFilters;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureFilters)
{
  LoadFeatureFilters processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_FILTERS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureFilters)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureFilters loadFeatureFilters;
  loadFeatureFilters.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureFilter();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureQCs)
{
  LoadFeatureQCs* ptrLoadFeatureQCs = nullptr;
  LoadFeatureQCs* nullPointerLoadFeatureQCs = nullptr;
  EXPECT_EQ(ptrLoadFeatureQCs, nullPointerLoadFeatureQCs);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureQCs)
{
  LoadFeatureQCs* ptrLoadFeatureQCs = nullptr;
  ptrLoadFeatureQCs = new LoadFeatureQCs();
  delete ptrLoadFeatureQCs;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureQCs)
{
  LoadFeatureQCs processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_QCS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureQCs)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureQCs loadFeatureQCs;
  loadFeatureQCs.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureQC();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureFilters)
{
  StoreFeatureFilters* ptrStoreFeatureFilters = nullptr;
  StoreFeatureFilters* nullPointerStoreFeatureFilters = nullptr;
  EXPECT_EQ(ptrStoreFeatureFilters, nullPointerStoreFeatureFilters);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureFilters)
{
  StoreFeatureFilters* ptrStoreFeatureFilters = nullptr;
  ptrStoreFeatureFilters = new StoreFeatureFilters();
  delete ptrStoreFeatureFilters;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureFilters)
{
  StoreFeatureFilters processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_FILTERS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureFilters)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureFilters loadFeatureFilters;
  loadFeatureFilters.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureFilters storeFeatureFilters;
  storeFeatureFilters.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureFilters.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureFilter();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureFilter();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureQCs)
{
  StoreFeatureQCs* ptrStoreFeatureQCs = nullptr;
  StoreFeatureQCs* nullPointerStoreFeatureQCs = nullptr;
  EXPECT_EQ(ptrStoreFeatureQCs, nullPointerStoreFeatureQCs);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureQCs)
{
  StoreFeatureQCs* ptrStoreFeatureQCs = nullptr;
  ptrStoreFeatureQCs = new StoreFeatureQCs();
  delete ptrStoreFeatureQCs;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureQCs)
{
  StoreFeatureQCs processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_QCS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureQCs)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureQCs loadFeatureQCs;
  loadFeatureQCs.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureQCs storeFeatureQCs;
  storeFeatureQCs.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureQCs.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureQC();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureQC();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadFeatureRSDFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureRSDFilters)
{
  LoadFeatureRSDFilters* ptrLoadFeatureRSDFilters = nullptr;
  LoadFeatureRSDFilters* nullPointerLoadFeatureRSDFilters = nullptr;
  EXPECT_EQ(ptrLoadFeatureRSDFilters, nullPointerLoadFeatureRSDFilters);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureRSDFilters)
{
  LoadFeatureRSDFilters* ptrLoadFeatureRSDFilters = nullptr;
  ptrLoadFeatureRSDFilters = new LoadFeatureRSDFilters();
  delete ptrLoadFeatureRSDFilters;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureRSDFilters)
{
  LoadFeatureRSDFilters processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_RSD_FILTERS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureRSDFilters)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureRSDFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureRSDFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureRSDFilters loadFeatureRSDFilters;
  loadFeatureRSDFilters.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDFilter();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureRSDQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureRSDQCs)
{
  LoadFeatureRSDQCs* ptrLoadFeatureRSDQCs = nullptr;
  LoadFeatureRSDQCs* nullPointerLoadFeatureRSDQCs = nullptr;
  EXPECT_EQ(ptrLoadFeatureRSDQCs, nullPointerLoadFeatureRSDQCs);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureRSDQCs)
{
  LoadFeatureRSDQCs* ptrLoadFeatureRSDQCs = nullptr;
  ptrLoadFeatureRSDQCs = new LoadFeatureRSDQCs();
  delete ptrLoadFeatureRSDQCs;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureRSDQCs)
{
  LoadFeatureRSDQCs processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_RSD_QCS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureRSDQCs)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureRSDQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureRSDQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureRSDQCs loadFeatureRSDQCs;
  loadFeatureRSDQCs.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDQC();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureRSDFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureRSDFilters)
{
  StoreFeatureRSDFilters* ptrStoreFeatureRSDFilters = nullptr;
  StoreFeatureRSDFilters* nullPointerStoreFeatureRSDFilters = nullptr;
  EXPECT_EQ(ptrStoreFeatureRSDFilters, nullPointerStoreFeatureRSDFilters);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureRSDFilters)
{
  StoreFeatureRSDFilters* ptrStoreFeatureRSDFilters = nullptr;
  ptrStoreFeatureRSDFilters = new StoreFeatureRSDFilters();
  delete ptrStoreFeatureRSDFilters;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureRSDFilters)
{
  StoreFeatureRSDFilters processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_RSD_FILTERS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureRSDFilters)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureRSDFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureRSDFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureRSDFilters loadFeatureRSDFilters;
  loadFeatureRSDFilters.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureRSDFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureRSDFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureRSDFilters storeFeatureRSDFilters;
  storeFeatureRSDFilters.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureRSDFilters.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDFilter();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureRSDFilter();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureRSDQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureRSDQCs)
{
  StoreFeatureRSDQCs* ptrStoreFeatureRSDQCs = nullptr;
  StoreFeatureRSDQCs* nullPointerStoreFeatureRSDQCs = nullptr;
  EXPECT_EQ(ptrStoreFeatureRSDQCs, nullPointerStoreFeatureRSDQCs);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureRSDQCs)
{
  StoreFeatureRSDQCs* ptrStoreFeatureRSDQCs = nullptr;
  ptrStoreFeatureRSDQCs = new StoreFeatureRSDQCs();
  delete ptrStoreFeatureRSDQCs;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureRSDQCs)
{
  StoreFeatureRSDQCs processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_RSD_QCS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureRSDQCs)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureRSDQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureRSDQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureRSDQCs loadFeatureRSDQCs;
  loadFeatureRSDQCs.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureRSDQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureRSDQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureRSDQCs storeFeatureRSDQCs;
  storeFeatureRSDQCs.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureRSDQCs.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDQC();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureRSDQC();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadFeatureBackgroundFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureBackgroundFilters)
{
  LoadFeatureBackgroundFilters* ptrLoadFeatureBackgroundFilters = nullptr;
  LoadFeatureBackgroundFilters* nullPointerLoadFeatureBackgroundFilters = nullptr;
  EXPECT_EQ(ptrLoadFeatureBackgroundFilters, nullPointerLoadFeatureBackgroundFilters);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureBackgroundFilters)
{
  LoadFeatureBackgroundFilters* ptrLoadFeatureBackgroundFilters = nullptr;
  ptrLoadFeatureBackgroundFilters = new LoadFeatureBackgroundFilters();
  delete ptrLoadFeatureBackgroundFilters;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureBackgroundFilters)
{
  LoadFeatureBackgroundFilters processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_BACKGROUND_FILTERS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureBackgroundFilters)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureBackgroundFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureBackgroundFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
  loadFeatureBackgroundFilters.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundFilter();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureBackgroundQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureBackgroundQCs)
{
  LoadFeatureBackgroundQCs* ptrLoadFeatureBackgroundQCs = nullptr;
  LoadFeatureBackgroundQCs* nullPointerLoadFeatureBackgroundQCs = nullptr;
  EXPECT_EQ(ptrLoadFeatureBackgroundQCs, nullPointerLoadFeatureBackgroundQCs);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureBackgroundQCs)
{
  LoadFeatureBackgroundQCs* ptrLoadFeatureBackgroundQCs = nullptr;
  ptrLoadFeatureBackgroundQCs = new LoadFeatureBackgroundQCs();
  delete ptrLoadFeatureBackgroundQCs;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureBackgroundQCs)
{
  LoadFeatureBackgroundQCs processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_BACKGROUND_QCS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureBackgroundQCs)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureBackgroundQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureBackgroundQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
  loadFeatureBackgroundQCs.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundQC();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureBackgroundFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureBackgroundFilters)
{
  StoreFeatureBackgroundFilters* ptrStoreFeatureBackgroundFilters = nullptr;
  StoreFeatureBackgroundFilters* nullPointerStoreFeatureBackgroundFilters = nullptr;
  EXPECT_EQ(ptrStoreFeatureBackgroundFilters, nullPointerStoreFeatureBackgroundFilters);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureBackgroundFilters)
{
  StoreFeatureBackgroundFilters* ptrStoreFeatureBackgroundFilters = nullptr;
  ptrStoreFeatureBackgroundFilters = new StoreFeatureBackgroundFilters();
  delete ptrStoreFeatureBackgroundFilters;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureBackgroundFilters)
{
  StoreFeatureBackgroundFilters processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_BACKGROUND_FILTERS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureBackgroundFilters)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureBackgroundFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureBackgroundFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
  loadFeatureBackgroundFilters.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureBackgroundFilterComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureBackgroundFilterComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureBackgroundFilters storeFeatureBackgroundFilters;
  storeFeatureBackgroundFilters.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureBackgroundFilters.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundFilter();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureBackgroundFilter();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureBackgroundQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureBackgroundQCs)
{
  StoreFeatureBackgroundQCs* ptrStoreFeatureBackgroundQCs = nullptr;
  StoreFeatureBackgroundQCs* nullPointerStoreFeatureBackgroundQCs = nullptr;
  EXPECT_EQ(ptrStoreFeatureBackgroundQCs, nullPointerStoreFeatureBackgroundQCs);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureBackgroundQCs)
{
  StoreFeatureBackgroundQCs* ptrStoreFeatureBackgroundQCs = nullptr;
  ptrStoreFeatureBackgroundQCs = new StoreFeatureBackgroundQCs();
  delete ptrStoreFeatureBackgroundQCs;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureBackgroundQCs)
{
  StoreFeatureBackgroundQCs processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_BACKGROUND_QCS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureBackgroundQCs)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureBackgroundQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureBackgroundQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
  loadFeatureBackgroundQCs.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureBackgroundQCComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureBackgroundQCComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureBackgroundQCs storeFeatureBackgroundQCs;
  storeFeatureBackgroundQCs.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureBackgroundQCs.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundQC();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureBackgroundQC();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  EstimateFeatureFilterValues Tests
*/
TEST(SequenceSegmentProcessor, constructorEstimateFeatureFilterValues)
{
  EstimateFeatureFilterValues* ptrEstimateFeatureFilterValues = nullptr;
  EstimateFeatureFilterValues* nullPointerEstimateFeatureFilterValues = nullptr;
  EXPECT_EQ(ptrEstimateFeatureFilterValues, nullPointerEstimateFeatureFilterValues);
}

TEST(SequenceSegmentProcessor, destructorEstimateFeatureFilterValues)
{
  EstimateFeatureFilterValues* ptrEstimateFeatureFilterValues = nullptr;
  ptrEstimateFeatureFilterValues = new EstimateFeatureFilterValues();
  delete ptrEstimateFeatureFilterValues;
}

TEST(SequenceSegmentProcessor, gettersEstimateFeatureFilterValues)
{
  EstimateFeatureFilterValues processor;
  EXPECT_EQ(processor.getName(), "ESTIMATE_FEATURE_FILTER_VALUES");
}

TEST(SequenceSegmentProcessor, processEstimateFeatureFilterValues)
{
  // Make the test sequence
  SequenceHandler sequenceHandler;
  addStandardsFeatures(sequenceHandler);
  addBlanksFeatures(sequenceHandler);
  addQCFeatures(sequenceHandler);

  // Make the transition list
  OpenMS::TargetedExperiment transitions;
  OpenMS::ReactionMonitoringTransition transition;
  transition.setNativeID("ser-L.ser-L_1.Light"); // transition group 1
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("ser-L.ser-L_1.Heavy");
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Light"); // transition group 2
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Heavy");
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Light"); // transition group 3
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Heavy");
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  sequenceHandler.getSequence().front().getRawData().setTargetedExperiment(transitions);

  // Make the template FeatureQC
  OpenMS::MRMFeatureQC feature_filter_template;
  feature_filter_template.component_group_qcs.resize(3);
  feature_filter_template.component_group_qcs.at(0).component_group_name = "ser-L";
  feature_filter_template.component_group_qcs.at(1).component_group_name = "amp";
  feature_filter_template.component_group_qcs.at(2).component_group_name = "atp";
  feature_filter_template.component_qcs.resize(6);
  feature_filter_template.component_qcs.at(0).component_name = "ser-L.ser-L_1.Light";
  feature_filter_template.component_qcs.at(0).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(1).component_name = "ser-L.ser-L_1.Heavy";
  feature_filter_template.component_qcs.at(1).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(2).component_name = "amp.amp_1.Light";
  feature_filter_template.component_qcs.at(2).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(3).component_name = "amp.amp_1.Heavy";
  feature_filter_template.component_qcs.at(3).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(4).component_name = "atp.atp_1.Light";
  feature_filter_template.component_qcs.at(4).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(5).component_name = "atp.atp_1.Heavy";
  feature_filter_template.component_qcs.at(5).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  sequenceHandler.getSequenceSegments().front().setFeatureFilter(feature_filter_template);

  // Make other constructs
  const ParameterSet params;
  Filenames filenames;

  // Process
  EstimateFeatureFilterValues processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureFilter();
  // Ser-L
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_heavy_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_heavy_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_light_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_light_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_detecting_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_detecting_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_quantifying_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_quantifying_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_transitions_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_transitions_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).ion_ratio_u, 1000000000000, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(0).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_l, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_u, 2230000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").second, 2230000, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(1).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_l, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_u, 24500, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").second, 24500, 1e-4);
  // Amp
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_heavy_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_heavy_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_light_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_light_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_detecting_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_detecting_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_quantifying_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_quantifying_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_transitions_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_transitions_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(1).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(1).ion_ratio_u, 1000000000000, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(2).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_l, 440, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_u, 3640000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").first, 440, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").second, 3640000, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(3).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_l, 215000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_u, 462000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").first, 215000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").second, 462000, 1e-4);
  // Atp
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_heavy_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_heavy_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_light_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_light_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_detecting_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_detecting_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_quantifying_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_quantifying_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_transitions_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_transitions_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(2).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(2).ion_ratio_u, 1000000000000, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(4).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_l, 221, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_u, 4190000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").first, 221, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").second, 4190000, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(5).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_l, 828, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_u, 322000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").first, 828, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").second, 322000, 1e-4);
}

/**
  EstimateFeatureQCValues Tests
*/
TEST(SequenceSegmentProcessor, constructorEstimateFeatureQCValues)
{
  EstimateFeatureQCValues* ptrEstimateFeatureQCValues = nullptr;
  EstimateFeatureQCValues* nullPointerEstimateFeatureQCValues = nullptr;
  EXPECT_EQ(ptrEstimateFeatureQCValues, nullPointerEstimateFeatureQCValues);
}

TEST(SequenceSegmentProcessor, destructorEstimateFeatureQCValues)
{
  EstimateFeatureQCValues* ptrEstimateFeatureQCValues = nullptr;
  ptrEstimateFeatureQCValues = new EstimateFeatureQCValues();
  delete ptrEstimateFeatureQCValues;
}

TEST(SequenceSegmentProcessor, gettersEstimateFeatureQCValues)
{
  EstimateFeatureQCValues processor;
  EXPECT_EQ(processor.getName(), "ESTIMATE_FEATURE_QC_VALUES");
}

TEST(SequenceSegmentProcessor, processEstimateFeatureQCValues)
{
  // Make the test sequence
  SequenceHandler sequenceHandler;
  addStandardsFeatures(sequenceHandler);
  addBlanksFeatures(sequenceHandler);
  addQCFeatures(sequenceHandler);

  // Make the transition list
  OpenMS::TargetedExperiment transitions;
  OpenMS::ReactionMonitoringTransition transition;
  transition.setNativeID("ser-L.ser-L_1.Light"); // transition group 1
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("ser-L.ser-L_1.Heavy");
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Light"); // transition group 2
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Heavy");
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Light"); // transition group 3
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Heavy");
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  sequenceHandler.getSequence().front().getRawData().setTargetedExperiment(transitions);

  // Make the template FeatureQC
  OpenMS::MRMFeatureQC feature_filter_template;
  feature_filter_template.component_group_qcs.resize(3);
  feature_filter_template.component_group_qcs.at(0).component_group_name = "ser-L";
  feature_filter_template.component_group_qcs.at(1).component_group_name = "amp";
  feature_filter_template.component_group_qcs.at(2).component_group_name = "atp";
  feature_filter_template.component_qcs.resize(6);
  feature_filter_template.component_qcs.at(0).component_name = "ser-L.ser-L_1.Light";
  feature_filter_template.component_qcs.at(0).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(1).component_name = "ser-L.ser-L_1.Heavy";
  feature_filter_template.component_qcs.at(1).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(2).component_name = "amp.amp_1.Light";
  feature_filter_template.component_qcs.at(2).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(3).component_name = "amp.amp_1.Heavy";
  feature_filter_template.component_qcs.at(3).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(4).component_name = "atp.atp_1.Light";
  feature_filter_template.component_qcs.at(4).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(5).component_name = "atp.atp_1.Heavy";
  feature_filter_template.component_qcs.at(5).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  sequenceHandler.getSequenceSegments().front().setFeatureQC(feature_filter_template);

  // Make other constructs
  const ParameterSet params;
  Filenames filenames;

  // Process
  EstimateFeatureQCValues processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureQC();
  // Ser-L
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_heavy_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_heavy_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_light_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_light_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_detecting_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_detecting_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_quantifying_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_quantifying_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_transitions_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_transitions_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).ion_ratio_u, 1000000000000, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(0).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_l, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_u, 2230000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").second, 2230000, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(1).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_l, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_u, 24500, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").second, 24500, 1e-4);
  // Amp
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_heavy_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_heavy_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_light_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_light_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_detecting_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_detecting_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_quantifying_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_quantifying_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_transitions_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_transitions_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(1).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(1).ion_ratio_u, 1000000000000, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(2).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_l, 440, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_u, 3640000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").first, 440, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").second, 3640000, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(3).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_l, 215000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_u, 462000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").first, 215000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").second, 462000, 1e-4);
  // Atp
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_heavy_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_heavy_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_light_l, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_light_u, 1);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_detecting_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_detecting_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_quantifying_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_quantifying_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_transitions_l, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_transitions_u, 2);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(2).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(2).ion_ratio_u, 1000000000000, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(4).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_l, 221, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_u, 4190000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").first, 221, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").second, 4190000, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(5).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_l, 828, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_u, 322000, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").first, 828, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").second, 322000, 1e-4);
}

/**
  TransferLOQToFeatureFilters Tests
*/
TEST(SequenceSegmentProcessor, constructorTransferLOQToFeatureFilters)
{
  TransferLOQToFeatureFilters* ptrTransferLOQToFeatureFilters = nullptr;
  TransferLOQToFeatureFilters* nullPointerTransferLOQToFeatureFilters = nullptr;
  EXPECT_EQ(ptrTransferLOQToFeatureFilters, nullPointerTransferLOQToFeatureFilters);
}

TEST(SequenceSegmentProcessor, destructorTransferLOQToFeatureFilters)
{
  TransferLOQToFeatureFilters* ptrTransferLOQToFeatureFilters = nullptr;
  ptrTransferLOQToFeatureFilters = new TransferLOQToFeatureFilters();
  delete ptrTransferLOQToFeatureFilters;
}

TEST(SequenceSegmentProcessor, gettersTransferLOQToFeatureFilters)
{
  TransferLOQToFeatureFilters processor;
  EXPECT_EQ(processor.getName(), "TRANSFER_LOQ_TO_FEATURE_FILTERS");
}

TEST(SequenceSegmentProcessor, processTransferLOQToFeatureFilters)
{
  // Make the test sequence
  SequenceHandler sequenceHandler;
  addStandardsFeatures(sequenceHandler);

  // Make the template FeatureQC
  OpenMS::MRMFeatureQC feature_filter_template;
  feature_filter_template.component_group_qcs.resize(3);
  feature_filter_template.component_group_qcs.at(0).component_group_name = "ser-L";
  feature_filter_template.component_group_qcs.at(1).component_group_name = "amp";
  feature_filter_template.component_group_qcs.at(2).component_group_name = "atp";
  feature_filter_template.component_qcs.resize(6);
  feature_filter_template.component_qcs.at(0).component_name = "ser-L.ser-L_1.Light";
  feature_filter_template.component_qcs.at(0).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(1).component_name = "ser-L.ser-L_1.Heavy";
  feature_filter_template.component_qcs.at(1).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(2).component_name = "amp.amp_1.Light";
  feature_filter_template.component_qcs.at(2).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(3).component_name = "amp.amp_1.Heavy";
  feature_filter_template.component_qcs.at(3).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(4).component_name = "atp.atp_1.Light";
  feature_filter_template.component_qcs.at(4).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(5).component_name = "atp.atp_1.Heavy";
  feature_filter_template.component_qcs.at(5).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  sequenceHandler.getSequenceSegments().front().setFeatureFilter(feature_filter_template);

  // Make the QuantitationMethods
  vector<OpenMS::AbsoluteQuantitationMethod> quant_methods;
  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("ser-L.ser-L_1.Light");
  aqm.setISName("ser-L.ser-L_1.Heavy");
  aqm.setLLOQ(100);
  aqm.setULOQ(1e4);
  quant_methods.push_back(aqm);
  aqm.setComponentName("amp.amp_1.Light");
  aqm.setISName("amp.amp_1.Heavy");
  aqm.setLLOQ(10);
  aqm.setULOQ(1e5);
  quant_methods.push_back(aqm);
  aqm.setComponentName("atp.atp_1.Light");
  aqm.setISName("atp.atp_1.Heavy");
  aqm.setLLOQ(1);
  aqm.setULOQ(1e6);
  quant_methods.push_back(aqm);
  sequenceHandler.getSequenceSegments().front().setQuantitationMethods(quant_methods);

  // Make other constructs
  const ParameterSet params;
  Filenames filenames;

  // Process
  TransferLOQToFeatureFilters processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureFilter();
  // Ser-L
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  EXPECT_EQ(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("calculated_concentration").first, 100, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("calculated_concentration").second, 1e4, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("calculated_concentration").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("calculated_concentration").second, 5000, 1e-4);
  // Amp
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  EXPECT_EQ(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("calculated_concentration").first, 10, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("calculated_concentration").second, 1e5, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("calculated_concentration").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("calculated_concentration").second, 5000, 1e-4);
  // Atp
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  EXPECT_EQ(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("calculated_concentration").first, 1, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("calculated_concentration").second, 1e6, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("calculated_concentration").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("calculated_concentration").second, 5000, 1e-4);
}

/**
  TransferLOQToFeatureQCs Tests
*/
TEST(SequenceSegmentProcessor, constructorTransferLOQToFeatureQCs)
{
  TransferLOQToFeatureQCs* ptrTransferLOQToFeatureQCs = nullptr;
  TransferLOQToFeatureQCs* nullPointerTransferLOQToFeatureQCs = nullptr;
  EXPECT_EQ(ptrTransferLOQToFeatureQCs, nullPointerTransferLOQToFeatureQCs);
}

TEST(SequenceSegmentProcessor, destructorTransferLOQToFeatureQCs)
{
  TransferLOQToFeatureQCs* ptrTransferLOQToFeatureQCs = nullptr;
  ptrTransferLOQToFeatureQCs = new TransferLOQToFeatureQCs();
  delete ptrTransferLOQToFeatureQCs;
}

TEST(SequenceSegmentProcessor, gettersTransferLOQToFeatureQCs)
{
  TransferLOQToFeatureQCs processor;
  EXPECT_EQ(processor.getName(), "TRANSFER_LOQ_TO_FEATURE_QCS");
}

TEST(SequenceSegmentProcessor, processTransferLOQToFeatureQCs)
{
  // Make the test sequence
  SequenceHandler sequenceHandler;
  addStandardsFeatures(sequenceHandler);

  // Make the template FeatureQC
  OpenMS::MRMFeatureQC feature_filter_template;
  feature_filter_template.component_group_qcs.resize(3);
  feature_filter_template.component_group_qcs.at(0).component_group_name = "ser-L";
  feature_filter_template.component_group_qcs.at(1).component_group_name = "amp";
  feature_filter_template.component_group_qcs.at(2).component_group_name = "atp";
  feature_filter_template.component_qcs.resize(6);
  feature_filter_template.component_qcs.at(0).component_name = "ser-L.ser-L_1.Light";
  feature_filter_template.component_qcs.at(0).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(1).component_name = "ser-L.ser-L_1.Heavy";
  feature_filter_template.component_qcs.at(1).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(2).component_name = "amp.amp_1.Light";
  feature_filter_template.component_qcs.at(2).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(3).component_name = "amp.amp_1.Heavy";
  feature_filter_template.component_qcs.at(3).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(4).component_name = "atp.atp_1.Light";
  feature_filter_template.component_qcs.at(4).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(5).component_name = "atp.atp_1.Heavy";
  feature_filter_template.component_qcs.at(5).meta_value_qc.emplace("calculated_concentration", std::make_pair(0.0, 5000.0));
  sequenceHandler.getSequenceSegments().front().setFeatureQC(feature_filter_template);

  // Make the QuantitationMethods
  vector<OpenMS::AbsoluteQuantitationMethod> quant_methods;
  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("ser-L.ser-L_1.Light");
  aqm.setISName("ser-L.ser-L_1.Heavy");
  aqm.setLLOQ(100);
  aqm.setULOQ(1e4);
  quant_methods.push_back(aqm);
  aqm.setComponentName("amp.amp_1.Light");
  aqm.setISName("amp.amp_1.Heavy");
  aqm.setLLOQ(10);
  aqm.setULOQ(1e5);
  quant_methods.push_back(aqm);
  aqm.setComponentName("atp.atp_1.Light");
  aqm.setISName("atp.atp_1.Heavy");
  aqm.setLLOQ(1);
  aqm.setULOQ(1e6);
  quant_methods.push_back(aqm);
  sequenceHandler.getSequenceSegments().front().setQuantitationMethods(quant_methods);

  // Make other constructs
  const ParameterSet params;
  Filenames filenames;

  // Process
  TransferLOQToFeatureQCs processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureQC();
  // Ser-L
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  EXPECT_EQ(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("calculated_concentration").first, 100, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("calculated_concentration").second, 1e4, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("calculated_concentration").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("calculated_concentration").second, 5000, 1e-4);
  // Amp
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  EXPECT_EQ(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("calculated_concentration").first, 10, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("calculated_concentration").second, 1e5, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("calculated_concentration").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("calculated_concentration").second, 5000, 1e-4);
  // Atp
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  EXPECT_EQ(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("calculated_concentration").first, 1, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("calculated_concentration").second, 1e6, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("calculated_concentration").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("calculated_concentration").second, 5000, 1e-4);
}

/**
  EstimateFeatureRSDs Tests
*/
TEST(SequenceSegmentProcessor, constructorEstimateFeatureRSDs)
{
  EstimateFeatureRSDs* ptrEstimateFeatureRSDs = nullptr;
  EstimateFeatureRSDs* nullPointerEstimateFeatureRSDs = nullptr;
  EXPECT_EQ(ptrEstimateFeatureRSDs, nullPointerEstimateFeatureRSDs);
}

TEST(SequenceSegmentProcessor, destructorEstimateFeatureRSDs)
{
  EstimateFeatureRSDs* ptrEstimateFeatureRSDs = nullptr;
  ptrEstimateFeatureRSDs = new EstimateFeatureRSDs();
  delete ptrEstimateFeatureRSDs;
}

TEST(SequenceSegmentProcessor, gettersEstimateFeatureRSDs)
{
  EstimateFeatureRSDs processor;
  EXPECT_EQ(processor.getName(), "ESTIMATE_FEATURE_RSDS");
}

TEST(SequenceSegmentProcessor, processEstimateFeatureRSDs)
{
  // Make the test sequence
  SequenceHandler sequenceHandler;
  addStandardsFeatures(sequenceHandler);
  addBlanksFeatures(sequenceHandler);
  addQCFeatures(sequenceHandler);

  // Make the transition list
  OpenMS::TargetedExperiment transitions;
  OpenMS::ReactionMonitoringTransition transition;
  transition.setNativeID("ser-L.ser-L_1.Light"); // transition group 1
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("ser-L.ser-L_1.Heavy");
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Light"); // transition group 2
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Heavy");
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Light"); // transition group 3
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Heavy");
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  sequenceHandler.getSequence().front().getRawData().setTargetedExperiment(transitions);

  // Make the template FeatureQC
  OpenMS::MRMFeatureQC feature_filter_template;
  feature_filter_template.component_group_qcs.resize(3);
  feature_filter_template.component_group_qcs.at(0).component_group_name = "ser-L";
  feature_filter_template.component_group_qcs.at(1).component_group_name = "amp";
  feature_filter_template.component_group_qcs.at(2).component_group_name = "atp";
  feature_filter_template.component_qcs.resize(6);
  feature_filter_template.component_qcs.at(0).component_name = "ser-L.ser-L_1.Light";
  feature_filter_template.component_qcs.at(0).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(1).component_name = "ser-L.ser-L_1.Heavy";
  feature_filter_template.component_qcs.at(1).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(2).component_name = "amp.amp_1.Light";
  feature_filter_template.component_qcs.at(2).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(3).component_name = "amp.amp_1.Heavy";
  feature_filter_template.component_qcs.at(3).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(4).component_name = "atp.atp_1.Light";
  feature_filter_template.component_qcs.at(4).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  feature_filter_template.component_qcs.at(5).component_name = "atp.atp_1.Heavy";
  feature_filter_template.component_qcs.at(5).meta_value_qc.emplace("peak_apex_int", std::make_pair(0.0, 5000.0));
  sequenceHandler.getSequenceSegments().front().setFeatureRSDFilter(feature_filter_template);

  // Make other constructs
  const ParameterSet params;
  Filenames filenames;

  // Process
  EstimateFeatureRSDs processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureRSDEstimations();
  // Ser-L
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_heavy_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_heavy_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_light_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_light_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_detecting_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_detecting_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_quantifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_quantifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_transitions_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).n_transitions_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).ion_ratio_u, 0, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(0).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_u, 45.940820332935274, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").second, 45.940820332935274, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(1).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_u, 45.940820332935274, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").second, 45.940820332935274, 1e-4);
  // Amp
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_heavy_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_heavy_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_light_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_light_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_detecting_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_detecting_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_quantifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_quantifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_transitions_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).n_transitions_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(1).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(1).ion_ratio_u, 0, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(2).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_u, 21.980528755438982, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").second, 21.980528755438982, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(3).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_u, 21.980528755438982, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").second, 21.980528755438982, 1e-4);
  // Atp
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_heavy_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_heavy_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_light_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_light_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_detecting_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_detecting_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_quantifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_quantifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_identifying_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_identifying_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_transitions_l, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).n_transitions_u, 0);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_1, "");
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_2, "");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(2).ion_ratio_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(2).ion_ratio_u, 0, 1e-4);
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).ion_ratio_feature_name, "");
  EXPECT_EQ(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(4).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_u, 148.21486099460165, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").second, 148.21486099460165, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(5).retention_time_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).retention_time_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_u, 148.21486099460165, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).overall_quality_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).overall_quality_u, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").second, 148.21486099460165, 1e-4);
}


/**
  EstimateFeatureBackgroundInterferences Tests
*/
TEST(SequenceSegmentProcessor, constructorEstimateFeatureBackgroundInterferences)
{
  EstimateFeatureBackgroundInterferences* ptrEstimateFeatureBackgroundInterferences = nullptr;
  EstimateFeatureBackgroundInterferences* nullPointerEstimateFeatureBackgroundInterferences = nullptr;
  EXPECT_EQ(ptrEstimateFeatureBackgroundInterferences, nullPointerEstimateFeatureBackgroundInterferences);
}

TEST(SequenceSegmentProcessor, destructorEstimateFeatureBackgroundInterferences)
{
  EstimateFeatureBackgroundInterferences* ptrEstimateFeatureBackgroundInterferences = nullptr;
  ptrEstimateFeatureBackgroundInterferences = new EstimateFeatureBackgroundInterferences();
  delete ptrEstimateFeatureBackgroundInterferences;
}

TEST(SequenceSegmentProcessor, gettersEstimateFeatureBackgroundInterferences)
{
  EstimateFeatureBackgroundInterferences processor;
  EXPECT_EQ(processor.getName(), "ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES");
}

TEST(SequenceSegmentProcessor, processEstimateFeatureBackgroundInterferences)
{
  // Make the test sequence
  SequenceHandler sequenceHandler;
  addStandardsFeatures(sequenceHandler);
  addBlanksFeatures(sequenceHandler);
  addQCFeatures(sequenceHandler);

  // Make the transition list
  OpenMS::TargetedExperiment transitions;
  OpenMS::ReactionMonitoringTransition transition;
  transition.setNativeID("ser-L.ser-L_1.Light"); // transition group 1
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("ser-L.ser-L_1.Heavy");
  transition.setPeptideRef("ser-L");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Light"); // transition group 2
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("amp.amp_1.Heavy");
  transition.setPeptideRef("amp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Light"); // transition group 3
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  transition.setNativeID("atp.atp_1.Heavy");
  transition.setPeptideRef("atp");
  transition.setDetectingTransition(true);
  transition.setIdentifyingTransition(false);
  transition.setQuantifyingTransition(true);
  transitions.addTransition(transition);
  sequenceHandler.getSequence().front().getRawData().setTargetedExperiment(transitions);

  // Make the template FeatureQC
  OpenMS::MRMFeatureQC feature_filter_template;
  feature_filter_template.component_group_qcs.resize(3);
  feature_filter_template.component_group_qcs.at(0).component_group_name = "ser-L";
  feature_filter_template.component_group_qcs.at(1).component_group_name = "amp";
  feature_filter_template.component_group_qcs.at(2).component_group_name = "atp";
  feature_filter_template.component_qcs.resize(6);
  feature_filter_template.component_qcs.at(0).component_name = "ser-L.ser-L_1.Light";
  feature_filter_template.component_qcs.at(1).component_name = "ser-L.ser-L_1.Heavy";
  feature_filter_template.component_qcs.at(2).component_name = "amp.amp_1.Light";
  feature_filter_template.component_qcs.at(3).component_name = "amp.amp_1.Heavy";
  feature_filter_template.component_qcs.at(4).component_name = "atp.atp_1.Light";
  feature_filter_template.component_qcs.at(5).component_name = "atp.atp_1.Heavy";
  sequenceHandler.getSequenceSegments().front().setFeatureBackgroundFilter(feature_filter_template);

  // Make other constructs
  const ParameterSet params;
  Filenames filenames;

  // Process
  EstimateFeatureBackgroundInterferences processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureBackgroundEstimations();
  // Ser-L
  EXPECT_EQ(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).intensity_u, 400499.23928506032, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(0).intensity_u, 400349.32857131958, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(1).intensity_u, 149.91428593226843, 1e-4);
  // Amp
  EXPECT_EQ(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).intensity_u, 400499.23928506032, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(2).intensity_u, 5839.5428571360453, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(3).intensity_u, 3019.2857142857142, 1e-4);
  // Atp
  EXPECT_EQ(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_group_qcs.at(0).intensity_u, 400499.23928506032, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(4).intensity_u, 6859.2635714156286, 1e-4);
  EXPECT_EQ(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_l, 0, 1e-4);
  EXPECT_NEAR(feature_filters.component_qcs.at(5).intensity_u, 870.49857139587402, 1e-4);
}

/**
  StoreFeatureRSDEstimations Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureRSDEstimations)
{
  StoreFeatureRSDEstimations* ptrStoreFeatureRSDEstimations = nullptr;
  StoreFeatureRSDEstimations* nullPointerStoreFeatureRSDEstimations = nullptr;
  EXPECT_EQ(ptrStoreFeatureRSDEstimations, nullPointerStoreFeatureRSDEstimations);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureRSDEstimations)
{
  StoreFeatureRSDEstimations* ptrStoreFeatureRSDEstimations = nullptr;
  ptrStoreFeatureRSDEstimations = new StoreFeatureRSDEstimations();
  delete ptrStoreFeatureRSDEstimations;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureRSDEstimations)
{
  StoreFeatureRSDEstimations processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_RSD_ESTIMATIONS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureRSDEstimations)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureRSDEstimationComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureRSDEstimationComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureRSDEstimations loadFeatureRSDEstimations;
  loadFeatureRSDEstimations.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureRSDEstimationComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureRSDEstimationComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureRSDEstimations storeFeatureRSDEstimations;
  storeFeatureRSDEstimations.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureRSDEstimations.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDEstimations();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureRSDEstimations();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureBackgroundEstimations Tests
*/
TEST(SequenceSegmentProcessor, constructorStoreFeatureBackgroundEstimations)
{
  StoreFeatureBackgroundEstimations* ptrStoreFeatureBackgroundEstimations = nullptr;
  StoreFeatureBackgroundEstimations* nullPointerStoreFeatureBackgroundEstimations = nullptr;
  EXPECT_EQ(ptrStoreFeatureBackgroundEstimations, nullPointerStoreFeatureBackgroundEstimations);
}

TEST(SequenceSegmentProcessor, destructorStoreFeatureBackgroundEstimations)
{
  StoreFeatureBackgroundEstimations* ptrStoreFeatureBackgroundEstimations = nullptr;
  ptrStoreFeatureBackgroundEstimations = new StoreFeatureBackgroundEstimations();
  delete ptrStoreFeatureBackgroundEstimations;
}

TEST(SequenceSegmentProcessor, gettersStoreFeatureBackgroundEstimations)
{
  StoreFeatureBackgroundEstimations processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURE_BACKGROUND_ESTIMATIONS");
}

TEST(SequenceSegmentProcessor, processStoreFeatureBackgroundEstimations)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.setFullPath("featureBackgroundEstimationComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureBackgroundEstimationComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));
  LoadFeatureBackgroundEstimations loadFeatureBackgroundEstimations;
  loadFeatureBackgroundEstimations.process(ssh, SequenceHandler(), {}, filenames);
  filenames.setFullPath("featureBackgroundEstimationComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv"));
  filenames.setFullPath("featureBackgroundEstimationComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv"));
  StoreFeatureBackgroundEstimations storeFeatureBackgroundEstimations;
  storeFeatureBackgroundEstimations.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureBackgroundEstimations.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundEstimations();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureBackgroundEstimations();

  EXPECT_EQ(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  EXPECT_EQ(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    EXPECT_TRUE(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadFeatureRSDEstimations Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureRSDEstimations)
{
  LoadFeatureRSDEstimations* ptrLoadFeatureRSDEstimations = nullptr;
  LoadFeatureRSDEstimations* nullPointerLoadFeatureRSDEstimations = nullptr;
  EXPECT_EQ(ptrLoadFeatureRSDEstimations, nullPointerLoadFeatureRSDEstimations);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureRSDEstimations)
{
  LoadFeatureRSDEstimations* ptrLoadFeatureRSDEstimations = nullptr;
  ptrLoadFeatureRSDEstimations = new LoadFeatureRSDEstimations();
  delete ptrLoadFeatureRSDEstimations;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureRSDEstimations)
{
  LoadFeatureRSDEstimations processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_RSD_ESTIMATIONS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureRSDEstimations)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureRSDEstimationComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureRSDEstimationComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureRSDEstimations loadFeatureRSDEstimations;
  loadFeatureRSDEstimations.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDEstimations();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}


/**
  LoadFeatureBackgroundEstimations Tests
*/
TEST(SequenceSegmentProcessor, constructorLoadFeatureBackgroundEstimations)
{
  LoadFeatureBackgroundEstimations* ptrLoadFeatureBackgroundEstimations = nullptr;
  LoadFeatureBackgroundEstimations* nullPointerLoadFeatureBackgroundEstimations = nullptr;
  EXPECT_EQ(ptrLoadFeatureBackgroundEstimations, nullPointerLoadFeatureBackgroundEstimations);
}

TEST(SequenceSegmentProcessor, destructorLoadFeatureBackgroundEstimations)
{
  LoadFeatureBackgroundEstimations* ptrLoadFeatureBackgroundEstimations = nullptr;
  ptrLoadFeatureBackgroundEstimations = new LoadFeatureBackgroundEstimations();
  delete ptrLoadFeatureBackgroundEstimations;
}

TEST(SequenceSegmentProcessor, gettersLoadFeatureBackgroundEstimations)
{
  LoadFeatureBackgroundEstimations processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURE_BACKGROUND_ESTIMATIONS");
}

TEST(SequenceSegmentProcessor, processLoadFeatureBackgroundEstimations)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.setFullPath("featureBackgroundEstimationComponents", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv"));
  filenames.setFullPath("featureBackgroundEstimationComponentGroups", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv"));

  LoadFeatureBackgroundEstimations loadFeatureBackgroundEstimations;
  loadFeatureBackgroundEstimations.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundEstimations();

  EXPECT_EQ(fQC.component_qcs.size(), 324);
  EXPECT_EQ(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(fQC.component_group_qcs.size(), 118);
  EXPECT_EQ(fQC.component_group_qcs[0].component_group_name, "arg-L");
}
