// TODO: Add copyright

 #include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceSegmentProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/SampleType.h>

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

BOOST_AUTO_TEST_SUITE(sequencesegmentprocessor)

/**
  CalculateCalibration Tests
*/
BOOST_AUTO_TEST_CASE(constructorCalculateCalibration)
{
  CalculateCalibration* ptrCalculateCalibration = nullptr;
  CalculateCalibration* nullPointerCalculateCalibration = nullptr;
  BOOST_CHECK_EQUAL(ptrCalculateCalibration, nullPointerCalculateCalibration);
}

BOOST_AUTO_TEST_CASE(destructorCalculateCalibration)
{
  CalculateCalibration* ptrCalculateCalibration = nullptr;
  ptrCalculateCalibration = new CalculateCalibration();
  delete ptrCalculateCalibration;
}

BOOST_AUTO_TEST_CASE(gettersCalculateCalibration)
{
  CalculateCalibration processor;

  BOOST_CHECK_EQUAL(processor.getID(), 15);
  BOOST_CHECK_EQUAL(processor.getName(), "CALCULATE_CALIBRATION");
}

BOOST_AUTO_TEST_CASE(getSampleIndicesBySampleType)
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

  BOOST_CHECK_EQUAL(sample_indices.size(), 2);
  BOOST_CHECK_EQUAL(sample_indices[0], 0);
  BOOST_CHECK_EQUAL(sample_indices[1], 2);
}

BOOST_AUTO_TEST_CASE(processCalculateCalibration)
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
  CalculateCalibration calculateCalibration;
  calculateCalibration.process(sequenceSegmentHandler, sequenceHandler, absquant_params, Filenames());

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs = sequenceSegmentHandler.getQuantitationMethods();

  BOOST_CHECK_EQUAL(AQMs.size(), 3);

  BOOST_CHECK_EQUAL(AQMs[0].getComponentName(), "amp.amp_1.Light");
  BOOST_CHECK_EQUAL(AQMs[0].getISName(), "amp.amp_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("slope")), 0.957996830126945, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("intercept")), -1.0475433871941753, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[0].getNPoints(), 11);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getCorrelationCoefficient()), 0.9991692616730385, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getLLOQ()), 0.02, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getULOQ()), 40.0, 1e-6);

  BOOST_CHECK_EQUAL(AQMs[1].getComponentName(), "atp.atp_1.Light");
  BOOST_CHECK_EQUAL(AQMs[1].getISName(), "atp.atp_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[1].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("slope")), 0.6230408240794582, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("intercept")), 0.36130172586029285, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[1].getNPoints(), 6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getCorrelationCoefficient()), 0.9982084021849695, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getLLOQ()), 0.02, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getULOQ()), 40.0, 1e-6);

  BOOST_CHECK_EQUAL(AQMs[2].getComponentName(), "ser-L.ser-L_1.Light");
  BOOST_CHECK_EQUAL(AQMs[2].getISName(), "ser-L.ser-L_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[2].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("slope")), 0.9011392589148208, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("intercept")), 1.8701850759567624, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[2].getNPoints(), 11);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getCorrelationCoefficient()), 0.9993200722867581, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getLLOQ()), 0.04, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getULOQ()), 200.0, 1e-6);

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs_rdh = sequenceHandler.getSequence()[0].getRawData().getQuantitationMethods();
  BOOST_CHECK_EQUAL(AQMs_rdh.size(), 3);

  BOOST_CHECK_EQUAL(AQMs_rdh[0].getComponentName(), "amp.amp_1.Light");
  BOOST_CHECK_EQUAL(AQMs_rdh[0].getISName(), "amp.amp_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs_rdh[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[0].getTransformationModelParams().getValue("slope")), 0.957996830126945, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[0].getTransformationModelParams().getValue("intercept")), -1.0475433871941753, 1e-6);
  BOOST_CHECK_EQUAL(AQMs_rdh[0].getNPoints(), 11);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[0].getCorrelationCoefficient()), 0.9991692616730385, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[0].getLLOQ()), 0.02, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[0].getULOQ()), 40.0, 1e-6);

  BOOST_CHECK_EQUAL(AQMs_rdh[1].getComponentName(), "atp.atp_1.Light");
  BOOST_CHECK_EQUAL(AQMs_rdh[1].getISName(), "atp.atp_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs_rdh[1].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[1].getTransformationModelParams().getValue("slope")), 0.6230408240794582, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[1].getTransformationModelParams().getValue("intercept")), 0.36130172586029285, 1e-6);
  BOOST_CHECK_EQUAL(AQMs_rdh[1].getNPoints(), 6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[1].getCorrelationCoefficient()), 0.9982084021849695, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[1].getLLOQ()), 0.02, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[1].getULOQ()), 40.0, 1e-6);

  BOOST_CHECK_EQUAL(AQMs_rdh[2].getComponentName(), "ser-L.ser-L_1.Light");
  BOOST_CHECK_EQUAL(AQMs_rdh[2].getISName(), "ser-L.ser-L_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs_rdh[2].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[2].getTransformationModelParams().getValue("slope")), 0.9011392589148208, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[2].getTransformationModelParams().getValue("intercept")), 1.8701850759567624, 1e-6);
  BOOST_CHECK_EQUAL(AQMs_rdh[2].getNPoints(), 11);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[2].getCorrelationCoefficient()), 0.9993200722867581, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[2].getLLOQ()), 0.04, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs_rdh[2].getULOQ()), 200.0, 1e-6);
}

/**
  PlotCalibrators Tests
*/
BOOST_AUTO_TEST_CASE(constructorPlotCalibrators)
{
  PlotCalibrators* ptrPlotCalibrators = nullptr;
  PlotCalibrators* nullPointerPlotCalibrators = nullptr;
  BOOST_CHECK_EQUAL(ptrPlotCalibrators, nullPointerPlotCalibrators);
}

BOOST_AUTO_TEST_CASE(destructorPlotCalibrators)
{
  PlotCalibrators* ptrPlotCalibrators = nullptr;
  ptrPlotCalibrators = new PlotCalibrators();
  delete ptrPlotCalibrators;
}

BOOST_AUTO_TEST_CASE(gettersPlotCalibrators)
{
  PlotCalibrators processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "PLOT_CALIBRATORS");
}

BOOST_AUTO_TEST_CASE(processPlotCalibrators)
{
  // TODO: Implementation plotCalibrators
}

/**
  LoadStandardsConcentrations Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadStandardsConcentrations)
{
  LoadStandardsConcentrations* ptrLoadStandardsConcentrations = nullptr;
  LoadStandardsConcentrations* nullPointerLoadStandardsConcentrations = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadStandardsConcentrations, nullPointerLoadStandardsConcentrations);
}

BOOST_AUTO_TEST_CASE(destructorLoadStandardsConcentrations)
{
  LoadStandardsConcentrations* ptrLoadStandardsConcentrations = nullptr;
  ptrLoadStandardsConcentrations = new LoadStandardsConcentrations();
  delete ptrLoadStandardsConcentrations;
}

BOOST_AUTO_TEST_CASE(gettersLoadStandardsConcentrations)
{
  LoadStandardsConcentrations processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_STANDARDS_CONCENTRATIONS");
}

BOOST_AUTO_TEST_CASE(processLoadStandardsConcentrations)
{
  Filenames filenames;
  filenames.standardsConcentrations_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  SequenceSegmentHandler ssh;
  LoadStandardsConcentrations loadStandardsConcentrations;
  loadStandardsConcentrations.process(ssh, SequenceHandler(), {}, filenames);
  const std::vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& rc = ssh.getStandardsConcentrations();

  BOOST_CHECK_EQUAL(rc.size(), 8);

  BOOST_CHECK_EQUAL(rc[0].sample_name, "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(rc[0].component_name, "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(rc[0].IS_component_name, "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(rc[0].actual_concentration, 0.0, 1e-6);
  BOOST_CHECK_CLOSE(rc[0].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[0].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[0].dilution_factor, 1.0, 1e-6);

  BOOST_CHECK_EQUAL(rc[4].sample_name, "150516_CM3_Level9");
  BOOST_CHECK_EQUAL(rc[4].component_name, "ump.ump_2.Light");
  BOOST_CHECK_EQUAL(rc[4].IS_component_name, "ump.ump_1.Heavy");
  BOOST_CHECK_CLOSE(rc[4].actual_concentration, 0.016, 1e-6);
  BOOST_CHECK_CLOSE(rc[4].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[4].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[4].dilution_factor, 1.0, 1e-6);

  BOOST_CHECK_EQUAL(rc[7].sample_name, "150516_CM3_Level9");
  BOOST_CHECK_EQUAL(rc[7].component_name, "utp.utp_2.Light");
  BOOST_CHECK_EQUAL(rc[7].IS_component_name, "utp.utp_1.Heavy");
  BOOST_CHECK_CLOSE(rc[7].actual_concentration, 0.0, 1e-6);
  BOOST_CHECK_CLOSE(rc[7].IS_actual_concentration, 1.0, 1e-6);
  BOOST_CHECK_EQUAL(rc[7].concentration_units, "uM");
  BOOST_CHECK_CLOSE(rc[7].dilution_factor, 1.0, 1e-6);
}

/**
  LoadQuantitationMethods Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadQuantitationMethods)
{
  LoadQuantitationMethods* ptrLoadQuantitationMethods = nullptr;
  LoadQuantitationMethods* nullPointerLoadQuantitationMethods = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadQuantitationMethods, nullPointerLoadQuantitationMethods);
}

BOOST_AUTO_TEST_CASE(destructorLoadQuantitationMethods)
{
  LoadQuantitationMethods* ptrLoadQuantitationMethods = nullptr;
  ptrLoadQuantitationMethods = new LoadQuantitationMethods();
  delete ptrLoadQuantitationMethods;
}

BOOST_AUTO_TEST_CASE(gettersLoadQuantitationMethods)
{
  LoadQuantitationMethods processor;

  BOOST_CHECK_EQUAL(processor.getID(), 17);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_QUANTITATION_METHODS");
}

BOOST_AUTO_TEST_CASE(processLoadQuantitationMethods)
{
  Filenames filenames;
  filenames.quantitationMethods_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_quantitationMethods_1.csv");
  SequenceSegmentHandler ssh;
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(ssh, SequenceHandler(), {}, filenames);
  const std::vector<OpenMS::AbsoluteQuantitationMethod>& aqm = ssh.getQuantitationMethods();

  BOOST_CHECK_EQUAL(aqm.size(), 107);

  BOOST_CHECK_EQUAL(aqm[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(aqm[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_EQUAL(aqm[0].getISName(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(aqm[0].getConcentrationUnits(), "uM");
  BOOST_CHECK_EQUAL(aqm[0].getTransformationModel(), "linear");
  BOOST_CHECK_CLOSE(aqm[0].getLLOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getULOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getLLOQ(), 0.25, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getULOQ(), 2.5, 1e-6);
  BOOST_CHECK_CLOSE(aqm[0].getCorrelationCoefficient(), 0.983846949, 1e-6);
  BOOST_CHECK_EQUAL(aqm[0].getNPoints(), 4);
  const OpenMS::Param params1 = aqm[0].getTransformationModelParams();
  BOOST_CHECK_CLOSE(static_cast<double>(params1.getValue("slope")), 2.429728323, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(params1.getValue("intercept")), -0.091856745, 1e-6);

  BOOST_CHECK_EQUAL(aqm[106].getComponentName(), "xan.xan_1.Light");
  BOOST_CHECK_EQUAL(aqm[106].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_EQUAL(aqm[106].getISName(), "xan.xan_1.Heavy");
  BOOST_CHECK_EQUAL(aqm[106].getConcentrationUnits(), "uM");
  BOOST_CHECK_EQUAL(aqm[106].getTransformationModel(), "linear");
  BOOST_CHECK_CLOSE(aqm[106].getLLOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getULOD(), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getLLOQ(), 0.004, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getULOQ(), 0.16, 1e-6);
  BOOST_CHECK_CLOSE(aqm[106].getCorrelationCoefficient(), 0.994348761, 1e-6);
  BOOST_CHECK_EQUAL(aqm[106].getNPoints(), 6);
  const OpenMS::Param params2 = aqm[106].getTransformationModelParams();
  BOOST_CHECK_CLOSE(static_cast<double>(params2.getValue("slope")), 1.084995619, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(params2.getValue("intercept")), -0.00224781, 1e-6);
}

/**
  StoreQuantitationMethods Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreQuantitationMethods)
{
  StoreQuantitationMethods* ptrStoreQuantitationMethods = nullptr;
  StoreQuantitationMethods* nullPointerStoreQuantitationMethods = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreQuantitationMethods, nullPointerStoreQuantitationMethods);
}

BOOST_AUTO_TEST_CASE(destructorStoreQuantitationMethods)
{
  StoreQuantitationMethods* ptrStoreQuantitationMethods = nullptr;
  ptrStoreQuantitationMethods = new StoreQuantitationMethods();
  delete ptrStoreQuantitationMethods;
}

BOOST_AUTO_TEST_CASE(gettersStoreQuantitationMethods)
{
  StoreQuantitationMethods processor;

  BOOST_CHECK_EQUAL(processor.getID(), 16);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_QUANTITATION_METHODS");
}

BOOST_AUTO_TEST_CASE(processStoreQuantitationMethods)
{
  // no tests, it wraps OpenMS store function
}

/**
  LoadFeatureFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureFilters)
{
  LoadFeatureFilters* ptrLoadFeatureFilters = nullptr;
  LoadFeatureFilters* nullPointerLoadFeatureFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureFilters, nullPointerLoadFeatureFilters);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureFilters)
{
  LoadFeatureFilters* ptrLoadFeatureFilters = nullptr;
  ptrLoadFeatureFilters = new LoadFeatureFilters();
  delete ptrLoadFeatureFilters;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureFilters)
{
  LoadFeatureFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_FILTERS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureFilters)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureFilters loadFeatureFilters;
  loadFeatureFilters.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureQCs)
{
  LoadFeatureQCs* ptrLoadFeatureQCs = nullptr;
  LoadFeatureQCs* nullPointerLoadFeatureQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureQCs, nullPointerLoadFeatureQCs);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureQCs)
{
  LoadFeatureQCs* ptrLoadFeatureQCs = nullptr;
  ptrLoadFeatureQCs = new LoadFeatureQCs();
  delete ptrLoadFeatureQCs;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureQCs)
{
  LoadFeatureQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_QCS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureQCs)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureQCs loadFeatureQCs;
  loadFeatureQCs.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureFilters)
{
  StoreFeatureFilters* ptrStoreFeatureFilters = nullptr;
  StoreFeatureFilters* nullPointerStoreFeatureFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureFilters, nullPointerStoreFeatureFilters);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureFilters)
{
  StoreFeatureFilters* ptrStoreFeatureFilters = nullptr;
  ptrStoreFeatureFilters = new StoreFeatureFilters();
  delete ptrStoreFeatureFilters;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureFilters)
{
  StoreFeatureFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_FILTERS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureFilters)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureFilters loadFeatureFilters;
  loadFeatureFilters.process(ssh, SequenceHandler(), {}, filenames);
  filenames.featureFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureFilters storeFeatureFilters;
  storeFeatureFilters.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureFilters.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureFilter();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureQCs)
{
  StoreFeatureQCs* ptrStoreFeatureQCs = nullptr;
  StoreFeatureQCs* nullPointerStoreFeatureQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureQCs, nullPointerStoreFeatureQCs);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureQCs)
{
  StoreFeatureQCs* ptrStoreFeatureQCs = nullptr;
  ptrStoreFeatureQCs = new StoreFeatureQCs();
  delete ptrStoreFeatureQCs;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureQCs)
{
  StoreFeatureQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_QCS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureQCs)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureQCs loadFeatureQCs;
  loadFeatureQCs.process(ssh, SequenceHandler(), {}, filenames);
  filenames.featureQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureQCs storeFeatureQCs;
  storeFeatureQCs.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureQCs.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureQC();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadFeatureRSDFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureRSDFilters)
{
  LoadFeatureRSDFilters* ptrLoadFeatureRSDFilters = nullptr;
  LoadFeatureRSDFilters* nullPointerLoadFeatureRSDFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureRSDFilters, nullPointerLoadFeatureRSDFilters);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureRSDFilters)
{
  LoadFeatureRSDFilters* ptrLoadFeatureRSDFilters = nullptr;
  ptrLoadFeatureRSDFilters = new LoadFeatureRSDFilters();
  delete ptrLoadFeatureRSDFilters;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureRSDFilters)
{
  LoadFeatureRSDFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_RSD_FILTERS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureRSDFilters)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.featureRSDFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureRSDFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureRSDFilters loadFeatureRSDFilters;
  loadFeatureRSDFilters.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureRSDQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureRSDQCs)
{
  LoadFeatureRSDQCs* ptrLoadFeatureRSDQCs = nullptr;
  LoadFeatureRSDQCs* nullPointerLoadFeatureRSDQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureRSDQCs, nullPointerLoadFeatureRSDQCs);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureRSDQCs)
{
  LoadFeatureRSDQCs* ptrLoadFeatureRSDQCs = nullptr;
  ptrLoadFeatureRSDQCs = new LoadFeatureRSDQCs();
  delete ptrLoadFeatureRSDQCs;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureRSDQCs)
{
  LoadFeatureRSDQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_RSD_QCS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureRSDQCs)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.featureRSDQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureRSDQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureRSDQCs loadFeatureRSDQCs;
  loadFeatureRSDQCs.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureRSDFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureRSDFilters)
{
  StoreFeatureRSDFilters* ptrStoreFeatureRSDFilters = nullptr;
  StoreFeatureRSDFilters* nullPointerStoreFeatureRSDFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureRSDFilters, nullPointerStoreFeatureRSDFilters);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureRSDFilters)
{
  StoreFeatureRSDFilters* ptrStoreFeatureRSDFilters = nullptr;
  ptrStoreFeatureRSDFilters = new StoreFeatureRSDFilters();
  delete ptrStoreFeatureRSDFilters;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureRSDFilters)
{
  StoreFeatureRSDFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_RSD_FILTERS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureRSDFilters)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.featureRSDFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureRSDFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureRSDFilters loadFeatureRSDFilters;
  loadFeatureRSDFilters.process(ssh, SequenceHandler(), {}, filenames);
  filenames.featureRSDFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureRSDFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureRSDFilters storeFeatureRSDFilters;
  storeFeatureRSDFilters.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureRSDFilters.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDFilter();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureRSDFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureRSDQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureRSDQCs)
{
  StoreFeatureRSDQCs* ptrStoreFeatureRSDQCs = nullptr;
  StoreFeatureRSDQCs* nullPointerStoreFeatureRSDQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureRSDQCs, nullPointerStoreFeatureRSDQCs);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureRSDQCs)
{
  StoreFeatureRSDQCs* ptrStoreFeatureRSDQCs = nullptr;
  ptrStoreFeatureRSDQCs = new StoreFeatureRSDQCs();
  delete ptrStoreFeatureRSDQCs;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureRSDQCs)
{
  StoreFeatureRSDQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_RSD_QCS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureRSDQCs)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.featureRSDQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureRSDQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureRSDQCs loadFeatureRSDQCs;
  loadFeatureRSDQCs.process(ssh, SequenceHandler(), {}, filenames);
  filenames.featureRSDQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureRSDQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureRSDQCs storeFeatureRSDQCs;
  storeFeatureRSDQCs.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureRSDQCs.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureRSDQC();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureRSDQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  LoadFeatureBackgroundFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureBackgroundFilters)
{
  LoadFeatureBackgroundFilters* ptrLoadFeatureBackgroundFilters = nullptr;
  LoadFeatureBackgroundFilters* nullPointerLoadFeatureBackgroundFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureBackgroundFilters, nullPointerLoadFeatureBackgroundFilters);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureBackgroundFilters)
{
  LoadFeatureBackgroundFilters* ptrLoadFeatureBackgroundFilters = nullptr;
  ptrLoadFeatureBackgroundFilters = new LoadFeatureBackgroundFilters();
  delete ptrLoadFeatureBackgroundFilters;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureBackgroundFilters)
{
  LoadFeatureBackgroundFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_BACKGROUND_FILTERS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureBackgroundFilters)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.featureBackgroundFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureBackgroundFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
  loadFeatureBackgroundFilters.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  LoadFeatureBackgroundQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeatureBackgroundQCs)
{
  LoadFeatureBackgroundQCs* ptrLoadFeatureBackgroundQCs = nullptr;
  LoadFeatureBackgroundQCs* nullPointerLoadFeatureBackgroundQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeatureBackgroundQCs, nullPointerLoadFeatureBackgroundQCs);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeatureBackgroundQCs)
{
  LoadFeatureBackgroundQCs* ptrLoadFeatureBackgroundQCs = nullptr;
  ptrLoadFeatureBackgroundQCs = new LoadFeatureBackgroundQCs();
  delete ptrLoadFeatureBackgroundQCs;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeatureBackgroundQCs)
{
  LoadFeatureBackgroundQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURE_BACKGROUND_QCS");
}

BOOST_AUTO_TEST_CASE(processLoadFeatureBackgroundQCs)
{
  SequenceSegmentHandler ssh;

  Filenames filenames;
  filenames.featureBackgroundQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureBackgroundQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");

  LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
  loadFeatureBackgroundQCs.process(ssh, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(fQC.component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), 118);
  BOOST_CHECK_EQUAL(fQC.component_group_qcs[0].component_group_name, "arg-L");
}

/**
  StoreFeatureBackgroundFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureBackgroundFilters)
{
  StoreFeatureBackgroundFilters* ptrStoreFeatureBackgroundFilters = nullptr;
  StoreFeatureBackgroundFilters* nullPointerStoreFeatureBackgroundFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureBackgroundFilters, nullPointerStoreFeatureBackgroundFilters);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureBackgroundFilters)
{
  StoreFeatureBackgroundFilters* ptrStoreFeatureBackgroundFilters = nullptr;
  ptrStoreFeatureBackgroundFilters = new StoreFeatureBackgroundFilters();
  delete ptrStoreFeatureBackgroundFilters;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureBackgroundFilters)
{
  StoreFeatureBackgroundFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_BACKGROUND_FILTERS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureBackgroundFilters)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.featureBackgroundFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureBackgroundFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureBackgroundFilters loadFeatureBackgroundFilters;
  loadFeatureBackgroundFilters.process(ssh, SequenceHandler(), {}, filenames);
  filenames.featureBackgroundFilterComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureBackgroundFilterComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureBackgroundFilters storeFeatureBackgroundFilters;
  storeFeatureBackgroundFilters.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureBackgroundFilters.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundFilter();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureBackgroundFilter();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  StoreFeatureBackgroundQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeatureBackgroundQCs)
{
  StoreFeatureBackgroundQCs* ptrStoreFeatureBackgroundQCs = nullptr;
  StoreFeatureBackgroundQCs* nullPointerStoreFeatureBackgroundQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeatureBackgroundQCs, nullPointerStoreFeatureBackgroundQCs);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeatureBackgroundQCs)
{
  StoreFeatureBackgroundQCs* ptrStoreFeatureBackgroundQCs = nullptr;
  ptrStoreFeatureBackgroundQCs = new StoreFeatureBackgroundQCs();
  delete ptrStoreFeatureBackgroundQCs;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeatureBackgroundQCs)
{
  StoreFeatureBackgroundQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURE_BACKGROUND_QCS");
}

BOOST_AUTO_TEST_CASE(processStoreFeatureBackgroundQCs)
{
  SequenceSegmentHandler ssh, ssh_test;

  Filenames filenames;
  filenames.featureBackgroundQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1.csv");
  filenames.featureBackgroundQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1.csv");
  LoadFeatureBackgroundQCs loadFeatureBackgroundQCs;
  loadFeatureBackgroundQCs.process(ssh, SequenceHandler(), {}, filenames);
  filenames.featureBackgroundQCComponents_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponents_1_test.csv");
  filenames.featureBackgroundQCComponentGroups_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_mrmfeatureqccomponentgroups_1_test.csv");
  StoreFeatureBackgroundQCs storeFeatureBackgroundQCs;
  storeFeatureBackgroundQCs.process(ssh, SequenceHandler(), {}, filenames);
  loadFeatureBackgroundQCs.process(ssh_test, SequenceHandler(), {}, filenames);
  const OpenMS::MRMFeatureQC& fQC = ssh.getFeatureBackgroundQC();
  const OpenMS::MRMFeatureQC& fQC_test = ssh_test.getFeatureBackgroundQC();

  BOOST_CHECK_EQUAL(fQC.component_qcs.size(), fQC_test.component_qcs.size());
  for (size_t i = 0; i < fQC.component_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_qcs.at(i) == fQC_test.component_qcs.at(i));
  }
  BOOST_CHECK_EQUAL(fQC.component_group_qcs.size(), fQC_test.component_group_qcs.size());
  for (size_t i = 0; i < fQC.component_group_qcs.size(); ++i) {
    BOOST_CHECK(fQC.component_group_qcs.at(i) == fQC_test.component_group_qcs.at(i));
  }
}

/**
  EstimateFeatureFilterValues Tests
*/
BOOST_AUTO_TEST_CASE(constructorEstimateFeatureFilterValues)
{
  EstimateFeatureFilterValues* ptrEstimateFeatureFilterValues = nullptr;
  EstimateFeatureFilterValues* nullPointerEstimateFeatureFilterValues = nullptr;
  BOOST_CHECK_EQUAL(ptrEstimateFeatureFilterValues, nullPointerEstimateFeatureFilterValues);
}

BOOST_AUTO_TEST_CASE(destructorEstimateFeatureFilterValues)
{
  EstimateFeatureFilterValues* ptrEstimateFeatureFilterValues = nullptr;
  ptrEstimateFeatureFilterValues = new EstimateFeatureFilterValues();
  delete ptrEstimateFeatureFilterValues;
}

BOOST_AUTO_TEST_CASE(gettersEstimateFeatureFilterValues)
{
  EstimateFeatureFilterValues processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "ESTIMATE_FEATURE_FILTER_VALUES");
}

BOOST_AUTO_TEST_CASE(processEstimateFeatureFilterValues)
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
  const map<string, vector<map<string, string>>> params;
  Filenames filenames;

  // Process
  EstimateFeatureFilterValues processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureFilter();
  // Ser-L
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_heavy_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_heavy_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_light_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_light_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_detecting_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_detecting_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_quantifying_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_quantifying_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_transitions_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_transitions_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).ion_ratio_u, 1000000000000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_l, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_u, 2230000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").second, 2230000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_l, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_u, 24500, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").second, 24500, 1e-4);
  // Amp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_heavy_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_heavy_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_light_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_light_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_detecting_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_detecting_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_quantifying_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_quantifying_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_transitions_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_transitions_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(1).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(1).ion_ratio_u, 1000000000000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_l, 440, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_u, 3640000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").first, 440, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").second, 3640000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_l, 215000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_u, 462000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").first, 215000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").second, 462000, 1e-4);
  // Atp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_heavy_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_heavy_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_light_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_light_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_detecting_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_detecting_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_quantifying_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_quantifying_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_transitions_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_transitions_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(2).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(2).ion_ratio_u, 1000000000000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_l, 221, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_u, 4190000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").first, 221, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").second, 4190000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_l, 828, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_u, 322000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").first, 828, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").second, 322000, 1e-4);
}

/**
  EstimateFeatureQCValues Tests
*/
BOOST_AUTO_TEST_CASE(constructorEstimateFeatureQCValues)
{
  EstimateFeatureQCValues* ptrEstimateFeatureQCValues = nullptr;
  EstimateFeatureQCValues* nullPointerEstimateFeatureQCValues = nullptr;
  BOOST_CHECK_EQUAL(ptrEstimateFeatureQCValues, nullPointerEstimateFeatureQCValues);
}

BOOST_AUTO_TEST_CASE(destructorEstimateFeatureQCValues)
{
  EstimateFeatureQCValues* ptrEstimateFeatureQCValues = nullptr;
  ptrEstimateFeatureQCValues = new EstimateFeatureQCValues();
  delete ptrEstimateFeatureQCValues;
}

BOOST_AUTO_TEST_CASE(gettersEstimateFeatureQCValues)
{
  EstimateFeatureQCValues processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "ESTIMATE_FEATURE_QC_VALUES");
}

BOOST_AUTO_TEST_CASE(processEstimateFeatureQCValues)
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
  const map<string, vector<map<string, string>>> params;
  Filenames filenames;

  // Process
  EstimateFeatureQCValues processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureQC();
  // Ser-L
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_heavy_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_heavy_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_light_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_light_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_detecting_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_detecting_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_quantifying_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_quantifying_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_transitions_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_transitions_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).ion_ratio_u, 1000000000000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_l, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_u, 2230000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").second, 2230000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_l, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_u, 24500, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").first, 2830, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").second, 24500, 1e-4);
  // Amp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_heavy_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_heavy_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_light_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_light_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_detecting_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_detecting_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_quantifying_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_quantifying_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_transitions_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_transitions_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(1).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(1).ion_ratio_u, 1000000000000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_l, 440, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_u, 3640000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").first, 440, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").second, 3640000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_l, 215000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_u, 462000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").first, 215000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").second, 462000, 1e-4);
  // Atp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_heavy_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_heavy_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_light_l, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_light_u, 1);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_detecting_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_detecting_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_quantifying_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_quantifying_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_transitions_l, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_transitions_u, 2);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(2).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(2).ion_ratio_u, 1000000000000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_l, 221, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_u, 4190000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").first, 221, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").second, 4190000, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_l, 828, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_u, 322000, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").first, 828, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").second, 322000, 1e-4);
}

/**
  TransferLOQToFeatureFilters Tests
*/
BOOST_AUTO_TEST_CASE(constructorTransferLOQToFeatureFilters)
{
  TransferLOQToFeatureFilters* ptrTransferLOQToFeatureFilters = nullptr;
  TransferLOQToFeatureFilters* nullPointerTransferLOQToFeatureFilters = nullptr;
  BOOST_CHECK_EQUAL(ptrTransferLOQToFeatureFilters, nullPointerTransferLOQToFeatureFilters);
}

BOOST_AUTO_TEST_CASE(destructorTransferLOQToFeatureFilters)
{
  TransferLOQToFeatureFilters* ptrTransferLOQToFeatureFilters = nullptr;
  ptrTransferLOQToFeatureFilters = new TransferLOQToFeatureFilters();
  delete ptrTransferLOQToFeatureFilters;
}

BOOST_AUTO_TEST_CASE(gettersTransferLOQToFeatureFilters)
{
  TransferLOQToFeatureFilters processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "TRANSFER_LOQ_TO_FEATURE_FILTERS");
}

BOOST_AUTO_TEST_CASE(processTransferLOQToFeatureFilters)
{
  // TODO: Implementation TransferLOQToFeatureFilters tests
}

/**
  TransferLOQToFeatureQCs Tests
*/
BOOST_AUTO_TEST_CASE(constructorTransferLOQToFeatureQCs)
{
  TransferLOQToFeatureQCs* ptrTransferLOQToFeatureQCs = nullptr;
  TransferLOQToFeatureQCs* nullPointerTransferLOQToFeatureQCs = nullptr;
  BOOST_CHECK_EQUAL(ptrTransferLOQToFeatureQCs, nullPointerTransferLOQToFeatureQCs);
}

BOOST_AUTO_TEST_CASE(destructorTransferLOQToFeatureQCs)
{
  TransferLOQToFeatureQCs* ptrTransferLOQToFeatureQCs = nullptr;
  ptrTransferLOQToFeatureQCs = new TransferLOQToFeatureQCs();
  delete ptrTransferLOQToFeatureQCs;
}

BOOST_AUTO_TEST_CASE(gettersTransferLOQToFeatureQCs)
{
  TransferLOQToFeatureQCs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "TRANSFER_LOQ_TO_FEATURE_QCS");
}

BOOST_AUTO_TEST_CASE(processTransferLOQToFeatureQCs)
{
  // TODO: Implementation TransferLOQToFeatureQCs tests
}

/**
  EstimateFeatureRSDs Tests
*/
BOOST_AUTO_TEST_CASE(constructorEstimateFeatureRSDs)
{
  EstimateFeatureRSDs* ptrEstimateFeatureRSDs = nullptr;
  EstimateFeatureRSDs* nullPointerEstimateFeatureRSDs = nullptr;
  BOOST_CHECK_EQUAL(ptrEstimateFeatureRSDs, nullPointerEstimateFeatureRSDs);
}

BOOST_AUTO_TEST_CASE(destructorEstimateFeatureRSDs)
{
  EstimateFeatureRSDs* ptrEstimateFeatureRSDs = nullptr;
  ptrEstimateFeatureRSDs = new EstimateFeatureRSDs();
  delete ptrEstimateFeatureRSDs;
}

BOOST_AUTO_TEST_CASE(gettersEstimateFeatureRSDs)
{
  EstimateFeatureRSDs processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "ESTIMATE_FEATURE_RSDS");
}

BOOST_AUTO_TEST_CASE(processEstimateFeatureRSDs)
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
  sequenceHandler.getSequenceSegments().front().setFeatureRSDEstimations(feature_filter_template);

  // Make other constructs
  const map<string, vector<map<string, string>>> params;
  Filenames filenames;

  // Process
  EstimateFeatureRSDs processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureRSDEstimations();
  // Ser-L
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_heavy_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_heavy_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_light_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_light_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_detecting_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_detecting_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_quantifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_quantifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_transitions_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).n_transitions_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).ion_ratio_u, 0, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_u, 45.940820332935274, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).meta_value_qc.at("peak_apex_int").second, 45.940820332935274, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_u, 45.940820332935274, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).meta_value_qc.at("peak_apex_int").second, 45.940820332935274, 1e-4);
  // Amp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_heavy_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_heavy_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_light_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_light_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_detecting_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_detecting_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_quantifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_quantifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_transitions_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).n_transitions_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(1).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(1).ion_ratio_u, 0, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_u, 21.980528755438982, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).meta_value_qc.at("peak_apex_int").second, 21.980528755438982, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_u, 21.980528755438982, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).meta_value_qc.at("peak_apex_int").second, 21.980528755438982, 1e-4);
  // Atp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_heavy_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_heavy_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_light_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_light_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_detecting_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_detecting_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_quantifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_quantifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_identifying_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_identifying_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_transitions_l, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).n_transitions_u, 0);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_1, "");
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_pair_name_2, "");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(2).ion_ratio_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(2).ion_ratio_u, 0, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).ion_ratio_feature_name, "");
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_u, 148.21486099460165, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).meta_value_qc.at("peak_apex_int").second, 148.21486099460165, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).retention_time_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).retention_time_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_u, 148.21486099460165, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).overall_quality_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).overall_quality_u, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").first, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).meta_value_qc.at("peak_apex_int").second, 148.21486099460165, 1e-4);
}


/**
  EstimateFeatureBackgroundInterferences Tests
*/
BOOST_AUTO_TEST_CASE(constructorEstimateFeatureBackgroundInterferences)
{
  EstimateFeatureBackgroundInterferences* ptrEstimateFeatureBackgroundInterferences = nullptr;
  EstimateFeatureBackgroundInterferences* nullPointerEstimateFeatureBackgroundInterferences = nullptr;
  BOOST_CHECK_EQUAL(ptrEstimateFeatureBackgroundInterferences, nullPointerEstimateFeatureBackgroundInterferences);
}

BOOST_AUTO_TEST_CASE(destructorEstimateFeatureBackgroundInterferences)
{
  EstimateFeatureBackgroundInterferences* ptrEstimateFeatureBackgroundInterferences = nullptr;
  ptrEstimateFeatureBackgroundInterferences = new EstimateFeatureBackgroundInterferences();
  delete ptrEstimateFeatureBackgroundInterferences;
}

BOOST_AUTO_TEST_CASE(gettersEstimateFeatureBackgroundInterferences)
{
  EstimateFeatureBackgroundInterferences processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES");
}

BOOST_AUTO_TEST_CASE(processEstimateFeatureBackgroundInterferences)
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
  sequenceHandler.getSequenceSegments().front().setFeatureBackgroundEstimations(feature_filter_template);

  // Make other constructs
  const map<string, vector<map<string, string>>> params;
  Filenames filenames;

  // Process
  EstimateFeatureBackgroundInterferences processor;
  processor.process(sequenceHandler.getSequenceSegments().front(), sequenceHandler, params, filenames);

  // Test for the expected values
  const OpenMS::MRMFeatureQC& feature_filters = sequenceHandler.getSequenceSegments().front().getFeatureBackgroundEstimations();
  // Ser-L
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(0).component_group_name, "ser-L");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).intensity_u, 400499.23928506032, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(0).component_name, "ser-L.ser-L_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(0).intensity_u, 400349.32857131958, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(1).component_name, "ser-L.ser-L_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(1).intensity_u, 149.91428593226843, 1e-4);
  // Amp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(1).component_group_name, "amp");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).intensity_u, 400499.23928506032, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(2).component_name, "amp.amp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(2).intensity_u, 5839.5428571360453, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(3).component_name, "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(3).intensity_u, 3019.2857142857142, 1e-4);
  // Atp
  BOOST_CHECK_EQUAL(feature_filters.component_group_qcs.at(2).component_group_name, "atp");
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_group_qcs.at(0).intensity_u, 400499.23928506032, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(4).component_name, "atp.atp_1.Light");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(4).intensity_u, 6859.2635714156286, 1e-4);
  BOOST_CHECK_EQUAL(feature_filters.component_qcs.at(5).component_name, "atp.atp_1.Heavy");
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_l, 0, 1e-4);
  BOOST_CHECK_CLOSE(feature_filters.component_qcs.at(5).intensity_u, 870.49857139587402, 1e-4);
}

BOOST_AUTO_TEST_SUITE_END()
