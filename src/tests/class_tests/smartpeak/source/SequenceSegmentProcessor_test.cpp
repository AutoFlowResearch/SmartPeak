// TODO: Add copyright

// #include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceSegmentProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceSegmentProcessor.h>

using namespace SmartPeak;
using namespace std;

void make_featuresAndStandardsConcentrations(
  SequenceHandler& sequenceHandler_IO,
  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& runs
)
{
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
    meta_data.setSampleType(MetaDataHandler::SampleType::Standard);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    sequenceHandler_IO.addSampleToSequence(meta_data, OpenMS::FeatureMap());

    RawDataHandler rawDataHandler;
    rawDataHandler.setFeatureMap(feature_map);
    sequenceHandler_IO.getSequence().at(i).setRawData(rawDataHandler);
  }
}

BOOST_AUTO_TEST_SUITE(sequencesegmentprocessor)

BOOST_AUTO_TEST_CASE(checkSequenceSegmentProcessing)
{
  const std::vector<std::string> events1 = {
    "calculate_calibration",
    "calculate_carryover",
    "calculate_variability",
    "store_quantitation_methods",
    "load_quantitation_methods",
    "store_components_to_concentrations",
    "plot_calibrators"
  };
  BOOST_CHECK_EQUAL(SequenceSegmentProcessor::checkSequenceSegmentProcessing(events1), true);

  const std::vector<std::string> events2 = {
    "calculate_calibration",
    "carryover"
  };
  BOOST_CHECK_EQUAL(SequenceSegmentProcessor::checkSequenceSegmentProcessing(events2), false);
}

BOOST_AUTO_TEST_CASE(getDefaultSequenceSegmentProcessingWorkflow)
{
  std::vector<std::string> workflow;

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Unknown, workflow);
  BOOST_CHECK_EQUAL(workflow.empty(), true);

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Standard, workflow);
  BOOST_CHECK_EQUAL(workflow.size(), 1);
  BOOST_CHECK_EQUAL(workflow[0], "calculate_calibration");

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::QC, workflow);
  BOOST_CHECK_EQUAL(workflow.size(), 1);
  BOOST_CHECK_EQUAL(workflow[0], "calculate_variability");

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Blank, workflow);
  BOOST_CHECK_EQUAL(workflow.empty(), true);

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::DoubleBlank, workflow);
  BOOST_CHECK_EQUAL(workflow.empty(), true);

  SequenceSegmentProcessor::getDefaultSequenceSegmentProcessingWorkflow(MetaDataHandler::SampleType::Solvent, workflow);
  BOOST_CHECK_EQUAL(workflow.size(), 1);
  BOOST_CHECK_EQUAL(workflow[0], "calculate_carryover");
}

BOOST_AUTO_TEST_CASE(getSampleIndicesBySampleType)
{
  MetaDataHandler meta_data1;
  meta_data1.setFilename("file1");
  meta_data1.setSampleName("sample1");
  meta_data1.setSampleGroupName("sample");
  meta_data1.setSequenceSegmentName("sequence_segment");
  meta_data1.setSampleType(MetaDataHandler::SampleType::Unknown);
  meta_data1.acq_method_name = "6";
  meta_data1.inj_volume = 7.0;
  meta_data1.inj_volume_units = "8";
  meta_data1.batch_name = "9";

  MetaDataHandler meta_data2;
  meta_data2.setFilename("file2");
  meta_data2.setSampleName("sample2");
  meta_data2.setSampleGroupName("sample");
  meta_data2.setSequenceSegmentName("sequence_segment");
  meta_data2.setSampleType(MetaDataHandler::SampleType::Standard);
  meta_data2.acq_method_name = "6";
  meta_data2.inj_volume = 7.0;
  meta_data2.inj_volume_units = "8";
  meta_data2.batch_name = "9";

  MetaDataHandler meta_data3;
  meta_data3.setFilename("file3");
  meta_data3.setSampleName("sample3");
  meta_data3.setSampleGroupName("sample");
  meta_data3.setSequenceSegmentName("sequence_segment");
  meta_data3.setSampleType(MetaDataHandler::SampleType::Unknown);
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
    MetaDataHandler::SampleType::Unknown,
    sample_indices
  );

  BOOST_CHECK_EQUAL(sample_indices.size(), 2);
  BOOST_CHECK_EQUAL(sample_indices[0], 0);
  BOOST_CHECK_EQUAL(sample_indices[1], 2);
}

BOOST_AUTO_TEST_CASE(optimizeCalibrationCurves)
{
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

  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs;
  SequenceHandler sequenceHandler;
  make_featuresAndStandardsConcentrations(sequenceHandler, runs);
  sequenceSegmentHandler.setStandardsConcentrations(runs);

  vector<size_t> indices(sequenceHandler.getSequence().size());
  std::iota(indices.begin(), indices.end(), 0);

  sequenceSegmentHandler.setSampleIndices(indices);

  SequenceSegmentProcessor::optimizeCalibrationCurves(
    sequenceSegmentHandler,
    sequenceHandler,
    absquant_params.at("AbsoluteQuantitation")
  );

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
}

BOOST_AUTO_TEST_CASE(plotCalibrators)
{
  // TODO: Implementation plotCalibrators
}

BOOST_AUTO_TEST_CASE(processSequenceSegment)
{
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

  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs;
  SequenceHandler sequenceHandler;
  make_featuresAndStandardsConcentrations(sequenceHandler, runs);
  sequenceSegmentHandler.setStandardsConcentrations(runs);

  vector<size_t> indices(sequenceHandler.getSequence().size());
  std::iota(indices.begin(), indices.end(), 0);

  sequenceSegmentHandler.setSampleIndices(indices);

  SequenceSegmentProcessor::processSequenceSegment(
    sequenceSegmentHandler,
    sequenceHandler,
    "calculate_calibration",
    absquant_params,
    Filenames()
  );

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& a = sequenceSegmentHandler.getQuantitationMethods();

  for (const size_t index : sequenceSegmentHandler.getSampleIndices()) {
    const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs = sequenceHandler.getSequence().at(index).getRawData().getQuantitationMethods();
    BOOST_CHECK_EQUAL(AQMs.size(), a.size());
  }
}

BOOST_AUTO_TEST_SUITE_END()
