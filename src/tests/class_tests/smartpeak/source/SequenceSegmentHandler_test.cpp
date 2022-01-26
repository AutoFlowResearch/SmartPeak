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
#include <SmartPeak/core/SequenceSegmentHandler.h>

using namespace SmartPeak;
using namespace std;

TEST(SequenceSegmentHandler, constructor)
{
  SequenceSegmentHandler* ptr = nullptr;
  SequenceSegmentHandler* nullPointer = nullptr;
  ptr = new SequenceSegmentHandler();
  EXPECT_NE(ptr, nullPointer);
  delete ptr;
}

TEST(SequenceSegmentHandler, set_get_SequenceSegmentName)
{
  SequenceSegmentHandler ssh;
  const string foo {"foo"};

  ssh.setSequenceSegmentName(foo);

  const string& name1 = ssh.getSequenceSegmentName(); // testing const getter
  EXPECT_EQ(name1, foo);

  const string bar {"bar"};
  ssh.getSequenceSegmentName() = bar;
  const string& name2 = ssh.getSequenceSegmentName(); // testing non-const getter
  EXPECT_EQ(name2, bar);
}

TEST(SequenceSegmentHandler, set_get_SampleIndices)
{
  SequenceSegmentHandler ssh;
  vector<size_t> si1 {1, 3, 5};

  ssh.setSampleIndices(si1);

  const vector<size_t>& si2 = ssh.getSampleIndices(); // testing const getter
  EXPECT_EQ(si2.size(), 3);
  EXPECT_EQ(si2[0], 1);
  EXPECT_EQ(si2[1], 3);
  EXPECT_EQ(si2[2], 5);

  ssh.getSampleIndices()[1] = 2;

  const vector<size_t>& si3 = ssh.getSampleIndices(); // testing non-const getter
  EXPECT_EQ(si3.size(), 3);
  EXPECT_EQ(si3[1], 2);
}

TEST(SequenceSegmentHandler, set_get_StandardsConcentrations)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationStandards::runConcentration rc;
  const string foo {"foo"};
  rc.sample_name = foo;
  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs1;
  runs1.push_back(rc);

  ssh.setStandardsConcentrations(runs1);

  const vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& runs2 = ssh.getStandardsConcentrations();
  EXPECT_EQ(runs2.size(), 1);
  EXPECT_STREQ(runs2[0].sample_name.c_str(), foo.c_str());

  const string bar {"bar"};
  ssh.getStandardsConcentrations()[0].component_name = bar;
  const vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& runs3 = ssh.getStandardsConcentrations();
  EXPECT_EQ(runs3.size(), 1);
  EXPECT_STREQ(runs3[0].sample_name.c_str(), foo.c_str());
  EXPECT_STREQ(runs3[0].component_name.c_str(), bar.c_str());
}

TEST(SequenceSegmentHandler, set_get_QuantitationMethods)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationMethod qm;
  const string foo {"foo"};
  qm.setComponentName(foo);
  vector<OpenMS::AbsoluteQuantitationMethod> qms1;
  qms1.push_back(qm);

  ssh.setQuantitationMethods(qms1);

  const vector<OpenMS::AbsoluteQuantitationMethod>& qms2 = ssh.getQuantitationMethods();
  EXPECT_EQ(qms2.size(), 1);
  EXPECT_STREQ(qms2[0].getComponentName().c_str(), foo.c_str());
  std::shared_ptr<vector<OpenMS::AbsoluteQuantitationMethod>>& qms2shared = ssh.getQuantitationMethodsShared();
  EXPECT_EQ(qms2shared->size(), 1);
  EXPECT_STREQ(qms2shared->at(0).getComponentName().c_str(), foo.c_str());

  const string bar {"bar"};
  ssh.getQuantitationMethods()[0].setFeatureName(bar);
  const vector<OpenMS::AbsoluteQuantitationMethod>& qms3 = ssh.getQuantitationMethods();
  EXPECT_EQ(qms3.size(), 1);
  EXPECT_STREQ(qms3[0].getComponentName().c_str(), foo.c_str());
  EXPECT_STREQ(qms3[0].getFeatureName().c_str(), bar.c_str());
  std::shared_ptr<vector<OpenMS::AbsoluteQuantitationMethod>>& qms3shared = ssh.getQuantitationMethodsShared();
  EXPECT_EQ(qms3shared->size(), 1);
  EXPECT_STREQ(qms3shared->at(0).getComponentName().c_str(), foo.c_str());
  EXPECT_STREQ(qms3shared->at(0).getFeatureName().c_str(), bar.c_str());
}

TEST(SequenceSegmentHandler, set_get_FeatureFilter)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureFilter(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureFilterShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureFilter();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureFilterShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureQC)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureQC(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureQCShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureQC();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureQCShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureRSDFilter)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureRSDFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureRSDFilter(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureRSDFilterShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureRSDFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureRSDFilter();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureRSDFilterShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureRSDQC)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureRSDQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureRSDQC(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureRSDQCShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureRSDQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureRSDQC();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureRSDQCShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureBackgroundFilter)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureBackgroundFilter(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureBackgroundFilter(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureBackgroundFilterShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureBackgroundFilter().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureBackgroundFilter();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureBackgroundFilterShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureBackgroundQC)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureBackgroundQC(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureBackgroundQC(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureBackgroundQCShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureBackgroundQC().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureBackgroundQC();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureBackgroundQCShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureRSDEstimations)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureRSDEstimations(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureRSDEstimations(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureRSDEstimationsShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureRSDEstimations().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureRSDEstimations();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureRSDEstimationsShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_FeatureBackgroundEstimations)
{
  SequenceSegmentHandler ssh;

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  const string name{ "foo" };
  qc.component_name = name;

  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);

  ssh.setFeatureBackgroundEstimations(fqc1);

  const OpenMS::MRMFeatureQC& fqc2 = ssh.getFeatureBackgroundEstimations(); // testing const getter
  EXPECT_EQ(fqc2.component_qcs.size(), 1);
  EXPECT_STREQ(fqc2.component_qcs[0].component_name.c_str(), name.c_str());
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc2shared = ssh.getFeatureBackgroundEstimationsShared(); // testing shared_ptr getter
  EXPECT_EQ(fqc2shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc2shared->component_qcs[0].component_name.c_str(), name.c_str());

  const double rt_low{ 4.0 };
  ssh.getFeatureBackgroundEstimations().component_qcs[0].retention_time_l = rt_low; // testing non-const getter

  const OpenMS::MRMFeatureQC& fqc3 = ssh.getFeatureBackgroundEstimations();
  EXPECT_EQ(fqc3.component_qcs.size(), 1);
  EXPECT_STREQ(fqc3.component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3.component_qcs[0].retention_time_l, rt_low);
  std::shared_ptr<OpenMS::MRMFeatureQC>& fqc3shared = ssh.getFeatureBackgroundEstimationsShared();
  EXPECT_EQ(fqc3shared->component_qcs.size(), 1);
  EXPECT_STREQ(fqc3shared->component_qcs[0].component_name.c_str(), name.c_str());
  EXPECT_EQ(fqc3shared->component_qcs[0].retention_time_l, rt_low);
}

TEST(SequenceSegmentHandler, set_get_ComponentsToConcentrations)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationStandards::featureConcentration fc;
  const string foo {"foo"};
  const double ac1 {4.5};
  fc.actual_concentration = ac1;
  vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> fc1;
  fc1.push_back(fc);
  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> m1;
  m1.insert({foo, fc1});

  ssh.setComponentsToConcentrations(m1);

  const map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    m2 = ssh.getComponentsToConcentrations();
  EXPECT_EQ(m2.size(), 1);
  EXPECT_EQ(m2.count(foo), 1);
  EXPECT_EQ(m2.at(foo)[0].actual_concentration, ac1);

  const double ac2 {7.3};
  fc1[0].IS_actual_concentration = ac2;
  ssh.getComponentsToConcentrations().erase(foo);
  ssh.getComponentsToConcentrations().insert({foo, fc1});

  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    m3 = ssh.getComponentsToConcentrations();
  EXPECT_EQ(m3.size(), 1);
  EXPECT_EQ(m3.count(foo), 1);
  EXPECT_EQ(m3.at(foo)[0].actual_concentration, ac1);
  EXPECT_EQ(m3.at(foo)[0].IS_actual_concentration, ac2);
}

TEST(SequenceSegmentHandler, set_get_OutlierComponentsToConcentrations)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationStandards::featureConcentration fc;
  const string foo{ "foo" };
  const double ac1{ 4.5 };
  fc.actual_concentration = ac1;
  vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> fc1;
  fc1.push_back(fc);
  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> m1;
  m1.insert({ foo, fc1 });

  ssh.setOuterComponentsToConcentrations(m1);

  const map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    m2 = ssh.getOutlierComponentsToConcentrations();
  EXPECT_EQ(m2.size(), 1);
  EXPECT_EQ(m2.count(foo), 1);
  EXPECT_EQ(m2.at(foo)[0].actual_concentration, ac1);

  const double ac2{ 7.3 };
  fc1[0].IS_actual_concentration = ac2;
  ssh.getOutlierComponentsToConcentrations().erase(foo);
  ssh.getOutlierComponentsToConcentrations().insert({ foo, fc1 });

  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    m3 = ssh.getOutlierComponentsToConcentrations();
  EXPECT_EQ(m3.size(), 1);
  EXPECT_EQ(m3.count(foo), 1);
  EXPECT_EQ(m3.at(foo)[0].actual_concentration, ac1);
  EXPECT_EQ(m3.at(foo)[0].IS_actual_concentration, ac2);
}

TEST(SequenceSegmentHandler, clear)
{
  SequenceSegmentHandler ssh;

  ssh.setSequenceSegmentName("foo");

  vector<size_t> si1 {1, 3, 5};
  ssh.setSampleIndices(si1);

  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs1(1);
  ssh.setStandardsConcentrations(runs1);

  vector<OpenMS::AbsoluteQuantitationMethod> qms1(1);
  ssh.setQuantitationMethods(qms1);

  OpenMS::MRMFeatureQC::ComponentQCs qc;
  qc.component_name = "foo";
  OpenMS::MRMFeatureQC fqc1;
  fqc1.component_qcs.push_back(qc);
  ssh.setFeatureFilter(fqc1);
  ssh.setFeatureQC(fqc1);
  ssh.setFeatureRSDFilter(fqc1);
  ssh.setFeatureRSDQC(fqc1);
  ssh.setFeatureBackgroundFilter(fqc1);
  ssh.setFeatureBackgroundQC(fqc1);
  ssh.setFeatureRSDEstimations(fqc1);
  ssh.setFeatureBackgroundEstimations(fqc1);

  vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> fc1(1);
  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> m1;
  m1.insert({"foo", fc1});
  ssh.setComponentsToConcentrations(m1);
  ssh.setOutlierComponentsToConcentrations(m1);

  EXPECT_FALSE(ssh.getSequenceSegmentName().empty());
  EXPECT_FALSE(ssh.getSampleIndices().empty());
  EXPECT_FALSE(ssh.getStandardsConcentrations().empty());
  EXPECT_FALSE(ssh.getQuantitationMethods().empty());
  EXPECT_FALSE(ssh.getComponentsToConcentrations().empty());
  EXPECT_FALSE(ssh.getOutlierComponentsToConcentrations().empty());
  EXPECT_FALSE(ssh.getFeatureFilter().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureQC().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureRSDFilter().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureRSDQC().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureBackgroundFilter().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureBackgroundQC().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureRSDEstimations().component_qcs.empty());
  EXPECT_FALSE(ssh.getFeatureBackgroundEstimations().component_qcs.empty());

  ssh.clear();

  EXPECT_TRUE(ssh.getSequenceSegmentName().empty());
  EXPECT_TRUE(ssh.getSampleIndices().empty());
  EXPECT_TRUE(ssh.getStandardsConcentrations().empty());
  EXPECT_TRUE(ssh.getQuantitationMethods().empty());
  EXPECT_TRUE(ssh.getComponentsToConcentrations().empty());
  EXPECT_TRUE(ssh.getOutlierComponentsToConcentrations().empty());
  EXPECT_TRUE(ssh.getFeatureFilter().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureQC().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureRSDFilter().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureRSDQC().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureBackgroundFilter().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureBackgroundQC().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureRSDEstimations().component_qcs.empty());
  EXPECT_TRUE(ssh.getFeatureBackgroundEstimations().component_qcs.empty());
}
