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
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

TEST(Filenames, filenames_getDefaultStaticFilenames)
{
  Filenames filenames = Filenames::getDefaultStaticFilenames("/home/user");
  EXPECT_STREQ(filenames.sequence_csv_i.c_str(), "/home/user/sequence.csv");
  EXPECT_STREQ(filenames.parameters_csv_i.c_str(), "/home/user/parameters.csv");
  EXPECT_STREQ(filenames.traML_csv_i.c_str(), "/home/user/traML.csv");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_i.c_str(), "/home/user/featureFilterComponents.csv");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_i.c_str(), "/home/user/featureFilterComponentGroups.csv");
  EXPECT_STREQ(filenames.featureQCComponents_csv_i.c_str(), "/home/user/featureQCComponents.csv");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_i.c_str(), "/home/user/featureQCComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDFilterComponents_csv_i.c_str(), "/home/user/featureRSDFilterComponents.csv");
  EXPECT_STREQ(filenames.featureRSDFilterComponentGroups_csv_i.c_str(), "/home/user/featureRSDFilterComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_i.c_str(), "/home/user/featureRSDQCComponents.csv");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_i.c_str(), "/home/user/featureRSDQCComponentGroups.csv");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_i.c_str(), "/home/user/featureBackgroundFilterComponents.csv");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_i.c_str(), "/home/user/featureBackgroundFilterComponentGroups.csv");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_i.c_str(), "/home/user/featureBackgroundQCComponents.csv");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_i.c_str(), "/home/user/featureBackgroundQCComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_i.c_str(), "/home/user/featureRSDEstimationComponents.csv");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_i.c_str(), "/home/user/featureRSDEstimationComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_i.c_str(), "/home/user/featureRSDQCComponents.csv");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_i.c_str(), "/home/user/featureRSDQCComponentGroups.csv");
  EXPECT_STREQ(filenames.quantitationMethods_csv_i.c_str(), "/home/user/quantitationMethods.csv");
  EXPECT_STREQ(filenames.standardsConcentrations_csv_i.c_str(), "/home/user/standardsConcentrations.csv");
  EXPECT_STREQ(filenames.referenceData_csv_i.c_str(), "/home/user/referenceData.csv");
  EXPECT_STREQ(filenames.mzML_i.c_str(), "");
  EXPECT_STREQ(filenames.mzTab_o.c_str(), "");
  EXPECT_STREQ(filenames.mzTab_i.c_str(), "");
  EXPECT_STREQ(filenames.featureXML_o.c_str(), "");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_o.c_str(), "");
  EXPECT_STREQ(filenames.featureXML_i.c_str(), "");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.quantitationMethods_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.componentsToConcentrations_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.pivotTable_csv_o.c_str(), "/home/user/PivotTable.csv");
  EXPECT_STREQ(filenames.featureDB_csv_o.c_str(), "/home/user/FeatureDB.csv");
}

TEST(Filenames, filenames_getDefaultDynamicFilenames)
{
  Filenames filenames = Filenames::getDefaultDynamicFilenames(
    "/home/user",
    "/home/user/mzML",
    "/home/user/featuresIn", 
    "/home/user/featuresOut", 
    "mzMLIn",
    "injIn",
    "injOut",
    "sampleIn",
    "sampleOut");
  EXPECT_STREQ(filenames.sequence_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.parameters_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.workflow_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.traML_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.quantitationMethods_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.standardsConcentrations_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.referenceData_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.mzML_i.c_str(), "/home/user/mzML/mzMLIn.mzML");
  EXPECT_STREQ(filenames.mzTab_o.c_str(), "/home/user/featuresOut/injOut.mzTab");
  EXPECT_STREQ(filenames.mzTab_i.c_str(), "/home/user/featuresIn/injIn.mzTab");
  EXPECT_STREQ(filenames.featureXML_o.c_str(), "/home/user/featuresOut/injOut.featureXML");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_o.c_str(), "/home/user/featuresOut/sampleOut.featureXML");
  EXPECT_STREQ(filenames.featureXML_i.c_str(), "/home/user/featuresIn/injIn.featureXML");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_i.c_str(), "/home/user/featuresIn/sampleIn.featureXML");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureFilterComponents.csv");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureFilterComponentGroups.csv");
  EXPECT_STREQ(filenames.featureQCComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureQCComponents.csv");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureQCComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDFilterComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureRSDFilterComponents.csv");
  EXPECT_STREQ(filenames.featureRSDFilterComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureRSDFilterComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureRSDQCComponents.csv");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureRSDQCComponentGroups.csv");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureBackgroundFilterComponents.csv");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureBackgroundFilterComponentGroups.csv");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureBackgroundQCComponents.csv");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureBackgroundQCComponentGroups.csv");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureRSDEstimationComponents.csv");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureRSDEstimationComponentGroups.csv");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_o.c_str(), "/home/user/featuresOut/injOut_featureBackgroundEstimationComponents.csv");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_o.c_str(), "/home/user/featuresOut/injOut_featureBackgroundEstimationComponentGroups.csv");
  EXPECT_STREQ(filenames.quantitationMethods_csv_o.c_str(), "/home/user/featuresOut/injOut_quantitationMethods.csv");
  EXPECT_STREQ(filenames.componentsToConcentrations_csv_o.c_str(), "/home/user/featuresOut/injOut_componentsToConcentrations.csv");
  EXPECT_STREQ(filenames.pivotTable_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureDB_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.selectiveDilutions_csv_i.c_str(), "/home/user/selectiveDilutions.csv");
}

TEST(Filenames, clear1)
{
  Filenames filenames = Filenames::getDefaultStaticFilenames("/home/user");
  filenames.clear();
  EXPECT_STREQ(filenames.sequence_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.parameters_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.traML_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.quantitationMethods_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.standardsConcentrations_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.referenceData_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.selectiveDilutions_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.mzML_i.c_str(), "");
  EXPECT_STREQ(filenames.mzTab_o.c_str(), "");
  EXPECT_STREQ(filenames.mzTab_i.c_str(), "");
  EXPECT_STREQ(filenames.featureXML_o.c_str(), "");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_o.c_str(), "");
  EXPECT_STREQ(filenames.featureXML_i.c_str(), "");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.quantitationMethods_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.componentsToConcentrations_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.pivotTable_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureDB_csv_o.c_str(), "");
}

TEST(Filenames, clear2)
{
  Filenames filenames = Filenames::getDefaultDynamicFilenames(
    "/home/user",
    "/home/user/mzML",
    "/home/user/featuresIn",
    "/home/user/featuresOut",
    "mzMLIn",
    "injIn",
    "injOut",
    "sampleIn",
    "sampleOut");
  filenames.clear();
  EXPECT_STREQ(filenames.sequence_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.parameters_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.traML_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.quantitationMethods_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.standardsConcentrations_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.referenceData_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.selectiveDilutions_csv_i.c_str(), "");
  EXPECT_STREQ(filenames.mzML_i.c_str(), "");
  EXPECT_STREQ(filenames.mzTab_o.c_str(), "");
  EXPECT_STREQ(filenames.mzTab_i.c_str(), "");
  EXPECT_STREQ(filenames.featureXML_o.c_str(), "");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_o.c_str(), "");
  EXPECT_STREQ(filenames.featureXML_i.c_str(), "");
  EXPECT_STREQ(filenames.featureXMLSampleGroup_i.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundFilterComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundQCComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureRSDEstimationComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponents_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureBackgroundEstimationComponentGroups_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.quantitationMethods_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.componentsToConcentrations_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.pivotTable_csv_o.c_str(), "");
  EXPECT_STREQ(filenames.featureDB_csv_o.c_str(), "");
}
