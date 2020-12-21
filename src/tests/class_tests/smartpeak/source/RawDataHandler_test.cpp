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

#define BOOST_TEST_MODULE RawDataHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/RawDataHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(rawdatahandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  RawDataHandler* ptr = nullptr;
  RawDataHandler* nullPointer = nullptr;
  ptr = new RawDataHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
  delete ptr;
}

BOOST_AUTO_TEST_CASE(set_get_FeatureMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMap(f1);

  const OpenMS::FeatureMap& f2 = rawDataHandler.getFeatureMap(); // testing const getter
  BOOST_CHECK_EQUAL(f2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMap().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMap();
  BOOST_CHECK_EQUAL(f3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_FeatureMapHistory)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMapHistory(f1);

  const OpenMS::FeatureMap& f2 = rawDataHandler.getFeatureMapHistory(); // testing const getter
  BOOST_CHECK_EQUAL(f2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f2.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMapHistory().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMapHistory();
  BOOST_CHECK_EQUAL(f3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(f3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(f3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_MetaData)
{
  RawDataHandler rawDataHandler;

  MetaDataHandler m1;
  const string name {"foo"};
  m1.setSampleName(name);

  rawDataHandler.setMetaData(m1);

  const MetaDataHandler& m2 = rawDataHandler.getMetaData(); // testing const getter
  BOOST_CHECK_EQUAL(m2.getSampleName(), name);
  std::shared_ptr<MetaDataHandler> m2shared = rawDataHandler.getMetaDataShared();
  BOOST_CHECK_EQUAL(m2shared->getSampleName(), name);

  const string group_name {"bar"};
  rawDataHandler.getMetaData().setSampleGroupName(group_name); // testing non-const getter

  const MetaDataHandler& m3 = rawDataHandler.getMetaData();
  BOOST_CHECK_EQUAL(m3.getSampleName(), name);
  BOOST_CHECK_EQUAL(m3.getSampleGroupName(), group_name);
  std::shared_ptr<MetaDataHandler> m3shared = rawDataHandler.getMetaDataShared();
  BOOST_CHECK_EQUAL(m3shared->getSampleName(), name);
  BOOST_CHECK_EQUAL(m3shared->getSampleGroupName(), group_name);
}

BOOST_AUTO_TEST_CASE(set_get_Parameters)
{
  RawDataHandler rawDataHandler;

  map<string, string> m1 = {
    {"name", "stop_report_after_feature"},
    {"value", "-1"}
  };
  vector<map<string, string>> v1;
  v1.push_back(m1);
  map<string, vector<map<string, string>>> parameters1;
  const string name1 {"MRMFeatureFinderScoring"};
  parameters1.insert({name1, v1});

  rawDataHandler.setParameters(parameters1);

  const map<string, vector<map<string, string>>>& parameters2 = rawDataHandler.getParameters();
  BOOST_CHECK_EQUAL(parameters2.count(name1), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1).size(), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters2.at(name1)[0].at("value"), "-1");
  std::shared_ptr< map<string, vector<map<string, string>>>> parameters2shared = rawDataHandler.getParametersShared();
  BOOST_CHECK_EQUAL(parameters2shared->count(name1), 1);
  BOOST_CHECK_EQUAL(parameters2shared->at(name1).size(), 1);
  BOOST_CHECK_EQUAL(parameters2shared->at(name1)[0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters2shared->at(name1)[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(parameters2shared->at(name1)[0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters2shared->at(name1)[0].at("value"), "-1");

  rawDataHandler.getParameters().at(name1)[0].insert({"type", "int"});

  const map<string, vector<map<string, string>>>& parameters3 = rawDataHandler.getParameters();
  BOOST_CHECK_EQUAL(parameters3.count(name1), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1).size(), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].at("value"), "-1");
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters3.at(name1)[0].at("type"), "int");
  std::shared_ptr< map<string, vector<map<string, string>>>> parameters3shared = rawDataHandler.getParametersShared();
  BOOST_CHECK_EQUAL(parameters3shared->count(name1), 1);
  BOOST_CHECK_EQUAL(parameters3shared->at(name1).size(), 1);
  BOOST_CHECK_EQUAL(parameters3shared->at(name1)[0].count("name"), 1);
  BOOST_CHECK_EQUAL(parameters3shared->at(name1)[0].at("name"), "stop_report_after_feature");
  BOOST_CHECK_EQUAL(parameters3shared->at(name1)[0].count("value"), 1);
  BOOST_CHECK_EQUAL(parameters3shared->at(name1)[0].at("value"), "-1");
  BOOST_CHECK_EQUAL(parameters3shared->at(name1)[0].count("type"), 1);
  BOOST_CHECK_EQUAL(parameters3shared->at(name1)[0].at("type"), "int");
}

BOOST_AUTO_TEST_CASE(set_get_QuantitationMethods)
{
  RawDataHandler rawDataHandler;

  OpenMS::AbsoluteQuantitationMethod aqm;
  const string name {"foo"};
  aqm.setComponentName(name);

  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1;
  AQMs1.push_back(aqm);

  rawDataHandler.setQuantitationMethods(AQMs1);

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs2 = rawDataHandler.getQuantitationMethods(); // testing const getter
  BOOST_CHECK_EQUAL(AQMs2.size(), 1);
  BOOST_CHECK_EQUAL(AQMs2[0].getComponentName(), name);
  std::shared_ptr<vector<OpenMS::AbsoluteQuantitationMethod>>& AQMs2shared = rawDataHandler.getQuantitationMethodsShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(AQMs2shared->size(), 1);
  BOOST_CHECK_EQUAL(AQMs2shared->at(0).getComponentName(), name);

  const string feature_name {"bar"};
  rawDataHandler.getQuantitationMethods()[0].setFeatureName(feature_name); // testing non-const getter

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs3 = rawDataHandler.getQuantitationMethods();
  BOOST_CHECK_EQUAL(AQMs3[0].getComponentName(), name);
  BOOST_CHECK_EQUAL(AQMs3[0].getFeatureName(), feature_name);
  std::shared_ptr<vector<OpenMS::AbsoluteQuantitationMethod>>& AQMs3shared = rawDataHandler.getQuantitationMethodsShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(AQMs3shared->at(0).getComponentName(), name);
  BOOST_CHECK_EQUAL(AQMs3shared->at(0).getFeatureName(), feature_name);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureFilter(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureFilterShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureFilter();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureFilterShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureQC(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureQCShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureQC();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureQCShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureRSDFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureRSDFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureRSDFilter(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureRSDFilterShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureRSDFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureRSDFilter();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureRSDFilterShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureRSDQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureRSDQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureRSDQC(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureRSDQCShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureRSDQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureRSDQC();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureRSDQCShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureBackgroundFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureBackgroundFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureBackgroundFilter(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureBackgroundFilterShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureBackgroundFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureBackgroundFilter();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureBackgroundFilterShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureBackgroundQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureBackgroundQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureBackgroundQC(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureBackgroundQCShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureBackgroundQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureBackgroundQC();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureBackgroundQCShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureRSDEstimations)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureRSDEstimations(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureRSDEstimations(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureRSDEstimationsShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureRSDEstimations().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureRSDEstimations();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureRSDEstimationsShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_FeatureBackgroundEstimations)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureBackgroundEstimations(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureBackgroundEstimations(); // testing const getter
  BOOST_CHECK_EQUAL(fqc2.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2.component_qcs[0].component_name, name);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureBackgroundEstimationsShared(); // testing shared_ptr getter
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc2shared->component_qcs[0].component_name, name);

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureBackgroundEstimations().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureBackgroundEstimations();
  BOOST_CHECK_EQUAL(fqc3.component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureBackgroundEstimationsShared();
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs.size(), 1);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].component_name, name);
  BOOST_CHECK_EQUAL(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

BOOST_AUTO_TEST_CASE(set_get_Experiment)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");

  rawDataHandler.setExperiment(experiment);

  const OpenMS::MSExperiment& experiment2 = rawDataHandler.getExperiment(); // testing const getter
  BOOST_CHECK_EQUAL(experiment2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(experiment2.getMetaValue("name"), "foo");

  rawDataHandler.getExperiment().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& experiment3 = rawDataHandler.getExperiment();
  BOOST_CHECK_EQUAL(experiment3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(experiment3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(experiment3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(experiment3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_ChromatogramMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment chromatogram_map;
  chromatogram_map.setMetaValue("name", "foo");

  rawDataHandler.setChromatogramMap(chromatogram_map);

  const OpenMS::MSExperiment& chromatogram_map2 = rawDataHandler.getChromatogramMap(); // testing const getter
  BOOST_CHECK_EQUAL(chromatogram_map2.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(chromatogram_map2.getMetaValue("name"), "foo");

  rawDataHandler.getChromatogramMap().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& chromatogram_map3 = rawDataHandler.getChromatogramMap();
  BOOST_CHECK_EQUAL(chromatogram_map3.metaValueExists("name"), true);
  BOOST_CHECK_EQUAL(chromatogram_map3.getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(chromatogram_map3.metaValueExists("name2"), true);
  BOOST_CHECK_EQUAL(chromatogram_map3.getMetaValue("name2"), "bar");
}

BOOST_AUTO_TEST_CASE(set_get_ReferenceData)
{
  RawDataHandler rawDataHandler;
  std::vector<std::map<std::string, CastValue>> data;
  CastValue c("bar");

  map<string, CastValue> m /*= {{"foo", c}}*/;
  m.emplace("foo", c);
  c = "bar2";
  m.emplace("foo2", c);
  data.push_back(m);

  rawDataHandler.setReferenceData(data);

  const std::vector<std::map<std::string, CastValue>>& data2 = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(data2.size(), 1);
  BOOST_CHECK_EQUAL(data2[0].count("foo"), 1);
  BOOST_CHECK_EQUAL(data2[0].at("foo").s_, "bar");
  BOOST_CHECK_EQUAL(data2[0].count("foo2"), 1);
  BOOST_CHECK_EQUAL(data2[0].at("foo2").s_, "bar2");

  c = "bar3";

  rawDataHandler.getReferenceData().push_back({{"foo3", c}});

  const std::vector<std::map<std::string, CastValue>>& data3 = rawDataHandler.getReferenceData();

  BOOST_CHECK_EQUAL(data3.size(), 2);
  BOOST_CHECK_EQUAL(data3[0].count("foo"), 1);
  BOOST_CHECK_EQUAL(data3[0].at("foo").s_, "bar");
  BOOST_CHECK_EQUAL(data3[0].count("foo2"), 1);
  BOOST_CHECK_EQUAL(data3[0].at("foo2").s_, "bar2");
  BOOST_CHECK_EQUAL(data3[1].count("foo3"), 1);
  BOOST_CHECK_EQUAL(data3[1].at("foo3").s_, "bar3");
}

BOOST_AUTO_TEST_CASE(set_get_MzTab)
{
  RawDataHandler rawDataHandler;

  OpenMS::MzTab mz_tab;
  OpenMS::MzTabMetaData md;
  md.mz_tab_type.fromCellString("Quantification");
  mz_tab.setMetaData(md);

  rawDataHandler.setMzTab(mz_tab);

  const OpenMS::MzTab& mztab1 = rawDataHandler.getMzTab(); // testing const getter
  BOOST_CHECK_EQUAL(mztab1.getMetaData().mz_tab_type.get(), "Quantification");

  md.mz_tab_type.fromCellString("Qualification");
  rawDataHandler.getMzTab().setMetaData(md); // testing non-const getter

  const OpenMS::MzTab& mztab2 = rawDataHandler.getMzTab();
  BOOST_CHECK_EQUAL(mztab2.getMetaData().mz_tab_type.get(), "Qualification");
}

BOOST_AUTO_TEST_CASE(clear)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");
  rawDataHandler.setFeatureMap(f1);

  MetaDataHandler m1;
  m1.setSampleName("foo");
  rawDataHandler.setMetaData(m1);

  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("foo");
  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1 = { aqm };
  rawDataHandler.setQuantitationMethods(AQMs1);

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  qc.component_name = "foo";
  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);
  rawDataHandler.setFeatureFilter(fqc1);
  rawDataHandler.setFeatureQC(fqc1);
  rawDataHandler.setFeatureRSDFilter(fqc1);
  rawDataHandler.setFeatureRSDQC(fqc1);
  rawDataHandler.setFeatureBackgroundFilter(fqc1);
  rawDataHandler.setFeatureBackgroundQC(fqc1);
  rawDataHandler.setFeatureRSDEstimations(fqc1);
  rawDataHandler.setFeatureBackgroundEstimations(fqc1);

  rawDataHandler.setFeatureMapHistory(f1);

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");
  rawDataHandler.setExperiment(experiment);

  OpenMS::MSExperiment chromatogram_map;
  chromatogram_map.setMetaValue("name", "foo");
  rawDataHandler.setChromatogramMap(chromatogram_map);

  OpenMS::MzTab mz_tab;
  OpenMS::MzTabMetaData md;
  md.mz_tab_type.fromCellString("foo");
  mz_tab.setMetaData(md);
  rawDataHandler.setMzTab(mz_tab);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getQuantitationMethods().front().getComponentName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getChromatogramMap().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDEstimations().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get(), "foo");

  rawDataHandler.clear();

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "");
  BOOST_CHECK_EQUAL(rawDataHandler.getQuantitationMethods().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureFilter().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getChromatogramMap().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDFilter().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDQC().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundFilter().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundQC().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDEstimations().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get(), "");
}

BOOST_AUTO_TEST_CASE(clearNonSharedData)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");
  rawDataHandler.setFeatureMap(f1);

  MetaDataHandler m1;
  m1.setSampleName("foo");
  rawDataHandler.setMetaData(m1);

  OpenMS::AbsoluteQuantitationMethod aqm;
  aqm.setComponentName("foo");
  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1 = { aqm };
  rawDataHandler.setQuantitationMethods(AQMs1);

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  qc.component_name = "foo";
  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);
  rawDataHandler.setFeatureFilter(fqc1);
  rawDataHandler.setFeatureQC(fqc1);
  rawDataHandler.setFeatureRSDFilter(fqc1);
  rawDataHandler.setFeatureRSDQC(fqc1);
  rawDataHandler.setFeatureBackgroundFilter(fqc1);
  rawDataHandler.setFeatureBackgroundQC(fqc1);
  rawDataHandler.setFeatureRSDEstimations(fqc1);
  rawDataHandler.setFeatureBackgroundEstimations(fqc1);

  rawDataHandler.setFeatureMapHistory(f1);

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");
  rawDataHandler.setExperiment(experiment);

  OpenMS::MSExperiment chromatogram_map;
  chromatogram_map.setMetaValue("name", "foo");
  rawDataHandler.setChromatogramMap(chromatogram_map);

  OpenMS::MzTab mz_tab;
  OpenMS::MzTabMetaData md;
  md.mz_tab_type.fromCellString("foo");
  mz_tab.setMetaData(md);
  rawDataHandler.setMzTab(mz_tab);

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getQuantitationMethods().front().getComponentName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getChromatogramMap().getMetaValue("name"), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDEstimations().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get(), "foo");

  rawDataHandler.clearNonSharedData();

  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getMetaData().getSampleName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getQuantitationMethods().front().getComponentName(), "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getExperiment().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getChromatogramMap().size(), 0);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundFilter().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundQC().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureRSDEstimations().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.front().component_name, "foo");
  BOOST_CHECK_EQUAL(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get(), "");
}

BOOST_AUTO_TEST_CASE(updateFeatureMapHistory)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap fm1;
  fm1.setPrimaryMSRunPath({"sample_1"});
  OpenMS::Feature f1, f2, s1a, s1b, s2a, s2b;
  f1.setMetaValue("PeptideRef", "component_group_1");  
  f1.setUniqueId("f_1"); // ASSUMPTION: unique id attribute will be set within OpenMS
  s1a.setMetaValue("native_id", "component_1a");
  f1.setSubordinates({s1a});
  fm1.push_back(f1);

  rawDataHandler.setFeatureMap(fm1);

  // Test empty feature_map_history with new feature_map
  rawDataHandler.updateFeatureMapHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);

  // Test empty feature_map_history with a feature_map with "used_" attribute annotated
  s1a.setMetaValue("used_", "false");
  s1a.setMetaValue("timestamp_", "now");
  fm1[0].setSubordinates({ s1a });
  rawDataHandler.clear();
  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("timestamp_"), "now");

  // Test new subordinate
  s1a.clearMetaInfo();
  s1a.setMetaValue("native_id", "component_1a");
  s1b.setMetaValue("native_id", "component_1b");
  fm1[0].setSubordinates({s1a, s1b});

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool(), true);

  // Test filtered subordinates
  fm1[0].setSubordinates({ s1a });

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool(), false);

  // Test new feature
  f2.setMetaValue("PeptideRef", "component_group_2");
  f2.setUniqueId("f_2"); // ASSUMPTION: unique id attribute will be set within OpenMS
  s2a.setMetaValue("native_id", "component_2a");
  s2b.setMetaValue("native_id", "component_2b");
  f2.setSubordinates({ s2a, s2b });
  fm1.push_back(f2);

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getUniqueId(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getMetaValue("PeptideRef"), "component_group_2");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("native_id"), "component_2a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("native_id"), "component_2b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("used_").toBool(), true);

  // Test removed feature
  fm1.clear();
  fm1.push_back(f2);

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool(), false);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getUniqueId(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getMetaValue("PeptideRef"), "component_group_2");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("native_id"), "component_2a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("native_id"), "component_2b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("used_").toBool(), true);
}

BOOST_AUTO_TEST_CASE(makeFeatureMapFromHistory)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap fm1;
  fm1.setPrimaryMSRunPath({ "sample_1" });
  OpenMS::Feature f1, f2, s1a, s1b, s2a, s2b;
  f1.setMetaValue("PeptideRef", "component_group_1");
  f1.setUniqueId("f_1"); // ASSUMPTION: unique id attribute will be set within OpenMS
  s1a.setMetaValue("native_id", "component_1a");
  s1a.setMetaValue("used_", "true");
  s1a.setMetaValue("timestamp_", "now");
  f1.setSubordinates({ s1a });
  fm1.push_back(f1);

  rawDataHandler.setFeatureMapHistory(fm1);

  // Test empty feature_map_history with new feature_map
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);

  // Test empty feature_map_history with a feature_map with "used_" attribute annotated
  s1a.setMetaValue("used_", "false");
  s1a.setMetaValue("timestamp_", "now");
  fm1[0].setSubordinates({ s1a });
  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 0);

  // Test multiple subordinate
  s1a.clearMetaInfo();
  s1a.setMetaValue("native_id", "component_1a");
  s1a.setMetaValue("used_", "true");
  s1b.setMetaValue("native_id", "component_1b");
  s1b.setMetaValue("used_", "true");
  fm1[0].setSubordinates({ s1a, s1b });

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("used_").toBool(), true);

  // Test multiple feature
  f2.setMetaValue("PeptideRef", "component_group_2");
  f2.setUniqueId("f_2"); // ASSUMPTION: unique id attribute will be set within OpenMS
  s2a.setMetaValue("native_id", "component_2a");
  s2a.setMetaValue("used_", "true");
  s2b.setMetaValue("native_id", "component_2b");
  s2b.setMetaValue("used_", "true");
  f2.setSubordinates({ s2a, s2b });
  fm1.push_back(f2);

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getUniqueId(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getMetaValue("PeptideRef"), "component_group_2");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getSubordinates()[0].getMetaValue("native_id"), "component_2a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getSubordinates()[1].getMetaValue("native_id"), "component_2b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[1].getSubordinates()[1].getMetaValue("used_").toBool(), true);

  // Test multiple feature with filtered subordinates
  s2a.setMetaValue("native_id", "component_2a");
  s2a.setMetaValue("used_", "false");
  s2b.setMetaValue("native_id", "component_2b");
  s2b.setMetaValue("used_", "false");
  fm1[1].setSubordinates({ s2a, s2b });

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id"), "component_1a");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool(), true);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("native_id"), "component_1b");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("used_").toBool(), true);

  // Test feature level
  fm1.clear();  
  f1.setMetaValue("PeptideRef", "component_group_1");
  f1.setMetaValue("used_", "true");
  f1.setUniqueId("f_1"); // ASSUMPTION: unique id attribute will be set within OpenMS
  f1.setSubordinates(std::vector<OpenMS::Feature>());
  fm1.push_back(f1);

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef"), "component_group_1");
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 0);

  // Test feature level
  fm1.clear();
  f1.setMetaValue("PeptideRef", "component_group_1");
  f1.setMetaValue("used_", "false");
  f1.setUniqueId("f_1"); // ASSUMPTION: unique id attribute will be set within OpenMS
  fm1.push_back(f1);

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  BOOST_CHECK_EQUAL(rawDataHandler.getFeatureMap().size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
