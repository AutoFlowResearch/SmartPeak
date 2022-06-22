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
#include <SmartPeak/core/RawDataHandler.h>

using namespace SmartPeak;
using namespace std;

TEST(RawDataHandler, constructor)
{
  RawDataHandler* ptr = nullptr;
  RawDataHandler* nullPointer = nullptr;
  ptr = new RawDataHandler();
  EXPECT_NE(ptr, nullPointer);
  delete ptr;
}

TEST(RawDataHandler, set_get_FeatureMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMap(f1);

  const OpenMS::FeatureMap& f2 = rawDataHandler.getFeatureMap(); // testing const getter
  EXPECT_TRUE(f2.metaValueExists("name"));
  EXPECT_EQ(f2.getMetaValue("name"), "foo");

  rawDataHandler.getFeatureMap().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMap();
  EXPECT_TRUE(f3.metaValueExists("name"));
  EXPECT_EQ(f3.getMetaValue("name"), "foo");
  EXPECT_TRUE(f3.metaValueExists("name2"));
  EXPECT_EQ(f3.getMetaValue("name2"), "bar");
}

TEST(RawDataHandler, set_get_FeatureMapHistory)
{
  RawDataHandler rawDataHandler;

  OpenMS::FeatureMap f1;
  f1.setMetaValue("name", "foo");

  rawDataHandler.setFeatureMapHistory(f1);

  const OpenMS::FeatureMap& f2 = rawDataHandler.getFeatureMapHistory(); // testing const getter
  EXPECT_TRUE(f2.metaValueExists("name"));
  EXPECT_STREQ(((std::string)f2.getMetaValue("name")).c_str(), "foo");

  rawDataHandler.getFeatureMapHistory().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::FeatureMap& f3 = rawDataHandler.getFeatureMapHistory();
  EXPECT_TRUE(f3.metaValueExists("name"));
  EXPECT_STREQ(((std::string)f3.getMetaValue("name")).c_str(), "foo");
  EXPECT_TRUE(f3.metaValueExists("name2"));
  EXPECT_STREQ(((std::string)f3.getMetaValue("name2")).c_str(), "bar");
}

TEST(RawDataHandler, set_get_MetaData)
{
  RawDataHandler rawDataHandler;

  MetaDataHandler m1;
  const string name {"foo"};
  m1.setSampleName(name);

  rawDataHandler.setMetaData(m1);

  const MetaDataHandler& m2 = rawDataHandler.getMetaData(); // testing const getter
  EXPECT_STREQ(m2.getSampleName().c_str(), name.c_str());
  std::shared_ptr<MetaDataHandler> m2shared = rawDataHandler.getMetaDataShared();
  EXPECT_STREQ(m2shared->getSampleName().c_str(), name.c_str());

  const string group_name {"bar"};
  rawDataHandler.getMetaData().setSampleGroupName(group_name); // testing non-const getter

  const MetaDataHandler& m3 = rawDataHandler.getMetaData();
  EXPECT_STREQ(m3.getSampleName().c_str(), name.c_str());
  EXPECT_STREQ(m3.getSampleGroupName().c_str(), group_name.c_str());
  std::shared_ptr<MetaDataHandler> m3shared = rawDataHandler.getMetaDataShared();
  EXPECT_STREQ(m3shared->getSampleName().c_str(), name.c_str());
  EXPECT_STREQ(m3shared->getSampleGroupName().c_str(), group_name.c_str());
}

TEST(RawDataHandler, set_get_Parameters)
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

  const ParameterSet& parameters2 = rawDataHandler.getParameters();
  EXPECT_EQ(parameters2.count(name1), 1);
  EXPECT_EQ(parameters2.at(name1).size(), 1);
  EXPECT_STREQ(parameters2.at(name1)[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_STREQ(parameters2.at(name1)[0].getValueAsString().c_str(), "-1");
  std::shared_ptr<ParameterSet> parameters2shared = rawDataHandler.getParametersShared();
  EXPECT_EQ(parameters2shared->count(name1), 1);
  EXPECT_EQ(parameters2shared->at(name1).size(), 1);
  EXPECT_STREQ(parameters2shared->at(name1)[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_STREQ(parameters2shared->at(name1)[0].getValueAsString().c_str(), "-1");

  ParameterSet& parameters3 = rawDataHandler.getParameters();
  EXPECT_EQ(parameters3.count(name1), 1);
  EXPECT_EQ(parameters3.at(name1).size(), 1);
  EXPECT_STREQ(parameters3.at(name1)[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_STREQ(parameters3.at(name1)[0].getValueAsString().c_str(), "-1");
  EXPECT_STREQ(parameters3.at(name1)[0].getType().c_str(), "int");
  std::shared_ptr<ParameterSet> parameters3shared = rawDataHandler.getParametersShared();
  EXPECT_EQ(parameters3shared->count(name1), 1);
  EXPECT_EQ(parameters3shared->at(name1).size(), 1);
  EXPECT_STREQ(parameters3shared->at(name1)[0].getName().c_str(), "stop_report_after_feature");
  EXPECT_STREQ(parameters3shared->at(name1)[0].getValueAsString().c_str(), "-1");
  EXPECT_STREQ(parameters3shared->at(name1)[0].getType().c_str(), "int");

}

TEST(RawDataHandler, set_get_QuantitationMethods)
{
  RawDataHandler rawDataHandler;

  OpenMS::AbsoluteQuantitationMethod aqm;
  const string name {"foo"};
  aqm.setComponentName(name);

  vector<OpenMS::AbsoluteQuantitationMethod> AQMs1;
  AQMs1.push_back(aqm);

  rawDataHandler.setQuantitationMethods(AQMs1);

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs2 = rawDataHandler.getQuantitationMethods(); // testing const getter
  EXPECT_EQ(AQMs2.size(), 1);
  EXPECT_STREQ(AQMs2[0].getComponentName().c_str(), name.c_str());
  std::shared_ptr<vector<OpenMS::AbsoluteQuantitationMethod>>& AQMs2shared = rawDataHandler.getQuantitationMethodsShared(); // testing shared_ptr getter
  EXPECT_EQ(AQMs2shared->size(), 1);
  EXPECT_STREQ(AQMs2shared->at(0).getComponentName().c_str(), name.c_str());

  const string feature_name {"bar"};
  rawDataHandler.getQuantitationMethods()[0].setFeatureName(feature_name); // testing non-const getter

  const vector<OpenMS::AbsoluteQuantitationMethod>& AQMs3 = rawDataHandler.getQuantitationMethods();
  EXPECT_STREQ(AQMs3[0].getComponentName().c_str(), name.c_str());
  EXPECT_STREQ(AQMs3[0].getFeatureName().c_str(), feature_name.c_str());
  std::shared_ptr<vector<OpenMS::AbsoluteQuantitationMethod>>& AQMs3shared = rawDataHandler.getQuantitationMethodsShared(); // testing shared_ptr getter
  EXPECT_STREQ(AQMs3shared->at(0).getComponentName().c_str(), name.c_str());
  EXPECT_STREQ(AQMs3shared->at(0).getFeatureName().c_str(), feature_name.c_str());
}

TEST(RawDataHandler, set_get_FeatureFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureFilter(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureFilterShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low {4.0};
  rawDataHandler.getFeatureFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureFilter();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureFilterShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name {"foo"};
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureQC(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureQCShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_EQ(fqc2shared->component_qcs[0].component_name.c_str(), name);

  const double rt_low {4.0};
  rawDataHandler.getFeatureQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureQC();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureQCShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureRSDFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureRSDFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureRSDFilter(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureRSDFilterShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureRSDFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureRSDFilter();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureRSDFilterShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureRSDQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureRSDQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureRSDQC(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureRSDQCShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureRSDQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureRSDQC();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureRSDQCShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureBackgroundFilter)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureBackgroundFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureBackgroundFilter(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureBackgroundFilterShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureBackgroundFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureBackgroundFilter();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureBackgroundFilterShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureBackgroundQC)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureBackgroundQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureBackgroundQC(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureBackgroundQCShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureBackgroundQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureBackgroundQC();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureBackgroundQCShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureRSDEstimations)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureRSDEstimations(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureRSDEstimations(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureRSDEstimationsShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureRSDEstimations().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureRSDEstimations();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureRSDEstimationsShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_FeatureBackgroundEstimations)
{
  RawDataHandler rawDataHandler;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  rawDataHandler.setFeatureBackgroundEstimations(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = rawDataHandler.getFeatureBackgroundEstimations(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = rawDataHandler.getFeatureBackgroundEstimationsShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  rawDataHandler.getFeatureBackgroundEstimations().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = rawDataHandler.getFeatureBackgroundEstimations();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = rawDataHandler.getFeatureBackgroundEstimationsShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(RawDataHandler, set_get_Library)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");

  rawDataHandler.setSpectraLibrary(experiment);

  const OpenMS::MSExperiment& experiment2 = rawDataHandler.getSpectraLibrary(); // testing const getter
  EXPECT_TRUE(experiment2.metaValueExists("name"));
  EXPECT_STREQ(((std::string)experiment2.getMetaValue("name")).c_str(), "foo");
  std::shared_ptr<OpenMS::MSExperiment>& experiment2shared = rawDataHandler.getSpectraLibraryShared(); // testing const getter
  EXPECT_TRUE(experiment2shared->metaValueExists("name"));
  EXPECT_STREQ(((std::string)experiment2shared->getMetaValue("name")).c_str(), "foo");

  rawDataHandler.getSpectraLibrary().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& experiment3 = rawDataHandler.getSpectraLibrary();
  EXPECT_TRUE(experiment3.metaValueExists("name"));
  EXPECT_STREQ(((std::string)experiment3.getMetaValue("name")).c_str(), "foo");
  EXPECT_TRUE(experiment3.metaValueExists("name2"));
  EXPECT_STREQ(((std::string)experiment3.getMetaValue("name2")).c_str(), "bar");
  std::shared_ptr<OpenMS::MSExperiment>& experiment3shared = rawDataHandler.getSpectraLibraryShared();
  EXPECT_TRUE(experiment3shared->metaValueExists("name"));
  EXPECT_STREQ(((std::string)experiment3shared->getMetaValue("name")).c_str(), "foo");
  EXPECT_TRUE(experiment3shared->metaValueExists("name2"));
  EXPECT_STREQ(((std::string)experiment3shared->getMetaValue("name2")).c_str(), "bar");
}

TEST(RawDataHandler, set_get_Experiment)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment experiment;
  experiment.setMetaValue("name", "foo");

  rawDataHandler.setExperiment(experiment);

  const OpenMS::MSExperiment& experiment2 = rawDataHandler.getExperiment(); // testing const getter
  EXPECT_TRUE(experiment2.metaValueExists("name"));
  EXPECT_STREQ(((std::string)experiment2.getMetaValue("name")).c_str(), "foo");

  rawDataHandler.getExperiment().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& experiment3 = rawDataHandler.getExperiment();
  EXPECT_TRUE(experiment3.metaValueExists("name"));
  EXPECT_STREQ(((std::string)experiment3.getMetaValue("name")).c_str(), "foo");
  EXPECT_TRUE(experiment3.metaValueExists("name2"));
  EXPECT_STREQ(((std::string)experiment3.getMetaValue("name2")).c_str(), "bar");
}

TEST(RawDataHandler, set_get_ChromatogramMap)
{
  RawDataHandler rawDataHandler;

  OpenMS::MSExperiment chromatogram_map;
  chromatogram_map.setMetaValue("name", "foo");

  rawDataHandler.setChromatogramMap(chromatogram_map);

  const OpenMS::MSExperiment& chromatogram_map2 = rawDataHandler.getChromatogramMap(); // testing const getter
  EXPECT_TRUE(chromatogram_map2.metaValueExists("name"));
  EXPECT_STREQ(((std::string)chromatogram_map2.getMetaValue("name")).c_str(), "foo");

  rawDataHandler.getChromatogramMap().setMetaValue("name2", "bar"); // testing non-const getter

  const OpenMS::MSExperiment& chromatogram_map3 = rawDataHandler.getChromatogramMap();
  EXPECT_TRUE(chromatogram_map3.metaValueExists("name"));
  EXPECT_STREQ(((std::string)chromatogram_map3.getMetaValue("name")).c_str(), "foo");
  EXPECT_TRUE(chromatogram_map3.metaValueExists("name2"));
  EXPECT_STREQ(((std::string)chromatogram_map3.getMetaValue("name2")).c_str(), "bar");
}

TEST(RawDataHandler, set_get_ReferenceData)
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

  EXPECT_EQ(data2.size(), 1);
  EXPECT_EQ(data2[0].count("foo"), 1);
  EXPECT_STREQ(data2[0].at("foo").s_.c_str(), "bar");
  EXPECT_EQ(data2[0].count("foo2"), 1);
  EXPECT_STREQ(data2[0].at("foo2").s_.c_str(), "bar2");

  c = "bar3";

  rawDataHandler.getReferenceData().push_back({{"foo3", c}});

  const std::vector<std::map<std::string, CastValue>>& data3 = rawDataHandler.getReferenceData();

  EXPECT_EQ(data3.size(), 2);
  EXPECT_EQ(data3[0].count("foo"), 1);
  EXPECT_STREQ(data3[0].at("foo").s_.c_str(), "bar");
  EXPECT_EQ(data3[0].count("foo2"), 1);
  EXPECT_STREQ(data3[0].at("foo2").s_.c_str(), "bar2");
  EXPECT_EQ(data3[1].count("foo3"), 1);
  EXPECT_STREQ(data3[1].at("foo3").s_.c_str(), "bar3");
}

TEST(RawDataHandler, set_get_MzTab)
{
  RawDataHandler rawDataHandler;

  OpenMS::MzTab mz_tab;
  OpenMS::MzTabMetaData md;
  md.mz_tab_type.fromCellString("Quantification");
  mz_tab.setMetaData(md);

  rawDataHandler.setMzTab(mz_tab);

  const OpenMS::MzTab& mztab1 = rawDataHandler.getMzTab(); // testing const getter
  EXPECT_STREQ(mztab1.getMetaData().mz_tab_type.get().c_str(), "Quantification");

  md.mz_tab_type.fromCellString("Qualification");
  rawDataHandler.getMzTab().setMetaData(md); // testing non-const getter

  const OpenMS::MzTab& mztab2 = rawDataHandler.getMzTab();
  EXPECT_STREQ(mztab2.getMetaData().mz_tab_type.get().c_str(), "Qualification");
}

TEST(RawDataHandler, clear)
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

  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getMetaData().getSampleName()).c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getQuantitationMethods().front().getComponentName()).c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getExperiment().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getChromatogramMap().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDEstimations().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getMzTab().getMetaData().mz_tab_type.get()).c_str(), "foo");

  rawDataHandler.clear();

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 0);
  EXPECT_STREQ(rawDataHandler.getMetaData().getSampleName().c_str(), "");
  EXPECT_EQ(rawDataHandler.getQuantitationMethods().size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureFilter().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureQC().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 0);
  EXPECT_EQ(rawDataHandler.getExperiment().size(), 0);
  EXPECT_EQ(rawDataHandler.getChromatogramMap().size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureRSDFilter().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureRSDQC().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureBackgroundFilter().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureBackgroundQC().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureRSDEstimations().component_qcs.size(), 0);
  EXPECT_EQ(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.size(), 0);
  EXPECT_STREQ(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get().c_str(), "");
}

TEST(RawDataHandler, clearNonSharedData)
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

  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getMetaData().getSampleName().c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getQuantitationMethods().front().getComponentName().c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getExperiment().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(((std::string)rawDataHandler.getChromatogramMap().getMetaValue("name")).c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDEstimations().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get().c_str(), "foo");

  rawDataHandler.clearNonSharedData();

  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 0);
  EXPECT_STREQ(rawDataHandler.getMetaData().getSampleName().c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getQuantitationMethods().front().getComponentName().c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 0);
  EXPECT_EQ(rawDataHandler.getExperiment().size(), 0);
  EXPECT_EQ(rawDataHandler.getChromatogramMap().size(), 0);
  EXPECT_STREQ(rawDataHandler.getFeatureRSDFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundFilter().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundQC().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureRSDEstimations().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getFeatureBackgroundEstimations().component_qcs.front().component_name.c_str(), "foo");
  EXPECT_STREQ(rawDataHandler.getMzTab().getMetaData().mz_tab_type.get().c_str(), "");
}

TEST(RawDataHandler, updateFeatureMapHistory)
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
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool());

  // Test empty feature_map_history with a feature_map with "used_" attribute annotated
  s1a.setMetaValue("used_", "false");
  s1a.setMetaValue("timestamp_", "now");
  fm1[0].setSubordinates({ s1a });
  rawDataHandler.clear();
  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_FALSE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("timestamp_")).c_str(), "now");

  // Test new subordinate
  s1a.clearMetaInfo();
  s1a.setMetaValue("native_id", "component_1a");
  s1b.setMetaValue("native_id", "component_1b");
  fm1[0].setSubordinates({s1a, s1b});

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool());

  // Test filtered subordinates
  fm1[0].setSubordinates({ s1a });

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_FALSE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool());

  // Test new feature
  f2.setMetaValue("PeptideRef", "component_group_2");
  f2.setUniqueId("f_2"); // ASSUMPTION: unique id attribute will be set within OpenMS
  s2a.setMetaValue("native_id", "component_2a");
  s2b.setMetaValue("native_id", "component_2b");
  f2.setSubordinates({ s2a, s2b });
  fm1.push_back(f2);

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 2);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_FALSE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[1].getUniqueId(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[1].getMetaValue("PeptideRef")).c_str(), "component_group_2");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[1].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[1].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_2a");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_2b");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("used_").toBool());

  // Test removed feature
  fm1.clear();
  fm1.push_back(f2);

  rawDataHandler.setFeatureMap(fm1);
  rawDataHandler.updateFeatureMapHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory().size(), 2);
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_FALSE(rawDataHandler.getFeatureMapHistory()[0].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_FALSE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_FALSE(rawDataHandler.getFeatureMapHistory()[0].getSubordinates()[1].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[1].getUniqueId(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[1].getMetaValue("PeptideRef")).c_str(), "component_group_2");
  EXPECT_EQ(rawDataHandler.getFeatureMapHistory()[1].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_2a");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_2b");
  EXPECT_TRUE(rawDataHandler.getFeatureMapHistory()[1].getSubordinates()[1].getMetaValue("used_").toBool());
}

TEST(RawDataHandler, makeFeatureMapFromHistory)
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
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool());

  // Test empty feature_map_history with a feature_map with "used_" attribute annotated
  s1a.setMetaValue("used_", "false");
  s1a.setMetaValue("timestamp_", "now");
  fm1[0].setSubordinates({ s1a });
  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 0);

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
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("used_").toBool());

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
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 2);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("used_").toBool());
  EXPECT_EQ(rawDataHandler.getFeatureMap()[1].getUniqueId(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[1].getMetaValue("PeptideRef")).c_str(), "component_group_2");
  EXPECT_EQ(rawDataHandler.getFeatureMap()[1].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[1].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_2a");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[1].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[1].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_2b");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[1].getSubordinates()[1].getMetaValue("used_").toBool());

  // Test multiple feature with filtered subordinates
  s2a.setMetaValue("native_id", "component_2a");
  s2a.setMetaValue("used_", "false");
  s2b.setMetaValue("native_id", "component_2b");
  s2b.setMetaValue("used_", "false");
  fm1[1].setSubordinates({ s2a, s2b });

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 2);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("native_id")).c_str(), "component_1a");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[0].getMetaValue("used_").toBool());
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("native_id")).c_str(), "component_1b");
  EXPECT_TRUE(rawDataHandler.getFeatureMap()[0].getSubordinates()[1].getMetaValue("used_").toBool());

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
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getUniqueId(), 1);
  EXPECT_STREQ(((std::string)rawDataHandler.getFeatureMap()[0].getMetaValue("PeptideRef")).c_str(), "component_group_1");
  EXPECT_EQ(rawDataHandler.getFeatureMap()[0].getSubordinates().size(), 0);

  // Test feature level
  fm1.clear();
  f1.setMetaValue("PeptideRef", "component_group_1");
  f1.setMetaValue("used_", "false");
  f1.setUniqueId("f_1"); // ASSUMPTION: unique id attribute will be set within OpenMS
  fm1.push_back(f1);

  rawDataHandler.clear();
  rawDataHandler.setFeatureMapHistory(fm1);
  rawDataHandler.makeFeatureMapFromHistory();
  EXPECT_EQ(rawDataHandler.getFeatureMap().size(), 0);
}
