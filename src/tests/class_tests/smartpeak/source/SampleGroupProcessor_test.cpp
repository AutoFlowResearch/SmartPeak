// TODO: Add copyright

 #include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SampleGroupProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SampleGroupProcessor.h>

using namespace SmartPeak;
using namespace std;

void makeSequence(SequenceHandler& sequenceHandler_IO, const bool& test_only_polarity) {
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    2.32e4, 2.45e4, 1.78e4, 2.11e4, 1.91e4,
    2.06e4, 1.85e4, 1.53e4
  };
  const vector<double> y1 = {
    4.94e3, 6.55e3, 7.37e3, 1.54e4, 2.87e4,
    5.41e4, 1.16e5, 1.85e5
  };
  const vector<double> z1 = {
    1.00e-2, 2.00e-2, 4.00e-2, 1.00e-1, 2.00e-1,
    4.00e-1, 1.00e0, 2.00e0
  };

  // amp.amp_1.Light
  const vector<double> x2 = {
    2.15e5, 2.32e5, 2.69e5, 2.53e5, 2.50e5,
    2.75e5, 2.67e5, 3.31e5
  };
  const vector<double> y2 = {
      4.40e2, 1.15e3, 1.53e3, 2.01e3, 4.47e3,
      7.36e3, 2.18e4, 4.46e4
  };
  const vector<double> z2 = {
      2.00e-3, 4.00e-3, 8.00e-3, 2.00e-2, 4.00e-2,
      8.00e-2, 2.00e-1, 4.00e-1
  };

  // atp.atp_1.Light
  const vector<double> x3 = {
      8.28e2, 1.32e3, 1.57e3, 1.63e3, 1.48e3,
      2.43e3, 4.44e3, 1.03e4
  };
  const vector<double> y3 = {
      2.21e2, 4.41e2, 3.31e2, 2.21e2, 3.09e2,
      5.96e2, 1.26e3, 2.49e3
  };
  const vector<double> z3 = {
      2.00e-3, 4.00e-3, 8.00e-3, 2.00e-2, 4.00e-2,
      8.00e-2, 2.00e-1, 4.00e-1
  };

  size_t max_iters = x1.size();
  if (test_only_polarity) max_iters = 2;
  for (size_t i = 0; i < max_iters; ++i) {
    const string sample_name = "level" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::Feature mrm_feature;
    OpenMS::Feature component, IS_component;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", x1[i]);
    component.setMetaValue("QC_transition_score", y1[i]);
    component.setMetaValue("calculated_concentration", z1[i]);
    component.setRT(z1[i]);
    component.setMZ(100);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    IS_component.setMetaValue("QC_transition_score", y1[i]);
    IS_component.setMetaValue("calculated_concentration", z1[i]);
    IS_component.setRT(z1[i]);
    IS_component.setMZ(100);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setMetaValue("peak_apex_int", x1[i]);
    mrm_feature.setMetaValue("QC_transition_score", y1[i]);
    mrm_feature.setMetaValue("calculated_concentration", z1[i]);
    mrm_feature.setRT(z1[i]);
    mrm_feature.setMZ(100);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // amp.amp_1.Light
    component.setMetaValue("native_id", "amp.amp_1.Light");
    component.setMetaValue("peak_apex_int", x2[i]);
    component.setMetaValue("QC_transition_score", y2[i]);
    component.setMetaValue("calculated_concentration", z2[i]);
    component.setRT(z2[i]);
    component.setMZ(200);
    IS_component.setMetaValue("native_id", "amp.amp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x2[i]);
    IS_component.setMetaValue("QC_transition_score", y2[i]);
    IS_component.setMetaValue("calculated_concentration", z2[i]);
    IS_component.setRT(z2[i]);
    IS_component.setMZ(200);
    mrm_feature.setMetaValue("PeptideRef", "amp");
    mrm_feature.setMetaValue("peak_apex_int", x2[i]);
    mrm_feature.setMetaValue("QC_transition_score", y2[i]);
    mrm_feature.setMetaValue("calculated_concentration", z2[i]);
    mrm_feature.setRT(z2[i]);
    mrm_feature.setMZ(200);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // atp.atp_1.Light
    component.setMetaValue("native_id", "atp.atp_1.Light");
    component.setMetaValue("peak_apex_int", x3[i]);
    component.setMetaValue("QC_transition_score", y3[i]);
    component.setMetaValue("calculated_concentration", z3[i]);
    component.setRT(z3[i]);
    component.setMZ(300);
    IS_component.setMetaValue("native_id", "atp.atp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x3[i]);
    IS_component.setMetaValue("QC_transition_score", y3[i]);
    IS_component.setMetaValue("calculated_concentration", z3[i]);
    IS_component.setRT(z3[i]);
    IS_component.setMZ(300);
    mrm_feature.setMetaValue("PeptideRef", "atp");
    mrm_feature.setMetaValue("peak_apex_int", x3[i]);
    mrm_feature.setMetaValue("QC_transition_score", y3[i]);
    mrm_feature.setMetaValue("calculated_concentration", z3[i]);
    mrm_feature.setRT(z3[i]);
    mrm_feature.setMZ(300);
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
    if (test_only_polarity) {
      meta_data.scan_polarity = (i % 2 == 0) ? "positive" : "negative";
      meta_data.scan_mass_low = 100;
      meta_data.scan_mass_high = 1000;
      meta_data.dilution_factor = 1.0;
    }
    else {
      meta_data.scan_polarity = (i < 4) ? "positive" : "negative";
      std::vector<int> ranges = { 0,0,1,1,0,0,1,1 };
      meta_data.scan_mass_low = (ranges.at(i) == 0) ? 50 : 200;
      meta_data.scan_mass_high = (ranges.at(i) == 0) ? 200 : 2000;
      meta_data.dilution_factor = (i % 2 == 0) ? 1.0 : 10.0;
    }

    sequenceHandler_IO.addSampleToSequence(meta_data, OpenMS::FeatureMap());

    RawDataHandler rawDataHandler;
    rawDataHandler.setFeatureMap(feature_map);
    sequenceHandler_IO.getSequence().at(i).setRawData(rawDataHandler);
  }
}

BOOST_AUTO_TEST_SUITE(samplegroupprocessor)

/**
  MergeInjections Tests
*/
BOOST_AUTO_TEST_CASE(constructorMergeInjections)
{
  MergeInjections* ptrMergeInjections = nullptr;
  MergeInjections* nullPointerMergeInjections = nullptr;
  BOOST_CHECK_EQUAL(ptrMergeInjections, nullPointerMergeInjections);
}

BOOST_AUTO_TEST_CASE(destructorMergeInjections)
{
  MergeInjections* ptrMergeInjections = nullptr;
  ptrMergeInjections = new MergeInjections();
  delete ptrMergeInjections;
}

BOOST_AUTO_TEST_CASE(gettersMergeInjections)
{
  MergeInjections processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "MERGE_INJECTIONS");
}

BOOST_AUTO_TEST_CASE(processMergeInjections)
{
  // setup the parameters
  map<string, vector<map<string, string>>> mergeinjs_params = {{"MergeInjections", {
    {
      {"name", "scan_polarity_merge_rule"},
      {"value", "WeightedMean"}
    },
    {
      {"name", "mass_range_merge_rule"},
      {"value", "Sum"}
    },
    {
      {"name", "dilution_series_merge_rule"},
      {"value", "Max"}
    },
    {
      {"name", "scan_polarity_merge_feature_name"},
      {"value", "calculated_concentration"}
    },
    {
      {"name", "mass_range_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "dilution_series_merge_feature_name"},
      {"value", "QC_transition_score"}
    },
    {
      {"name", "merge_subordinates"},
      {"value", "true"}
    }
  }}};

  // setup the sequence
  SequenceHandler sequenceHandler;
  makeSequence(sequenceHandler, false);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().front();

  // test merge injections on all with subordinates
  MergeInjections sampleGroupProcessor;
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.458285719, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 400, 1e-4);

  // test merge injections on all with out subordinates
  mergeinjs_params.at("MergeInjections").at(6).at("value") = std::string("false");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 0);
  const OpenMS::Feature& feat1 = sampleGroupHandler.getFeatureMap().at(0);
  BOOST_CHECK_EQUAL(feat1.getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_CLOSE(static_cast<float>(feat1.getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(feat1.getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(feat1.getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(feat1.getRT()), 0.458285719, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(feat1.getMZ()), 400, 1e-4);

  // Test merge injections on polarities with Max
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).at("value") = std::string("Max");
  mergeinjs_params.at("MergeInjections").at(6).at("value") = std::string("true");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);  
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 232000, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 1150, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.004, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.004, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);

  // Test merge injections on polarities with Min
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).at("value") = std::string("Min");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);  
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 215000, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 440, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.002, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.002, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);

  // Test merge injections on polarities with Sum
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).at("value") = std::string("Sum");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);  
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 4.47E+05, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 1.59E+03, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 6.00E-03, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 6.00E-03, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 400, 1e-4);

  // Test merge injections on polarities with Mean
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).at("value") = std::string("Mean");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);  
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 223500, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 7.95E+02, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 3.00E-03, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 3.00E-03, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);

  // Test merge injections on polarities with WeightedMean
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).at("value") = std::string("WeightedMean");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, Filenames());

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);  
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 226333.344, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 913.333374, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.00333333365, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.00333333365, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);
}

/**
  LoadFeaturesSampleGroup Tests
*/
BOOST_AUTO_TEST_CASE(constructorLoadFeaturesSampleGroup)
{
  LoadFeaturesSampleGroup* ptrLoadFeaturesSampleGroup = nullptr;
  LoadFeaturesSampleGroup* nullPointerLoadFeaturesSampleGroup = nullptr;
  BOOST_CHECK_EQUAL(ptrLoadFeaturesSampleGroup, nullPointerLoadFeaturesSampleGroup);
}

BOOST_AUTO_TEST_CASE(destructorLoadFeaturesSampleGroup)
{
  LoadFeaturesSampleGroup* ptrLoadFeaturesSampleGroup = nullptr;
  ptrLoadFeaturesSampleGroup = new LoadFeaturesSampleGroup();
  delete ptrLoadFeaturesSampleGroup;
}

BOOST_AUTO_TEST_CASE(gettersLoadFeaturesSampleGroup)
{
  LoadFeaturesSampleGroup processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "LOAD_FEATURES_SAMPLE_GROUP");
}

BOOST_AUTO_TEST_CASE(processLoadFeaturesSampleGroup)
{
  // setup the sequence
  SequenceHandler sequenceHandler;
  makeSequence(sequenceHandler, false);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().front();

  // test store features
  LoadFeaturesSampleGroup process;
  Filenames filenames;
  filenames.featureXMLSampleGroup_i = SMARTPEAK_GET_TEST_DATA_PATH(sampleGroupHandler.getSampleGroupName() + ".featureXML");
  process.process(sampleGroupHandler, sequenceHandler, {}, filenames);

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "amp");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "amp.amp_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.458285719, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 400, 1e-4);
}

/**
  StoreFeaturesSampleGroup Tests
*/
BOOST_AUTO_TEST_CASE(constructorStoreFeaturesSampleGroup)
{
  StoreFeaturesSampleGroup* ptrStoreFeaturesSampleGroup = nullptr;
  StoreFeaturesSampleGroup* nullPointerStoreFeaturesSampleGroup = nullptr;
  BOOST_CHECK_EQUAL(ptrStoreFeaturesSampleGroup, nullPointerStoreFeaturesSampleGroup);
}

BOOST_AUTO_TEST_CASE(destructorStoreFeaturesSampleGroup)
{
  StoreFeaturesSampleGroup* ptrStoreFeaturesSampleGroup = nullptr;
  ptrStoreFeaturesSampleGroup = new StoreFeaturesSampleGroup();
  delete ptrStoreFeaturesSampleGroup;
}

BOOST_AUTO_TEST_CASE(gettersStoreFeaturesSampleGroup)
{
  StoreFeaturesSampleGroup processor;

  BOOST_CHECK_EQUAL(processor.getID(), -1);
  BOOST_CHECK_EQUAL(processor.getName(), "STORE_FEATURES_SAMPLE_GROUP");
}

BOOST_AUTO_TEST_CASE(processStoreFeaturesSampleGroup)
{
  // no tests, it wraps OpenMS store function
}

BOOST_AUTO_TEST_SUITE_END()
