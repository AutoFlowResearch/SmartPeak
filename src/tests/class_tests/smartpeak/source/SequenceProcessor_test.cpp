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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <boost/filesystem.hpp>

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

BOOST_AUTO_TEST_SUITE(sequenceprocessor)

BOOST_AUTO_TEST_CASE(createSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames_        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  // Test initialization of the sequence
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  InjectionHandler& injection0 = sequenceHandler.getSequence()[0];
  BOOST_CHECK_EQUAL(injection0.getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(injection0.getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(injection0.getRawData().getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(injection0.getRawData().getParameters().size(), 23);
  BOOST_CHECK_EQUAL(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName(), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(injection0.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection0.getRawData().getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(injection0.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  InjectionHandler& injection5 = sequenceHandler.getSequence()[5];
  BOOST_CHECK_EQUAL(injection5.getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(injection5.getMetaData().getSampleGroupName(), "Test02");
  BOOST_CHECK_EQUAL(injection5.getRawData().getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");
  BOOST_CHECK_EQUAL(injection5.getRawData().getParameters().size(), 23);
  BOOST_CHECK_EQUAL(injection5.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName(), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(injection5.getRawData().getTargetedExperiment().getTransitions().size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(injection5.getRawData().getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(injection5.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 1);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods().size(), 107);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs.size(), 324);
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy");

  // Test non-shared resources
  injection0.getMetaData().setSampleName("modified");
  BOOST_CHECK_EQUAL(injection0.getRawData().getMetaData().getSampleName(), "modified");
  BOOST_CHECK_EQUAL(injection5.getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr3_1x");

  // Test shared resources between all raw data handlers
  injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].setName("modified");
  BOOST_CHECK_EQUAL(injection0.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName(), "modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getParameters().at("MRMFeatureFinderScoring")[0].getName(), "modified");
  auto transitions = injection0.getRawData().getTargetedExperiment().getTransitions();
  transitions[0].setPeptideRef("arg-L-mod");
  injection0.getRawData().getTargetedExperiment().setTransitions(transitions);
  BOOST_CHECK_EQUAL(injection0.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getTargetedExperiment().getTransitions()[0].getPeptideRef(), "arg-L-mod");

  // Test shared resources between sequence segment handlers
  injection0.getRawData().getQuantitationMethods()[0].setComponentName("23dpg.23dpg_1.Light-mod");
  BOOST_CHECK_EQUAL(injection0.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getQuantitationMethods()[0].getComponentName(), "23dpg.23dpg_1.Light-mod");
  injection0.getRawData().getFeatureFilter().component_qcs[0].component_name = "arg-L.arg-L_1.Heavy-mod";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureFilter().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-mod");
  injection0.getRawData().getFeatureQC().component_qcs[0].component_name = "arg-L.arg-L_1.Heavy-modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureQC().component_qcs[0].component_name, "arg-L.arg-L_1.Heavy-modified");
  injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name = "trp-L.trp-L_1.Heavy-mod";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDFilter().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDFilter().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-mod");
  injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name = "trp-L.trp-L_1.Heavy-modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureRSDQC().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureRSDQC().component_qcs[0].component_name, "trp-L.trp-L_1.Heavy-modified");
  injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name = "ala-L.ala-L_1.Heavy-mod";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundFilter().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-mod");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundFilter().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-mod");
  injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name = "ala-L.ala-L_1.Heavy-modified";
  BOOST_CHECK_EQUAL(injection0.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(injection5.getRawData().getFeatureBackgroundQC().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-modified");
  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments()[0].getFeatureBackgroundQC().component_qcs[0].component_name, "ala-L.ala-L_1.Heavy-modified");

  sequenceHandler.clear();
  Filenames filenames { generateTestFilenames() };
  filenames.sequence_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_empty_sequence.csv");

  cs.filenames_ = filenames;
  cs.process();

  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 0);
}

BOOST_AUTO_TEST_CASE(gettersCreateSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);

  BOOST_CHECK_EQUAL(cs.getID(), -1);
  BOOST_CHECK_EQUAL(cs.getName(), "CREATE_SEQUENCE");
}

BOOST_AUTO_TEST_CASE(processSequence)
{
  SequenceHandler sequenceHandler;
  CreateSequence cs(sequenceHandler);
  cs.filenames_        = generateTestFilenames();
  cs.delimiter        = ",";
  cs.checkConsistency = false;
  cs.process();

  const vector<std::shared_ptr<RawDataProcessor>> raw_data_processing_methods = { std::make_shared<LoadRawData>() };
  const RawDataHandler& rawDataHandler0 = sequenceHandler.getSequence()[0].getRawData();
  BOOST_CHECK_EQUAL(rawDataHandler0.getExperiment().getChromatograms().size(), 0); // empty (not loaded, yet)

  std::map<std::string, Filenames> dynamic_filenames;
  const std::string path = SMARTPEAK_GET_TEST_DATA_PATH("");
  for (const InjectionHandler& injection : sequenceHandler.getSequence()) {
    const std::string key = injection.getMetaData().getInjectionName();
    dynamic_filenames[key] = Filenames::getDefaultDynamicFilenames(
      path + "mzML/",
      path + "features/",
      path + "features/",
      injection.getMetaData().getFilename(), // previous: injection.getMetaData().getSampleName(),
      key,
      key,
      injection.getMetaData().getSampleGroupName(),
      injection.getMetaData().getSampleGroupName()
    );
  }

  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), dynamic_filenames.size());

  // Select injection names
  std::set<std::string> injection_names;
  for (int i = 0; i < 2; ++i) injection_names.insert(sequenceHandler.getSequence().at(i).getMetaData().getInjectionName());
  ProcessSequence ps(sequenceHandler);
  ps.filenames_ = dynamic_filenames;
  ps.raw_data_processing_methods_ = raw_data_processing_methods;
  ps.injection_names_ = injection_names;
  ps.process();
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  int n_chroms = 0;
  for (int i = 0; i < sequenceHandler.getSequence().size(); ++i) n_chroms += sequenceHandler.getSequence().at(i).getRawData().getExperiment().getChromatograms().size();
  BOOST_CHECK_EQUAL(n_chroms, 680); // loaded only the first two injections

  // Default injection names (i.e., the entire sequence)
  ps.injection_names_ = {};
  ps.process();
  BOOST_CHECK_EQUAL(sequenceHandler.getSequence().size(), 6);
  n_chroms = 0;
  for (int i = 0; i < sequenceHandler.getSequence().size(); ++i) n_chroms += sequenceHandler.getSequence().at(i).getRawData().getExperiment().getChromatograms().size();
  BOOST_CHECK_EQUAL(n_chroms, 2040); // loaded all injections

  // Test multi threading parameters  
  ParameterSet const* params;
  params = &rawDataHandler0.getParameters();
  BOOST_CHECK_EQUAL(params->count("SequenceProcessor"), 1);
  unsigned int n_threads = std::stoul(params->at("SequenceProcessor")[0].getValueAsString());
  BOOST_CHECK_EQUAL(n_threads, 4);
  
  SmartPeak::SequenceProcessorMultithread spMT1(sequenceHandler.getSequence(),
    dynamic_filenames,
    raw_data_processing_methods);
  SmartPeak::SequenceProcessorMultithread spMT2(sequenceHandler.getSequence(),
    dynamic_filenames,
    raw_data_processing_methods);
  const unsigned int max_threads = std::thread::hardware_concurrency();
  if (max_threads != 0 && 4 <= max_threads) {
    BOOST_CHECK_EQUAL(spMT1.getNumWorkers(4), 3);
    BOOST_CHECK_EQUAL(spMT2.getNumWorkers(3), 2);
  }
  else {
    BOOST_CHECK_EQUAL(spMT1.getNumWorkers(8), 1);
    BOOST_CHECK_EQUAL(spMT2.getNumWorkers(3), 1);
  }
}

BOOST_AUTO_TEST_CASE(gettersProcessSequence)
{
  SequenceHandler sequenceHandler;
  ProcessSequence cs(sequenceHandler);

  BOOST_CHECK_EQUAL(cs.getID(), -1);
  BOOST_CHECK_EQUAL(cs.getName(), "PROCESS_SEQUENCE");
}

BOOST_AUTO_TEST_CASE(processSequenceSegments)
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

  BOOST_CHECK_EQUAL(sequenceHandler.getSequenceSegments().size(), 1);

  const std::vector<OpenMS::AbsoluteQuantitationMethod>& AQMs = sequenceHandler.getSequenceSegments()[0].getQuantitationMethods();

  BOOST_CHECK_EQUAL(AQMs.size(), 107);

  BOOST_CHECK_EQUAL(AQMs[0].getComponentName(), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(AQMs[0].getISName(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[0].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("slope")), 2.429728323, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getTransformationModelParams().getValue("intercept")), -0.091856745000000004, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[0].getNPoints(), 4);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getCorrelationCoefficient()), 0.98384694900000003, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getLLOQ()), 0.25, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[0].getULOQ()), 2.5, 1e-6);

  BOOST_CHECK_EQUAL(AQMs[1].getComponentName(), "35cgmp.35cgmp_1.Light");
  BOOST_CHECK_EQUAL(AQMs[1].getISName(), "camp.camp_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[1].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("slope")), 6.5645316830000002, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getTransformationModelParams().getValue("intercept")), -0.0015584049999999999, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[1].getNPoints(), 10);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getCorrelationCoefficient()), 0.99739781999999999, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getLLOQ()), 0.0002, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[1].getULOQ()), 1.0, 1e-6);

  BOOST_CHECK_EQUAL(AQMs[2].getComponentName(), "6pgc.6pgc_1.Light");
  BOOST_CHECK_EQUAL(AQMs[2].getISName(), "6pgc.6pgc_1.Heavy");
  BOOST_CHECK_EQUAL(AQMs[2].getFeatureName(), "peak_apex_int");
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("slope")), 66.39342173, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getTransformationModelParams().getValue("intercept")), -0.14264795499999999, 1e-6);
  BOOST_CHECK_EQUAL(AQMs[2].getNPoints(), 7);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getCorrelationCoefficient()), 0.99547012000000001, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getLLOQ()), 0.008, 1e-6);
  BOOST_CHECK_CLOSE(static_cast<double>(AQMs[2].getULOQ()), 0.8, 1e-6);

  // TODO: Selected sequence segment names
}

BOOST_AUTO_TEST_CASE(gettersProcessSequenceSegments)
{
  SequenceHandler sequenceHandler;
  ProcessSequenceSegments cs(sequenceHandler);

  BOOST_CHECK_EQUAL(cs.getID(), -1);
  BOOST_CHECK_EQUAL(cs.getName(), "PROCESS_SEQUENCE_SEGMENTS");
}

BOOST_AUTO_TEST_CASE(processSampleGroups)
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

  BOOST_CHECK_EQUAL(sequenceHandler.getSampleGroups().size(), 2);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().at(0);

  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().size(), 117);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 3);
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString(), "23dpg");
  BOOST_CHECK_EQUAL(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString(), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 305.160126, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 15.7456121, 1e-4);
  BOOST_CHECK_CLOSE(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 170, 1e-4);

  // TODO: Selected sample group names
}

BOOST_AUTO_TEST_CASE(StoreWorkflow1)
{
  SequenceHandler sequenceHandler;
  namespace fs = boost::filesystem;
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
  processor.filename_ = (fs::temp_directory_path().append(fs::unique_path().string())).string();
  processor.process();
  // compare with reference file
  const string reference_filename = SMARTPEAK_GET_TEST_DATA_PATH("SequenceProcessor_workflow.csv");
  std::ifstream created_if(processor.filename_);
  std::ifstream reference_if(reference_filename);
  std::istream_iterator<char> created_is(created_if), created_end;
  std::istream_iterator<char> reference_is(reference_if), reference_end;
  BOOST_CHECK_EQUAL_COLLECTIONS(created_is, created_end, reference_is, reference_end);
}

BOOST_AUTO_TEST_CASE(LoadWorkflow1)
{
  SequenceHandler sequenceHandler;
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
  BOOST_REQUIRE(commands.size() == expected_command_names.size());
  for (auto i = 0; i < expected_command_names.size(); ++i)
  {
    BOOST_CHECK_EQUAL(expected_command_names[i], commands[i]);
  }
}

BOOST_AUTO_TEST_CASE(gettersProcessSampleGroups)
{
  SequenceHandler sequenceHandler;
  ProcessSampleGroups cs(sequenceHandler);

  BOOST_CHECK_EQUAL(cs.getID(), -1);
  BOOST_CHECK_EQUAL(cs.getName(), "PROCESS_SAMPLE_GROUPS");
}

BOOST_AUTO_TEST_SUITE_END()
