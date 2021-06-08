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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <string>
#include <filesystem>

namespace SmartPeak
{
  class Filenames
  {
public:
    std::string sequence_csv_i;
    std::string parameters_csv_i;
    std::string workflow_csv_i;
    std::string traML_csv_i;
    std::string featureFilterComponents_csv_i;
    std::string featureFilterComponentGroups_csv_i;
    std::string featureQCComponents_csv_i;
    std::string featureQCComponentGroups_csv_i;
    std::string featureRSDFilterComponents_csv_i;
    std::string featureRSDFilterComponentGroups_csv_i;
    std::string featureRSDQCComponents_csv_i;
    std::string featureRSDQCComponentGroups_csv_i;
    std::string featureBackgroundFilterComponents_csv_i;
    std::string featureBackgroundFilterComponentGroups_csv_i;
    std::string featureBackgroundQCComponents_csv_i;
    std::string featureBackgroundQCComponentGroups_csv_i;
    std::string featureRSDEstimationComponents_csv_i;
    std::string featureRSDEstimationComponentGroups_csv_i;
    std::string featureBackgroundEstimationComponents_csv_i;
    std::string featureBackgroundEstimationComponentGroups_csv_i;
    std::string quantitationMethods_csv_i;
    std::string standardsConcentrations_csv_i;
    std::string referenceData_csv_i;
    std::string mzML_i;
    std::string mzTab_i;
    std::string mzTab_o;
    std::string featureXML_o;
    std::string features_pdf_o;
    std::string featureXMLSampleGroup_o;
    std::string featureXML_i;
    std::string featureXMLSampleGroup_i;
    std::string featureFilterComponents_csv_o;
    std::string featureFilterComponentGroups_csv_o;
    std::string featureQCComponents_csv_o;
    std::string featureQCComponentGroups_csv_o;
    std::string featureRSDFilterComponents_csv_o;
    std::string featureRSDFilterComponentGroups_csv_o;
    std::string featureRSDQCComponents_csv_o;
    std::string featureRSDQCComponentGroups_csv_o;
    std::string featureBackgroundFilterComponents_csv_o;
    std::string featureBackgroundFilterComponentGroups_csv_o;
    std::string featureBackgroundQCComponents_csv_o;
    std::string featureBackgroundQCComponentGroups_csv_o;
    std::string featureRSDEstimationComponents_csv_o;
    std::string featureRSDEstimationComponentGroups_csv_o;
    std::string featureBackgroundEstimationComponents_csv_o;
    std::string featureBackgroundEstimationComponentGroups_csv_o;
    std::string quantitationMethods_csv_o;
    std::string componentsToConcentrations_csv_o;
    std::string pivotTable_csv_o;
    std::string featureDB_csv_o;
    // TODO: do not hardcode entire pathnames (all those above this line)
    // Instead, construct them when needed, using the strings below
    std::string mzml_input_path;
    std::string features_input_path;
    std::string output_path;

    static Filenames getDefaultStaticFilenames(
      const std::string& dir
    );

    static Filenames getDefaultDynamicFilenames(
      const std::string& mzml_input_path,
      const std::string& features_input_path,
      const std::string& output_path,
      const std::string& input_mzML_filename,
      const std::string& input_inj_name,
      const std::string& output_inj_name,
      const std::string& input_sample_name,
      const std::string& output_sample_name
    );

    static void updateDefaultDynamicFilenames(
      const std::string& mzml_input_path,
      const std::string& features_input_path,
      const std::string& output_path,
      Filenames& filenames
    );

    void clear();
  };
}
