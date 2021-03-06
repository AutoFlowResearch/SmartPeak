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
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/Filenames.h>
#include <filesystem>

using namespace SmartPeak;
using namespace std;

Filenames generateTestFilenames()
{
  const std::string dir = SMARTPEAK_GET_TEST_DATA_PATH("");
  Filenames filenames;
  filenames.sequence_csv_i                                = dir + "SequenceProcessor_sequence.csv";
  filenames.parameters_csv_i                              = dir + "RawDataProcessor_params_1_core.csv";
  filenames.traML_csv_i                                   = dir + "OpenMSFile_traML_1.csv";
  filenames.featureFilterComponents_csv_i                 = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureFilterComponentGroups_csv_i            = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureQCComponents_csv_i                     = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureQCComponentGroups_csv_i                = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureRSDFilterComponents_csv_i              = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureRSDFilterComponentGroups_csv_i         = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureRSDQCComponents_csv_i                  = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureRSDQCComponentGroups_csv_i             = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureBackgroundFilterComponents_csv_i       = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureBackgroundFilterComponentGroups_csv_i  = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.featureBackgroundQCComponents_csv_i           = dir + "OpenMSFile_mrmfeatureqccomponents_1.csv";
  filenames.featureBackgroundQCComponentGroups_csv_i      = dir + "OpenMSFile_mrmfeatureqccomponentgroups_1.csv";
  filenames.quantitationMethods_csv_i                     = dir + "OpenMSFile_quantitationMethods_1.csv";
  filenames.standardsConcentrations_csv_i                 = dir + "OpenMSFile_standardsConcentrations_1.csv";
  return filenames;
}

TEST(SequenceHandler, createSequence_onFilePicked)
{
  ApplicationHandler ah;
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  std::string datapath_ = SMARTPEAK_GET_TEST_DATA_PATH("");
  auto workflow = std::filesystem::path{ datapath_ } / std::filesystem::path{ "workflow_csv_files" };
  auto filenames_ = Filenames::getDefaultStaticFilenames(workflow.string());
  cs.onFilePicked(filenames_.sequence_csv_i, &ah);

  EXPECT_EQ(sequenceHandler.getSequence().size(), 2);
  InjectionHandler& injection0 = sequenceHandler.getSequence()[0];
  EXPECT_STREQ(injection0.getMetaData().getSampleName().c_str(), "150516_CM1_Level1");
  EXPECT_STREQ(injection0.getMetaData().getSampleGroupName().c_str(), "CM");
  EXPECT_STREQ(injection0.getRawData().getMetaData().getSampleName().c_str(), "150516_CM1_Level1");
  EXPECT_EQ(injection0.getRawData().getParameters().size(), 27);
  EXPECT_STREQ(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_EQ(injection0.getRawData().getQuantitationMethods().size(), 10);
  EXPECT_STREQ(injection0.getRawData().getQuantitationMethods()[0].getComponentName().c_str(), "arg-L.arg-L_1.Light");
}

TEST(SequenceHandler, createSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames_        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  // Test initialization of the sequence
  EXPECT_EQ(sequenceHandler.getSequence().size(), 6);
  InjectionHandler& injection0 = sequenceHandler.getSequence()[0];
  EXPECT_STREQ(injection0.getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_STREQ(injection0.getMetaData().getSampleGroupName().c_str(), "Test01");
  EXPECT_STREQ(injection0.getRawData().getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Sacc1_1x");
  EXPECT_EQ(injection0.getRawData().getParameters().size(), 27);
  EXPECT_STREQ(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_EQ(injection0.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  EXPECT_STREQ(injection0.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef().c_str(), "arg-L");
  EXPECT_EQ(injection0.getRawData().getFeatureFilter().component_qcs.size(), 324);
  EXPECT_STREQ(injection0.getRawData().getFeatureFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection0.getRawData().getFeatureQC().component_qcs.size(), 324);
  EXPECT_STREQ(injection0.getRawData().getFeatureQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection0.getRawData().getFeatureRSDFilter().component_qcs.size(), 324);
  EXPECT_STREQ(injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection0.getRawData().getFeatureRSDQC().component_qcs.size(), 324);
  EXPECT_STREQ(injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection0.getRawData().getFeatureBackgroundFilter().component_qcs.size(), 324);
  EXPECT_STREQ(injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection0.getRawData().getFeatureBackgroundQC().component_qcs.size(), 324);
  EXPECT_STREQ(injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection0.getRawData().getQuantitationMethods().size(), 107);
  EXPECT_STREQ(injection0.getRawData().getQuantitationMethods()[0].getComponentName().c_str(), "23dpg.23dpg_1.Light");
  InjectionHandler& injection5 = sequenceHandler.getSequence()[5];
  EXPECT_STREQ(injection5.getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr3_1x");
  EXPECT_STREQ(injection5.getMetaData().getSampleGroupName().c_str(), "Test02");
  EXPECT_STREQ(injection5.getRawData().getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr3_1x");
  EXPECT_EQ(injection5.getRawData().getParameters().size(), 27);
  EXPECT_STREQ(injection5.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_EQ(injection5.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  EXPECT_STREQ(injection5.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef().c_str(), "arg-L");
  EXPECT_EQ(injection5.getRawData().getFeatureFilter().component_qcs.size(), 324);
  EXPECT_STREQ(injection5.getRawData().getFeatureFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection5.getRawData().getFeatureQC().component_qcs.size(), 324);
  EXPECT_STREQ(injection5.getRawData().getFeatureQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection5.getRawData().getFeatureRSDFilter().component_qcs.size(), 324);
  EXPECT_STREQ(injection5.getRawData().getFeatureRSDFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection5.getRawData().getFeatureRSDQC().component_qcs.size(), 324);
  EXPECT_STREQ(injection5.getRawData().getFeatureRSDQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection5.getRawData().getFeatureBackgroundFilter().component_qcs.size(), 324);
  EXPECT_STREQ(injection5.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection5.getRawData().getFeatureBackgroundQC().component_qcs.size(), 324);
  EXPECT_STREQ(injection5.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(injection5.getRawData().getQuantitationMethods().size(), 107);
  EXPECT_STREQ(injection5.getRawData().getQuantitationMethods()[0].getComponentName().c_str(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(sequenceHandler.getSequenceSegments().size(), 1);
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods().size(), 107);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName().c_str(), "23dpg.23dpg_1.Light");
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs.size(), 324);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs.size(), 324);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs.size(), 324);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs.size(), 324);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs.size(), 324);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");
  EXPECT_EQ(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs.size(), 324);
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy");

  // Test non-shared resources
  injection0.getMetaData().setSampleName("modified");
  EXPECT_STREQ(injection0.getRawData().getMetaData().getSampleName().c_str(), "modified");
  EXPECT_STREQ(injection5.getMetaData().getSampleName().c_str(), "170808_Jonathan_yeast_Yarr3_1x");

  // Test shared resources between all raw data handlers
  injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].setName("modified");
  EXPECT_STREQ(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName().c_str(), "modified");
  EXPECT_STREQ(injection5.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName().c_str(), "modified");
  auto transitions = injection0.getRawData().getTargetedExperiment().getTransitions();
  transitions[0].setPeptideRef("arg-L-mod");
  injection0.getRawData().getTargetedExperiment().setTransitions(transitions);
  EXPECT_STREQ(injection0.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef().c_str(), "arg-L-mod");
  EXPECT_STREQ(injection5.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef().c_str(), "arg-L-mod");

  // Test shared resources between sequence segment handlers
  injection0.getRawData().getQuantitationMethods()[0].setComponentName("23dpg.23dpg_1.Light-mod");
  EXPECT_STREQ(injection0.getRawData().getQuantitationMethods()[0].getComponentName().c_str(), "23dpg.23dpg_1.Light-mod");
  EXPECT_STREQ(injection5.getRawData().getQuantitationMethods()[0].getComponentName().c_str(), "23dpg.23dpg_1.Light-mod");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName().c_str(), "23dpg.23dpg_1.Light-mod");
  injection0.getRawData().getFeatureFilter().component_qcs[0].component_name = "arg-L.arg-L_1.Heavy-mod";
  EXPECT_STREQ(injection0.getRawData().getFeatureFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy-mod");
  EXPECT_STREQ(injection5.getRawData().getFeatureFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy-mod");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy-mod");
  injection0.getRawData().getFeatureQC().component_qcs[0].component_name = "arg-L.arg-L_1.Heavy-modified";
  EXPECT_STREQ(injection0.getRawData().getFeatureQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy-modified");
  EXPECT_STREQ(injection5.getRawData().getFeatureQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy-modified");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs[0].component_name.c_str(), "arg-L.arg-L_1.Heavy-modified");
  injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name = "trp-L.trp-L_1.Heavy-mod";
  EXPECT_STREQ(injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name.c_str(), "trp-L.trp-L_1.Heavy-mod");
  EXPECT_STREQ(injection5.getRawData().getFeatureRSDFilter().component_qcs[0].component_name.c_str(), "trp-L.trp-L_1.Heavy-mod");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs[0].component_name.c_str(), "trp-L.trp-L_1.Heavy-mod");
  injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name = "trp-L.trp-L_1.Heavy-modified";
  EXPECT_STREQ(injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name.c_str(), "trp-L.trp-L_1.Heavy-modified");
  EXPECT_STREQ(injection5.getRawData().getFeatureRSDQC().component_qcs[0].component_name.c_str(), "trp-L.trp-L_1.Heavy-modified");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs[0].component_name.c_str(), "trp-L.trp-L_1.Heavy-modified");
  injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name = "ala-L.ala-L_1.Heavy-mod";
  EXPECT_STREQ(injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name.c_str(), "ala-L.ala-L_1.Heavy-mod");
  EXPECT_STREQ(injection5.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name.c_str(), "ala-L.ala-L_1.Heavy-mod");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs[0].component_name.c_str(), "ala-L.ala-L_1.Heavy-mod");
  injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name = "ala-L.ala-L_1.Heavy-modified";
  EXPECT_STREQ(injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name.c_str(), "ala-L.ala-L_1.Heavy-modified");
  EXPECT_STREQ(injection5.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name.c_str(), "ala-L.ala-L_1.Heavy-modified");
  EXPECT_STREQ(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs[0].component_name.c_str(), "ala-L.ala-L_1.Heavy-modified");

  sequenceHandler.clear();
  Filenames filenames { generateTestFilenames() };
  filenames.sequence_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_empty_sequence.csv");

  cs.filenames_ = filenames;
  cs.process();

  EXPECT_EQ(sequenceHandler.getSequence().size(), 0);
}

TEST(SequenceHandler, gettersCreateSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);

  EXPECT_EQ(cs.getID(), -1);
  EXPECT_STREQ(cs.getName().c_str(), "CREATE_SEQUENCE");
}

TEST(SequenceHandler, processSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames_        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  const vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = { std::make_shared<LoadRawData>() };
  const RawDataHandler& rawDataHandler0 = sequenceHandler.getSequence()[0].getRawData();
  EXPECT_EQ(rawDataHandler0.getExperiment().getChromatograms().size(), 0); // empty (not loaded, yet)

  std::map<std::string, Filenames> dynamic_filenames;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string key = injection.getMetaData().getInjectionName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      path,
      path + "mzML",
      path + "features",
      path + "features",
      injection.getMetaData().getFilename(), // previous: injection.getMetaData().getSampleName(),
      key,
      key,
      injection.getMetaData().getSampleGroupName(),
      injection.getMetaData().getSampleGroupName()
    );
  }

  EXPECT_EQ(sequenceHandler.getSequence().size(), dynamic_filenames.size());

  // Select injection names
  std::set<std::string> injection_names;
  for (int i = 0; i < 2; ++i) injection_names.insert(sequenceHandler.getSequence().at(i).getMetaData().getInjectionName());
  ProcessSequence ps(sequenceHandler);
  ps.filenames_ = dynamic_filenames;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.injection_names_ = injection_names;
  ps.process();
  EXPECT_EQ(sequenceHandler.getSequence().size(), 6);
  int n_chroms = 0;
  for (int i = 0; i < sequenceHandler.getSequence().size(); ++i) n_chroms += sequenceHandler.getSequence().at(i).getRawData().getExperiment().getChromatograms().size();
  EXPECT_EQ(n_chroms, 680); // loaded only the first two injections

  // Default injection names (i.e., the entire sequence)
  ps.injection_names_ = {};
  ps.process();
  EXPECT_EQ(sequenceHandler.getSequence().size(), 6);
  n_chroms = 0;
  for (int i = 0; i < sequenceHandler.getSequence().size(); ++i)
    n_chroms += sequenceHandler.getSequence().at(i).getRawData().getExperiment().getChromatograms().size();
  EXPECT_EQ(n_chroms, 2040); // loaded all injections

  // Test multi threading parameters
  ParameterSet const* params;
  params = &rawDataHandler0.getParameters();
  EXPECT_EQ(params->count("SequenceProcessor"), 1);
  unsigned int n_threads = std::stoul(params->at("SequenceProcessor")[0].getValueAsString());
  EXPECT_EQ(n_threads, 4);

  SmartPeak::SequenceProcessorMultithread spMT1(sequenceHandler.getSequence(),
    dynamic_filenames,
    raw_data_processing_methods);
  SmartPeak::SequenceProcessorMultithread spMT2(sequenceHandler.getSequence(),
    dynamic_filenames,
    raw_data_processing_methods);
  const unsigned int max_threads = std::thread::hardware_concurrency();
  if (max_threads != 0 && 4 <= max_threads) {
    EXPECT_EQ(spMT1.getNumWorkers(4), 3);
    EXPECT_EQ(spMT2.getNumWorkers(3), 2);
  }
  else {
    EXPECT_EQ(spMT1.getNumWorkers(8), 1);
    EXPECT_EQ(spMT2.getNumWorkers(3), 1);
  }
}

TEST(SequenceHandler, gettersProcessSequence)
{
  SequenceHandler sequenceHandler;
  ProcessSequence cs(sequenceHandler);

  EXPECT_EQ(cs.getID(), -1);
  EXPECT_STREQ(cs.getName().c_str(), "PROCESS_SEQUENCE");
}

TEST(SequenceHandler, processSequenceSegments)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames_        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  const vector<std::shared_ptr<SequenceSegmentProcessor>> sequence_segment_processing_methods =
    { std::make_shared<CalculateCalibration>() };

  std::map<std::string, Filenames> dynamic_filenames;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  for (const SequenceSegmentHandler& sequence_segment : sequenceHandler.getSequenceSegments()) {
    const std::string key = sequence_segment.getSequenceSegmentName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      path,
      path + "mzML/",
      path + "features/",
      path + "features/",
      "",
      key,
      key,
      key,
      key
    );
  }

  // Default sequence segment names (i.e., all)
  ProcessSequenceSegments pss(sequenceHandler);
  pss.filenames_                             = dynamic_filenames;
  pss.sequence_segment_processing_methods_ = sequence_segment_processing_methods;
  pss.process();

  EXPECT_EQ(sequenceHandler.getSequenceSegments().size(), 1);

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs = sequenceHandler.getSequenceSegments()[0].getQuantitationMethods();

  EXPECT_EQ(AQMs.size(), 107);

  EXPECT_STREQ(AQMs[0].getComponentName().c_str(), "23dpg.23dpg_1.Light");
  EXPECT_STREQ(AQMs[0].getISName().c_str(), "23dpg.23dpg_1.Heavy");
  EXPECT_STREQ(AQMs[0].getFeatureName().c_str(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("slope")), 2.429728323, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("intercept")), -0.091856745000000004, 1e-6);
  EXPECT_EQ(AQMs[0].getNPoints(), 4);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getCorrelationCoefficient()), 0.98384694900000003, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getLLOQ()), 0.25, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[0].getULOQ()), 2.5, 1e-6);

  EXPECT_STREQ(AQMs[1].getComponentName().c_str(), "35cgmp.35cgmp_1.Light");
  EXPECT_STREQ(AQMs[1].getISName().c_str(), "camp.camp_1.Heavy");
  EXPECT_STREQ(AQMs[1].getFeatureName().c_str(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("slope")), 6.5645316830000002, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("intercept")), -0.0015584049999999999, 1e-6);
  EXPECT_EQ(AQMs[1].getNPoints(), 10);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getCorrelationCoefficient()), 0.99739781999999999, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getLLOQ()), 0.0002, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[1].getULOQ()), 1.0, 1e-6);

  EXPECT_STREQ(AQMs[2].getComponentName().c_str(), "6pgc.6pgc_1.Light");
  EXPECT_STREQ(AQMs[2].getISName().c_str(), "6pgc.6pgc_1.Heavy");
  EXPECT_STREQ(AQMs[2].getFeatureName().c_str(), "peak_apex_int");
  EXPECT_NEAR(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("slope")), 66.39342173, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("intercept")), -0.14264795499999999, 1e-6);
  EXPECT_EQ(AQMs[2].getNPoints(), 7);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getCorrelationCoefficient()), 0.99547012000000001, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getLLOQ()), 0.008, 1e-6);
  EXPECT_NEAR(static_cast<double>(AQMs[2].getULOQ()), 0.8, 1e-6);

  // TODO: Selected sequence segment names
}

TEST(SequenceHandler, gettersProcessSequenceSegments)
{
  SequenceHandler sequenceHandler;
  ProcessSequenceSegments cs(sequenceHandler);

  EXPECT_EQ(cs.getID(), -1);
  EXPECT_STREQ(cs.getName().c_str(), "PROCESS_SEQUENCE_SEGMENTS");
}

TEST(SequenceHandler, processSampleGroups)
{
  // Create the sequence
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames_ = generateTestFilenames();
  cs.delimiter = ",";
  cs.checkConsistency = false;
  cs.process();

  // Generate the filenames
  std::map<std::string, Filenames> dynamic_filenames;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string key = injection.getMetaData().getInjectionName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      path,
      path,
      path,
      path,
      injection.getMetaData().getFilename(),
      key,
      key,
      injection.getMetaData().getSampleGroupName(),
      injection.getMetaData().getSampleGroupName()
    );
  }

  // Load in the raw data featureMaps
  const vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = { std::make_shared<LoadFeatures>() };
  ProcessSequence ps(sequenceHandler);
  ps.filenames_ = dynamic_filenames;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process();

  // Update the filenames
  dynamic_filenames.clear();
  for (const SampleGroupHandler& sampleGroupHandler : sequenceHandler.getSampleGroups()) {
    dynamic_filenames[sampleGroupHandler.getSampleGroupName()] = Filenames::getDefaultDynamicFilenames(
      path,
      path + "mzML/",
      path + "features/",
      path + "features/",
      "",
      sampleGroupHandler.getSampleGroupName(),
      sampleGroupHandler.getSampleGroupName(),
      sampleGroupHandler.getSampleGroupName(),
      sampleGroupHandler.getSampleGroupName()
    );
  }

  const vector<std::shared_ptr<SampleGroupProcessor>> sample_group_processing_methods =
  { std::make_shared<MergeInjections>() };

  // Default sample group names (i.e., all)
  ProcessSampleGroups psg(sequenceHandler);
  psg.filenames_ = dynamic_filenames;
  psg.sample_group_processing_methods_ = sample_group_processing_methods;
  psg.process();

  EXPECT_EQ(sequenceHandler.getSampleGroups().size(), 2);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().at(0);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 117);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 3);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "23dpg");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "23dpg.23dpg_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 305.160126, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 15.7456121, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 170, 1e-4);

  // TODO: Selected sample group names
}

TEST(SequenceHandler, processSampleGroups_no_injections)
{
  // Try to launch ProcessSequence while no injections is set.
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  ProcessSequence ps(sequenceHandler);
  const vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = { std::make_shared<LoadFeatures>() };
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.process();
}

TEST(SequenceHandler, StoreWorkflow_onFilePicked)
{
  namespace fs = std::filesystem;
  ApplicationHandler application_handler;
  std::vector<std::string> command_names = {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
  };
  application_handler.sequenceHandler_.setWorkflow(command_names);
  StoreWorkflow store_workflow(application_handler.sequenceHandler_);
  std::string filename = std::tmpnam(nullptr);
  ASSERT_TRUE(store_workflow.onFilePicked(filename, &application_handler));
}

TEST(SequenceHandler, StoreWorkflow1)
{
  SequenceHandler sequenceHandler;
  namespace fs = std::filesystem;
  std::vector<std::string> command_names = {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
  };
  sequenceHandler.setWorkflow(command_names);
  StoreWorkflow processor(sequenceHandler);
  processor.filename_ = (SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_workflow.csv"));

  processor.process();
  // compare with reference file
  const string reference_filename = SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_workflow.csv");
  EXPECT_STREQ(processor.filename_.c_str(), reference_filename.c_str());
}

TEST(SequenceHandler, LoadWorkflow_onFilePicked)
{
  ApplicationHandler application_handler;
  LoadWorkflow load_workflow(application_handler.sequenceHandler_);
  std::string filename = SMARTPEAK_GET_TEST_DATA_PATH("ApplicationProcessor_workflow.csv");
  ASSERT_TRUE(load_workflow.onFilePicked(filename, &application_handler));
  const auto& commands = application_handler.sequenceHandler_.getWorkflow();
  std::vector<std::string> expected_command_names = {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
  };
  ASSERT_TRUE(commands.size() == expected_command_names.size());
  for (auto i = 0; i < expected_command_names.size(); ++i)
  {
    EXPECT_EQ(expected_command_names[i], commands[i]);
  }
}

TEST(SequenceHandler, LoadWorkflow1)
{
  SequenceHandler sequenceHandler;
  struct WorkflowObserverTest : public IWorkflowObserver
  {
    virtual void onWorkflowUpdated() override
    {
      nb_notifications_++;
    }
    int nb_notifications_ = 0;
  } workflow_observer;
  sequenceHandler.addWorkflowObserver(&workflow_observer);
  LoadWorkflow processor(sequenceHandler);
  processor.filename_ = SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_workflow.csv");
  processor.process();
  const auto& commands = sequenceHandler.getWorkflow();
  std::vector<std::string> expected_command_names = {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
  };
  EXPECT_TRUE(commands.size() == expected_command_names.size());
  for (auto i = 0; i < expected_command_names.size(); ++i)
  {
    EXPECT_STREQ(expected_command_names[i].c_str(), commands[i].c_str());
  }
  EXPECT_EQ(workflow_observer.nb_notifications_, 1);
}

TEST(SequenceHandler, gettersProcessSampleGroups)
{
  SequenceHandler sequenceHandler;
  ProcessSampleGroups cs(sequenceHandler);

  EXPECT_EQ(cs.getID(), -1);
  EXPECT_STREQ(cs.getName().c_str(), "PROCESS_SAMPLE_GROUPS");
}
