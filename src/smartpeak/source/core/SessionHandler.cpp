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

#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/io/SequenceParser.h>

#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>

#include <plog/Log.h>

namespace SmartPeak
{
  static int max_nb_points = 10000000;
  
  void SessionHandler::onSequenceUpdated()
  {
    sequence_table.clear();
  }

  void SessionHandler::onTransitionsUpdated()
  {
    transitions_table.clear();
  }

  void SessionHandler::onFeaturesUpdated() {}

  std::string findSampleNameFromStandardConcentration(
    const SmartPeak::SequenceSegmentHandler& sequence_segment,
    const std::string& component_name,
    const OpenMS::AbsoluteQuantitationStandards::featureConcentration& point
    )
  {
    std::string sample_name;
    for (const auto& stand_concs : sequence_segment.getStandardsConcentrations())
    {
      if ((stand_concs.component_name == component_name)
        && (std::abs(stand_concs.actual_concentration - point.actual_concentration) < 1e-9)
        && (std::abs(stand_concs.IS_actual_concentration - point.IS_actual_concentration) < 1e-9)
        && (std::abs(stand_concs.dilution_factor - point.dilution_factor) < 1e-9))
      {
        sample_name = stand_concs.sample_name;
        break;
      }
    }
    return sample_name;
  }

  void SessionHandler::setMinimalDataAndFilters(const SequenceHandler & sequence_handler)
  {
    //LOGD << "START setMinimalDataAndFilters"; // Not helpful as it is called every few seconds when the GUI is displayed
    // Set the minimal data for the transitions
    setTransitionsTable(sequence_handler, transitions_table);
    setTransitionExplorer(transitions_table);
    // Set the minimal data for the injections
    setSequenceTable(sequence_handler, sequence_table);
    setInjectionExplorer(sequence_table);
    // Set the minimal data for the filters
    setFeatureExplorer();
    // Set the minimal data for the spectra
    setSpectrumTable(sequence_handler, spectrum_table);
    setSpectrumExplorer(spectrum_table);
    //LOGD << "END setMinimalDataAndFilters";
  }

  void SessionHandler::setInjectionExplorer(GenericTableData& table_data) {
    // Make the injection explorer headers
    if (injection_explorer_data.checkbox_headers.size() <= 0) {
      LOGD << "Making injection_explorer_checkbox_headers";
      injection_explorer_data.checkbox_headers.resize(3);
      injection_explorer_data.checkbox_headers.setValues({ "workflow", "plot", "table" });
    }
    const int n_rows = table_data.body_.dimension(0);
    // Make the injection explorer body
    if (injection_explorer_data.checkbox_body.dimension(0) != n_rows) {
      LOGD << "Making injection_explorer_checkbox_body";
      injection_explorer_data.checkbox_body.resize(n_rows, (int)injection_explorer_data.checkbox_headers.size());
      injection_explorer_data.checkbox_body.setConstant(true);
      for (int i = 0; i < injection_explorer_data.checkbox_body.dimension(0); ++i) injection_explorer_data.checkbox_body(i, 1) = false; // set all to false
      injection_explorer_data.checked_rows.resize(n_rows);
      injection_explorer_data.checked_rows.setConstant(true);
    }
  }

  void SessionHandler::setTransitionExplorer(GenericTableData& table_data) {
    // Make the transition explorer headers
    if (transition_explorer_data.checkbox_headers.size() <= 0) {
      LOGD << "Making transition_explorer_checkbox_headers";
      transition_explorer_data.checkbox_headers.resize(2);
      transition_explorer_data.checkbox_headers.setValues({ "plot", "table" });
    }
    const int n_rows = table_data.body_.dimension(0);
    // Make the transition table body
    if (transition_explorer_data.checkbox_body.dimension(0) != n_rows) {
      LOGD << "Making transition_explorer_checkbox_body";
      transition_explorer_data.checkbox_body.resize(n_rows, (int)transition_explorer_data.checkbox_headers.size());
      transition_explorer_data.checkbox_body.setConstant(true);
      for (int i = 0; i < transition_explorer_data.checkbox_body.dimension(0); ++i) transition_explorer_data.checkbox_body(i, 0) = false; // set all to false
      transition_explorer_data.checked_rows.resize(n_rows);
      transition_explorer_data.checked_rows.setConstant(true);
    }
  }

  void SessionHandler::setFeatureExplorer() {
    // Make the feature explorer headers
    if (feature_table.headers_.size() <= 0) {
      LOGD << "Making feature_table.headers_";
      feature_table.headers_.resize(1);
      feature_table.headers_.setValues({ "name" });
      feature_explorer_data.checkbox_headers.resize(2);
      feature_explorer_data.checkbox_headers.setValues({ "plot", "table" });
    }
    const int n_cols = feature_table.headers_.size();
    const int n_rows = metadataFloatToString.size();
    // Make the feature explorer body
    if (feature_table.body_.dimension(0) != n_rows) {
      LOGD << "Making feature_table.body_";
      feature_table.body_.resize(n_rows, n_cols);
      int col = 0, row = 0;
      for (const auto& metadata : metadataFloatToString) {
        feature_table.body_(row, col) = metadata.second;
        col = 0;
        ++row;
      }
      feature_explorer_data.checkbox_body.resize(n_rows, (int)feature_explorer_data.checkbox_headers.size());
      feature_explorer_data.checkbox_body.setConstant(false);
      feature_explorer_data.checked_rows.resize(n_rows);
      feature_explorer_data.checked_rows.setConstant(true);
    }
  }

  void SessionHandler::setSpectrumExplorer(GenericTableData& table_data)
  {
    // Make the spectrum explorer headers
    if (spectrum_explorer_data.checkbox_headers.size() <= 0) {
      LOGD << "Making spectrum_explorer_checkbox_headers";
      spectrum_explorer_data.checkbox_headers.resize(2);
      spectrum_explorer_data.checkbox_headers.setValues({ "plot", "table" });
    }
    const int n_rows = table_data.body_.dimension(0);
    // Make the spectrum table body
    if (spectrum_explorer_data.checkbox_body.dimension(0) != n_rows) {
      LOGD << "Making spectrum_explorer_checkbox_body";
      spectrum_explorer_data.checkbox_body.resize(n_rows, (int)spectrum_explorer_data.checkbox_headers.size());
      spectrum_explorer_data.checkbox_body.setConstant(true);
      for (int i = 1; i < spectrum_explorer_data.checkbox_body.dimension(0); ++i) spectrum_explorer_data.checkbox_body(i, 0) = false; // only the first spectrum
      spectrum_explorer_data.checked_rows.resize(n_rows);
      spectrum_explorer_data.checked_rows.setConstant(true);
    }
  }

  void SessionHandler::setSequenceTable(const SequenceHandler & sequence_handler, GenericTableData& table_data) {
    // Make the sequence table headers
    if (table_data.headers_.size() <= 0) {
      LOGD << "Making sequence_table_headers";
      table_data.headers_.resize(12);
      table_data.headers_.setValues({
      "inj#", "sample_name", "sample_group_name" , "sequence_segment_name" , "replicate_group_name", "sample_type", 
      "original_filename", "acq_method_name", "inj_volume", "inj_volume_units", "batch_name", // skipping optional members
      "acquisition_date_and_time" });
    }
    const int n_cols = table_data.headers_.size();
    const int n_rows = sequence_handler.getSequence().size();
    // Make the sequence table body
    if (table_data.body_.dimension(0) != n_rows) {
      LOGD << "Making sequence_table_body";
      table_data.body_.resize(n_rows, n_cols);
      int col = 0, row = 0;
      for (const auto& injection : sequence_handler.getSequence()) {
        table_data.body_(row, col) = std::to_string(injection.getMetaData().inj_number);
        ++col;
        table_data.body_(row, col) = injection.getMetaData().getSampleName();
        ++col;
        table_data.body_(row, col) = injection.getMetaData().getSampleGroupName();
        ++col;
        table_data.body_(row, col) = injection.getMetaData().getSequenceSegmentName();
        ++col;
        table_data.body_(row, col) = injection.getMetaData().getReplicateGroupName();
        ++col;
        table_data.body_(row, col) = injection.getMetaData().getSampleTypeAsString();
        ++col;
        table_data.body_(row, col) = injection.getMetaData().getFilename();
        ++col;
        table_data.body_(row, col) = injection.getMetaData().acq_method_name;
        ++col;
        table_data.body_(row, col) = std::to_string(injection.getMetaData().inj_volume);
        ++col;
        table_data.body_(row, col) = injection.getMetaData().inj_volume_units;
        ++col;
        table_data.body_(row, col) = injection.getMetaData().batch_name;
        ++col;
        // Skipping optional members
        table_data.body_(row, col) = injection.getMetaData().getAcquisitionDateAndTimeAsString();
        col = 0;
        ++row;
      }
    }
  }

  void SessionHandler::setTransitionsTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    // Make the transition table headers
    if (table_data.headers_.size() <= 0) {
      LOGD << "Making transitions_table_headers";
      table_data.headers_.resize(9);
      table_data.headers_.setValues({ // NOTE: only showing a subset
      "transition_group","transition_name","RetentionTime","PrecursorMz","ProductMz",
      "LabelType","quantifying_transition","identifying_transition","detecting_transition" });
    }
    const int n_cols = table_data.headers_.size();
    // Make the transition table body
    if (sequence_handler.getSequence().size() > 0) {
      const auto& targeted_exp = sequence_handler.getSequence().at(0).getRawData().getTargetedExperiment();
      const int n_rows = targeted_exp.getTransitions().size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making transitions_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& transition : targeted_exp.getTransitions()) {
          table_data.body_(row, col) = transition.getPeptideRef();
          ++col;
          table_data.body_(row, col) = transition.getNativeID();
          ++col;
          table_data.body_(row, col) = std::to_string(targeted_exp.getPeptideByRef(transition.getPeptideRef()).getRetentionTime());
          ++col;
          table_data.body_(row, col) = std::to_string(transition.getPrecursorMZ());
          ++col;
          table_data.body_(row, col) = std::to_string(transition.getProductMZ());
          ++col;
          //transitions_table_body(row,col) = transition.getProduct().getMetaValue("LabelType");
          table_data.body_(row, col) = "?";
          ++col;
          table_data.body_(row, col) = std::to_string(transition.isQuantifyingTransition());
          ++col;
          table_data.body_(row, col) = std::to_string(transition.isIdentifyingTransition());
          ++col;
          table_data.body_(row, col) = std::to_string(transition.isDetectingTransition());
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setSpectrumTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    // Make the spectrum table headers
    if (table_data.headers_.size() <= 0) {
      LOGD << "Making spectrum_table_headers";
      table_data.headers_.resize(8);
      table_data.headers_.setValues({
      "native_id","ms_level","type","base peak m/z", "base peak intensity", "total ion current", "lowest observed m/z", "highest observed m/z" });
    }
    const int n_cols = table_data.headers_.size();
    // Make the spectrum table body
    if (sequence_handler.getSequence().size() > 0) {
      const auto& spectra = sequence_handler.getSequence().at(0).getRawData().getExperiment().getSpectra();
      const int n_rows = spectra.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making spectrum_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& spectrum : spectra) {
          table_data.body_(row, col) = spectrum.getNativeID();
          ++col;
          table_data.body_(row, col) = std::to_string(spectrum.getMSLevel());
          ++col;
          table_data.body_(row, col) = spectrum.getType();
          ++col;
          table_data.body_(row, col) = spectrum.getMetaValue("base peak m/z").toString();
          ++col;
          table_data.body_(row, col) = spectrum.getMetaValue("base peak intensity").toString();
          ++col;
          table_data.body_(row, col) = spectrum.getMetaValue("total ion current").toString();
          ++col;
          table_data.body_(row, col) = spectrum.getMetaValue("lowest observed m/z").toString();
          ++col;
          table_data.body_(row, col) = spectrum.getMetaValue("highest observed m/z").toString();
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setWorkflowTable(const std::vector<ApplicationHandler::Command>& commands, GenericTableData& table_data)
  {
    // Make the workflow table headers
    if (table_data.headers_.size() <= 0) {
      LOGD << "Making workflow_table_headers";
      table_data.headers_.resize(2);
      table_data.headers_.setValues({ "step", "command" });
    }
    const int n_cols = table_data.headers_.size();
    const int n_rows = commands.size();
    // Make the workflow table body
    if (table_data.body_.dimension(0) != commands.size()) { // TODO: does not account for case of different commands of the same length!
      LOGD << "Making workflow_table_body";
      table_data.body_.resize(n_rows, n_cols);
      int col = 0, row = 0;
      for (const auto& command : commands) {
        table_data.body_(row, col) = std::to_string(row);
        ++col;
        table_data.body_(row, col) = command.getName();
        col = 0;
        ++row;
      }
    }
  }

  void SessionHandler::setQuantMethodTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the quant_method table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making quant_method_table_headers";
        table_data.headers_.resize(20);
        table_data.headers_.setValues({
          "component_name", "feature_name", "sequence_segment_name", "IS_name", "concentration_units",
          "llod", "ulod", "lloq", "uloq", "correlation_coefficient",
          "n_points", "transformation_model",
          "transformation_model_param_slope",
          "transformation_model_param_intercept",
          "transformation_model_param_x_weight",
          "transformation_model_param_y_weight",
          "transformation_model_param_x_datum_min",
          "transformation_model_param_x_datum_max",
          "transformation_model_param_y_datum_min",
          "transformation_model_param_y_datum_max" });
      }
      const int n_cols = table_data.headers_.size();
      // Make the quant_method table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().size() * sequence_handler.getSequenceSegments().size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making quant_method_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& seq_segment : sequence_handler.getSequenceSegments()) {
          for (const auto& quant_method : seq_segment.getQuantitationMethods()) {
            table_data.body_(row, col) = quant_method.getComponentName();
            ++col;
            table_data.body_(row, col) = quant_method.getFeatureName();
            ++col;
            table_data.body_(row, col) = seq_segment.getSequenceSegmentName();
            ++col;
            table_data.body_(row, col) = quant_method.getISName();
            ++col;
            table_data.body_(row, col) = quant_method.getConcentrationUnits();
            ++col;
            table_data.body_(row, col) = std::to_string(quant_method.getLLOD());
            ++col;
            table_data.body_(row, col) = std::to_string(quant_method.getULOD());
            ++col;
            table_data.body_(row, col) = std::to_string(quant_method.getLLOQ());
            ++col;
            table_data.body_(row, col) = std::to_string(quant_method.getULOQ());
            ++col;
            table_data.body_(row, col) = std::to_string(quant_method.getCorrelationCoefficient());
            ++col;
            table_data.body_(row, col) = std::to_string(quant_method.getNPoints());
            ++col;
            table_data.body_(row, col) = quant_method.getTransformationModel();
            ++col;
            table_data.body_(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("slope"));
            ++col;
            table_data.body_(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("intercept"));
            ++col;
            table_data.body_(row, col) = (std::string)quant_method.getTransformationModelParams().getValue("x_weight");
            ++col;
            table_data.body_(row, col) = (std::string)quant_method.getTransformationModelParams().getValue("y_weight");
            ++col;
            table_data.body_(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("x_datum_min"));
            ++col;
            table_data.body_(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("x_datum_max"));
            ++col;
            table_data.body_(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("y_datum_min"));
            ++col;
            table_data.body_(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("y_datum_max"));
            col = 0;
            ++row;
          }
        }
      }
    }
  }

  void SessionHandler::setStdsConcsTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    // Make the stds_concs table headers
    if (table_data.headers_.size() <= 0) {
      LOGD << "Making stds_concs_table_headers";
      table_data.headers_.resize(7);
      table_data.headers_.setValues({
        "sample_name", "component_name", "IS_component_name", "actual_concentration",
          "IS_actual_concentration", "concentration_units", "dilution_factor" });
    }
    const int n_cols = table_data.headers_.size();
    // Make the stds_concs table body
    if (sequence_handler.getSequenceSegments().size() > 0) {
      const auto& stand_concs = sequence_handler.getSequenceSegments().at(0).getStandardsConcentrations();
      const int n_rows = stand_concs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making stds_concs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& stds_concs : stand_concs) {
          table_data.body_(row, col) = stds_concs.sample_name;
          ++col;
          table_data.body_(row, col) = stds_concs.component_name;
          ++col;
          table_data.body_(row, col) = stds_concs.IS_component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(stds_concs.actual_concentration);
          ++col;
          table_data.body_(row, col) = std::to_string(stds_concs.IS_actual_concentration);
          ++col;
          table_data.body_(row, col) = stds_concs.concentration_units;
          ++col;
          table_data.body_(row, col) = std::to_string(stds_concs.dilution_factor);
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentFiltersTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the comp_filters table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_filters_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_filters_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_qcs) {
          table_data.body_(row, col) = comp_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupFiltersTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.size() > 0) {
      // Make the comp_group_filters table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_filters_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      // Make the comp_group_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_filters_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs) {
          table_data.body_(row, col) = comp_group_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentQCsTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs.size() > 0) {
      // Make the comp_qcs table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_qcs_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_qcs table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_qcs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs) {
          table_data.body_(row, col) = comp_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupQCsTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.size() > 0) {
      // Make the comp_group_qcs table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_qcs_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.size();
      // Make the comp_group_qcs table body
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_qcs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs) {
          table_data.body_(row, col) = comp_group_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentRSDFiltersTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_qcs.size() > 0) {
      // Make the comp_rsd_filters table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_rsd_filters_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_rsd_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_rsd_filters_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_rsd_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_qcs) {
          table_data.body_(row, col) = comp_rsd_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_rsd_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupRSDFiltersTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_group_qcs.size() > 0) {
      // Make the comp_group_rsd_filters table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_rsd_filters_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      // Make the comp_group_rsd_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_group_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_rsd_filters_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_rsd_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureRSDFilter().component_group_qcs) {
          table_data.body_(row, col) = comp_group_rsd_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_rsd_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentRSDQCsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_qcs.size() > 0) {
      // Make the comp_rsd_qcs table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_rsd_qcs_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_rsd_qcs table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_rsd_qcs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_rsd_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_qcs) {
          table_data.body_(row, col) = comp_rsd_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_rsd_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupRSDQCsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_group_qcs.size() > 0) {
      // Make the comp_group_rsd_qcs table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_rsd_qcs_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_group_qcs.size();
      // Make the comp_group_rsd_qcs table body
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_rsd_qcs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_rsd_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureRSDQC().component_group_qcs) {
          table_data.body_(row, col) = comp_group_rsd_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_rsd_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentBackgroundFiltersTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_qcs.size() > 0) {
      // Make the comp_background_filters table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_background_filters_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_background_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_background_filters_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_background_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_qcs) {
          table_data.body_(row, col) = comp_background_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_background_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupBackgroundFiltersTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_group_qcs.size() > 0) {
      // Make the comp_group_background_filters table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_background_filters_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      // Make the comp_group_background_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_group_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_background_filters_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_background_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundFilter().component_group_qcs) {
          table_data.body_(row, col) = comp_group_background_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_background_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentBackgroundQCsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_qcs.size() > 0) {
      // Make the comp_background_qcs table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_background_qcs_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_background_qcs table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_background_qcs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_background_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_qcs) {
          table_data.body_(row, col) = comp_background_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_background_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupBackgroundQCsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_group_qcs.size() > 0) {
      // Make the comp_group_background_qcs table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_background_qcs_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_group_qcs.size();
      // Make the comp_group_background_qcs table body
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_background_qcs_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_background_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundQC().component_group_qcs) {
          table_data.body_(row, col) = comp_group_background_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_background_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentRSDEstimationsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_qcs.size() > 0) {
      // Make the comp_rsd_estimations table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_rsd_estimations_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_rsd_estimations table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_rsd_estimations_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_rsd_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_qcs) {
          table_data.body_(row, col) = comp_rsd_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_rsd_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_rsd_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupRSDEstimationsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_group_qcs.size() > 0) {
      // Make the comp_group_rsd_estimations table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_rsd_estimations_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      // Make the comp_group_rsd_estimations table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_group_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_rsd_estimations_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_rsd_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureRSDEstimations().component_group_qcs) {
          table_data.body_(row, col) = comp_group_rsd_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_rsd_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_rsd_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_rsd_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentBackgroundEstimationsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_qcs.size() > 0) {
      // Make the comp_background_estimations table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_background_estimations_table_headers";
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();

      // Make the comp_background_estimations table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_background_estimations_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_background_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_qcs) {
          table_data.body_(row, col) = comp_background_qcs.component_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_background_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_background_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  void SessionHandler::setComponentGroupBackgroundEstimationsTable(const SequenceHandler& sequence_handler, GenericTableData& table_data)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 && sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_group_qcs.size() > 0) {
      // Make the comp_group_background_estimations table headers
      if (table_data.headers_.size() <= 0) {
        LOGD << "Making comp_group_background_estimations_table_headers";
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        table_data.headers_.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) table_data.headers_(i) = tmp.at(i);
      }
      const int n_cols = table_data.headers_.size();
      // Make the comp_group_background_estimations table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_group_qcs.size();
      if (table_data.body_.dimension(0) != n_rows) {
        LOGD << "Making comp_group_background_estimations_table_body";
        table_data.body_.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_background_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureBackgroundEstimations().component_group_qcs) {
          table_data.body_(row, col) = comp_group_background_qcs.component_group_name;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.retention_time_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.retention_time_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.intensity_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.intensity_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.overall_quality_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.overall_quality_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_heavy_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_heavy_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_light_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_light_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_detecting_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_detecting_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_quantifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_quantifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_identifying_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_identifying_u);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_transitions_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.n_transitions_u);
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_pair_name_1;
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_pair_name_2;
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.ion_ratio_l);
          ++col;
          table_data.body_(row, col) = std::to_string(comp_group_background_qcs.ion_ratio_u);
          ++col;
          table_data.body_(row, col) = comp_group_background_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_background_qcs.meta_value_qc) {
            table_data.body_(row, col) = std::to_string(meta_data.second.first);
            ++col;
            table_data.body_(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }

  bool SessionHandler::setFeatureTable(const SequenceHandler & sequence_handler, GenericTableData& table_data)
  {
    bool within_max_size = true;
    if (sequence_handler.getSequence().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0) {
      // Make the feature table headers and body
      if (feature_table_unique_samples_transitions_ != getNSelectedSampleNamesPlot()*getNSelectedTransitionsPlot()
          || table_data.body_.dimension(1) != 23 + getNSelectedFeatureMetaValuesTable()) {
        LOGD << "Making feature_table_body and feature_table_headers";
        // get the selected feature names
        Eigen::Tensor<std::string, 1> selected_feature_names = getSelectFeatureMetaValuesTable();
        std::vector<std::string> feature_names;
        for (int i = 0; i < selected_feature_names.size(); ++i) {
          if (std::count(feature_names.begin(), feature_names.end(), selected_feature_names(i)) == 0 && !selected_feature_names(i).empty())
            feature_names.push_back(selected_feature_names(i));
        }
        // get the selected sample types
        std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
        for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
        // get the selected sample names
        Eigen::Tensor<std::string, 1> selected_sample_names = getSelectSampleNamesPlot();
        std::set<std::string> sample_names;
        for (int i = 0; i < selected_sample_names.size(); ++i) {
          if (!selected_sample_names(i).empty())
            sample_names.insert(selected_sample_names(i));
        }
        // get the selected transitions
        Eigen::Tensor<std::string, 1> selected_transitions = getSelectTransitionsPlot();
        std::set<std::string> component_names;
        for (int i = 0; i < selected_transitions.size(); ++i) {
          if (!selected_transitions(i).empty())
            component_names.insert(selected_transitions(i));
        }
        // get the selected transition groups
        Eigen::Tensor<std::string, 1> selected_transition_groups = getSelectTransitionGroupsPlot();
        std::set<std::string> component_group_names;
        for (int i = 0; i < selected_transition_groups.size(); ++i) {
          if (!selected_transition_groups(i).empty())
            component_group_names.insert(selected_transition_groups(i));
        }
        // update the number of unique samples, transitions, and feature metavalues
        feature_table_unique_samples_transitions_ = getNSelectedSampleNamesPlot() * getNSelectedTransitionsPlot();
        std::vector<std::vector<std::string>> table;
        std::vector<std::string> headers;
        SequenceParser::makeDataTableFromMetaValue(sequence_handler, table, headers, feature_names,
                                                   sample_types, sample_names, component_group_names, component_names);
        const int n_cols = headers.size();
        size_t n_rows = table.size();
        table_data.headers_.resize(n_cols);
        table_data.body_.resize(n_rows, n_cols);
        for (size_t row = 0; row < n_rows; ++row) {
          if (n_cols >= 64) { // 64 max. permitted columns as of ImGui v1.81
            within_max_size = false;
            LOGI << "Maximum permitted number of columns reached.";
            break;
          }
          for (int col = 0; col < n_cols; ++col) {
            if (row == 0) table_data.headers_(col) = headers.at(col);
            table_data.body_(row, col) = table.at(row).at(col);
          }
        }
      }
    }
    return within_max_size;
  }

  void SessionHandler::setFeatureMatrix(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequence().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0) {
      // Make the feature_pivot table headers and body
      if (feat_value_data.dimension(1) != getNSelectedSampleNamesPlot() || feature_matrix_unique_transitions_ != getNSelectedTransitionsPlot() * getNSelectedFeatureMetaValuesPlot()) {
        LOGD << "Making feature matrix, line plot, and data tables";
        // get the selected feature names
        Eigen::Tensor<std::string, 1> selected_feature_names = getSelectFeatureMetaValuesPlot();
        std::vector<std::string> feature_names;
        for (int i = 0; i < selected_feature_names.size(); ++i) {
          if (std::count(feature_names.begin(), feature_names.end(), selected_feature_names(i)) == 0 && !selected_feature_names(i).empty())
            feature_names.push_back(selected_feature_names(i));
        }
        // get the selected sample types
        std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
        for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
        // get the selected sample names
        Eigen::Tensor<std::string, 1> selected_sample_names = getSelectSampleNamesPlot();
        std::set<std::string> sample_names;
        for (int i = 0; i < selected_sample_names.size(); ++i) {
          if (!selected_sample_names(i).empty())
            sample_names.insert(selected_sample_names(i));
        }
        // get the selected transitions
        Eigen::Tensor<std::string, 1> selected_transitions = getSelectTransitionsPlot();
        std::set<std::string> component_names;
        for (int i = 0; i < selected_transitions.size(); ++i) {
          if (!selected_transitions(i).empty())
            component_names.insert(selected_transitions(i));
        }
        // get the selected transition groups
        Eigen::Tensor<std::string, 1> selected_transition_groups = getSelectTransitionGroupsPlot();
        std::set<std::string> component_group_names;
        for (int i = 0; i < selected_transition_groups.size(); ++i) {
          if (!selected_transition_groups(i).empty())
            component_group_names.insert(selected_transition_groups(i));
        }
        // Update the unique transitions
        feature_matrix_unique_transitions_ = getNSelectedTransitionsPlot() * getNSelectedFeatureMetaValuesPlot();
        // get the matrix of data
        Eigen::Tensor<std::string, 2> rows_out;
        SequenceParser::makeDataMatrixFromMetaValue(sequence_handler, feat_value_data, feat_col_labels, rows_out, feature_names, sample_types, sample_names, component_group_names, component_names);
        setFeatureLinePlot();
        // update the pivot table headers with the columns for the pivot table/heatmap rows
        feature_pivot_table.headers_.resize((int)feat_col_labels.size() + 3);
        feature_pivot_table.headers_(0) = "feature_name";
        feature_pivot_table.headers_(1) = "component_group_name";
        feature_pivot_table.headers_(2) = "component_name";
        feature_pivot_table.headers_.slice(Eigen::array<Eigen::Index, 1>({ 3 }), Eigen::array<Eigen::Index, 1>({ feat_col_labels.size() })) = feat_col_labels;
        const int n_cols = feature_pivot_table.headers_.size();
        const int n_rows = feat_value_data.dimension(0);
        // allocate space for the pivot table body and heatmap row labels
        feat_row_labels.resize(n_rows);
        feature_pivot_table.body_.resize(n_rows, n_cols);
        // assign the pivot table body data and heatmap row labels
        int col = 0;
        for (int row = 0; row < n_rows; ++row) {
          feat_row_labels(row) = rows_out(row, 0) + "::" + rows_out(row, 2);
          for (int j = 0; j < rows_out.dimension(1); ++j) {
            feature_pivot_table.body_(row, col) = rows_out(row, j);
            ++col;
          }
          for (int j = 0; j < feat_value_data.dimension(1); ++j) {
            feature_pivot_table.body_(row, col) = std::to_string(feat_value_data(row, j));
            ++col;
          }
          col = 0;
        }
      }
    }
  }
  
  void SessionHandler::getChromatogramScatterPlot(const SequenceHandler & sequence_handler, 
                                                  GraphVizData& result, 
                                                  const std::pair<float, float>& chrom_time_range,
                                                  const std::set<std::string>& sample_names,
                                                  const std::set<std::string>& component_names) const
  {
    if (sequence_handler.getSequence().size() > 0 &&
      (sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0 ||
      sequence_handler.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size() > 0))
    {
      LOGD << "Making the chromatogram data for plotting";
      // Set the axes titles and min/max defaults
      result.reset("Time (sec)", "Intensity (au)", {}, max_nb_points);
      for (const auto& injection : sequence_handler.getSequence()) {
        if (sample_names.count(injection.getMetaData().getSampleName()) == 0) continue;
        // Extract out the raw data for plotting
        for (const auto& chromatogram : injection.getRawData().getChromatogramMap().getChromatograms()) {
          if (component_names.count(chromatogram.getNativeID()) == 0) continue;
          std::vector<float> x_data, y_data;
          //for (const auto& point : chromatogram) {
          for (auto point = chromatogram.PosBegin(chrom_time_range.first); point != chromatogram.PosEnd(chrom_time_range.second); ++point) {
            x_data.push_back(point->getRT());
            y_data.push_back(point->getIntensity());
          }
          if (!result.addData(x_data, y_data, injection.getMetaData().getSampleName() + "::" + chromatogram.getNativeID()))
          {
            return;
          }
        }
        // Extract out the smoothed points for plotting
        for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
          for (const auto& subordinate : feature.getSubordinates()) {
            if (subordinate.getMetaValue("used_") == "true" && component_names.count(subordinate.getMetaValue("native_id").toString())) {
              std::vector<float> x_data, y_data;
              for (const auto& point : subordinate.getConvexHull().getHullPoints()) {
                if (point.getX() < chrom_time_range.first || point.getX() > chrom_time_range.second) continue;
                x_data.push_back(point.getX());
                y_data.push_back(point.getY());
              }
              if (x_data.size() <= 0) continue;
              if (!result.addScatterData(x_data,
                                    y_data,
                                    injection.getMetaData().getSampleName() + 
                                    "::" + subordinate.getMetaValue("native_id").toString() + 
                                    "::" + subordinate.getMetaValue("timestamp_").toString()))
              {
                return;
              }
            }
          }
        }
      }
    }
  }

  void SessionHandler::getChromatogramTIC(const SequenceHandler& sequence_handler,
    GraphVizData& result,
    const std::pair<float, float>& chrom_time_range,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& scan_names) const
  {
    LOGD << "Making the chromatogram TIC data for plotting";
    // Set the axes titles and min/max defaults
    result.reset("Time (sec)", "Intensity (au)", {}, max_nb_points);
    for (const auto& injection : sequence_handler.getSequence())
    {
      if (sample_names.count(injection.getMetaData().getSampleName()) == 0) continue;
      const auto& experiment = injection.getRawData().getExperiment();
      std::vector<float> x_data, y_data;
      for (const auto& spectrum : experiment)
      {
        const auto ms_level = spectrum.getMSLevel();
        if (ms_level == 1)
        {
          if (scan_names.count(spectrum.getNativeID()) == 0) continue;
          x_data.push_back(spectrum.getRT());
          y_data.push_back(spectrum.calculateTIC());
        }
      }
      if (!result.addData(x_data, y_data, injection.getMetaData().getSampleName()))
      {
        return;
      }
    }
  }

  void SessionHandler::getChromatogramXIC(const SequenceHandler& sequence_handler,
      GraphVizData& result,
      const std::pair<float, float>& chrom_time_range,
      const std::set<std::string>& sample_names,
      const std::set<std::string>& component_names,
      float mz) const
  {
    if (sequence_handler.getSequence().size() > 0 &&
      (sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0 
      ))
    {
      LOGD << "Making the chromatogram data for plotting";
      // Set the axes titles and min/max defaults
      result.reset("Time (sec)", "Intensity (au)", {}, max_nb_points);
      for (const auto& injection : sequence_handler.getSequence())
      {
        // Extract out the smoothed points for plotting
        for (const auto& feature : injection.getRawData().getFeatureMapHistory())
        {
          for (const auto& subordinate : feature.getSubordinates()) 
          {
            auto feature_mz = subordinate.getPosition()[1];
            if ((int)feature_mz == (int)mz)
            {
              std::vector<float> x_data, y_data;
              for (const auto& point : subordinate.getConvexHull().getHullPoints()) {
                // try to find intensity in the spectra.
                auto hull_rt = point.getX();
                auto hull_mz = point.getY();
                float intensity = 0.0f;
                for (const auto& spectrum : injection.getRawData().getExperiment())
                {
                  if (abs(spectrum.getRT() - hull_rt) < 1e-06)
                  {
                    for (const auto& peak : spectrum)
                    {
                      if (abs(peak.getMZ() - hull_mz) < 1e-06)
                      {
                        intensity = peak.getIntensity();
                        break;
                      }
                    }
                  }
                }
                //
                if (point.getX() < chrom_time_range.first || point.getX() > chrom_time_range.second) continue;
                x_data.push_back(point.getX());
                y_data.push_back(intensity);
              }
              if (x_data.size() <= 0) continue;
              if (!result.addData(x_data,
                             y_data,
                             injection.getMetaData().getSampleName() + 
                             "::" + subordinate.getMetaValue("native_id").toString() + 
                             "::" + subordinate.getMetaValue("timestamp_").toString()))
              {
                return;
              }
            }
          }
        }
      }
    }
  }

  void SessionHandler::getSpectrumScatterPlot(const SequenceHandler& sequence_handler,
    GraphVizData& result,
    const std::pair<float, float>& range,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& scan_names,
    const std::set<std::string>& component_group_names) const
  {
    // Notes: native_id matches the spec NativeID (i.e., scan_names), PeptideRef matches the first annotation identifier
    if (sequence_handler.getSequence().size() > 0 &&
       (sequence_handler.getSequence().at(0).getRawData().getExperiment().getSpectra().size() > 0 ||
       sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0))
    {
      LOGD << "Making the spectra data for plotting";
      // Set the axes titles and min/max defaults
      result.reset("m/z (Da)", "Intensity (au)", {}, max_nb_points);
      for (const auto& injection : sequence_handler.getSequence()) {
        if (sample_names.count(injection.getMetaData().getSampleName()) == 0) continue;
        // Extract out the raw data for plotting
        for (const auto& spectra : injection.getRawData().getExperiment().getSpectra()) {
          if (scan_names.count(spectra.getNativeID()) == 0) continue;
          std::vector<float> x_data, y_data;
          for (auto point = spectra.PosBegin(range.first); point != spectra.PosEnd(range.second); ++point) {
            x_data.push_back(point->getMZ());
            y_data.push_back(point->getIntensity());
          }
          if (!result.addData(x_data, y_data, injection.getMetaData().getSampleName() + "::" + spectra.getNativeID()))
          {
            return;
          }
        }
        // Extract out the smoothed points for plotting
        for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
          for (const auto& subordinate : feature.getSubordinates()) {
            if (subordinate.getMetaValue("used_") == "true" && component_group_names.count(subordinate.getMetaValue("PeptideRef").toString())) {
              std::vector<float> x_data, y_data;
              for (const auto& point : subordinate.getConvexHull().getHullPoints()) {
                if (point.getX() < range.first || point.getX() > range.first) continue;
                x_data.push_back(point.getX());
                y_data.push_back(point.getY());
              }
              if (x_data.size() <= 0) continue;
              if (!result.addScatterData(x_data,
                             y_data, 
                             injection.getMetaData().getSampleName() + 
                             "::" + subordinate.getMetaValue("chemical_formula").toString() + 
                             ":" + subordinate.getMetaValue("modifications").toString()))
              {
                return;
              }
            }
          }
        }
      }
    }
  }

  void SessionHandler::getSpectrumMSMSPlot(const SequenceHandler& sequence_handler,
    GraphVizData& result,
    const std::pair<float, float>& range,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& scan_names,
    const std::set<std::string>& component_group_names,
    const float rt,
    const int ms_level) const
  {
    if (sequence_handler.getSequence().size() > 0 &&
      (sequence_handler.getSequence().at(0).getRawData().getExperiment().getSpectra().size() > 0 ||
        sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0)) {
      LOGD << "Making the spectra data for plotting";
      // Set the axes titles and min/max defaults
      result.reset("m/z (Da)", "Intensity (au)", "Retention Time (s)", max_nb_points);
      int total_nb_points = 0;
      for (const auto& injection : sequence_handler.getSequence())
      {
        if (sample_names.count(injection.getMetaData().getSampleName()) == 0) continue;
        // Extract out the raw data for plotting
        for (const auto& spectra : injection.getRawData().getExperiment().getSpectra())
        {
          if (spectra.getMSLevel() == ms_level)
          {
            result.z_data_area_.push_back(spectra.getRT());
            if (scan_names.count(spectra.getNativeID()) == 0) continue;
            std::vector<float> x_data, y_data;
            const float rt_threashold = 0.50f;
            if ((!result.points_overflow_) && (abs(spectra.getRT()-rt)< rt_threashold))
            {
              for (auto point = spectra.PosBegin(range.first); point != spectra.PosEnd(range.second); ++point)
              {
                x_data.push_back(point->getMZ());
                y_data.push_back(point->getIntensity());
              }
              result.addData(x_data, y_data, injection.getMetaData().getSampleName() + "::" + spectra.getNativeID());
            }
          }
        }
      }
      if (result.nb_points_ == 0)
      {
        // we will set the range to user's range
        result.x_min_ = range.first;
        result.x_max_ = range.second;
      }
    }
  }

  void SessionHandler::setFeatureLinePlot()
  {
    // Set the axes titles and min/max defaults
    feat_line_x_axis_title = "Injections";
    std::string feat_line_y_axis_title = "Value";
    feat_line_sample_min = 1e6; 
    feat_line_sample_max = 0; 
    feat_value_min = 1e6; 
    feat_value_max = 0;
    // make the injection index for the line plot
    const int n_samples = feat_col_labels.size();
    const int n_rows = feat_value_data.dimension(0);
    feat_sample_data.resize(n_rows, n_samples);
    for (float i = 0; i < n_samples; i += 1) feat_sample_data.chip(i, 1) = feat_sample_data.chip(i, 1).constant(i);
    feat_line_sample_min = 0;
    feat_line_sample_max = n_samples - 1;
    const Eigen::Tensor<float, 0> feat_value_data_maximum = feat_value_data.maximum();
    feat_value_max = feat_value_data_maximum(0);
    const Eigen::Tensor<float, 0> feat_value_data_minimum = feat_value_data.minimum();
    feat_value_min = feat_value_data_minimum(0);
  }
  void SessionHandler::getHeatMap(const SequenceHandler& sequence_handler, HeatMapData& result, const std::string& feature_name)
  {
    LOGD << "Getting Heatmap data";
    std::vector<std::string> feature_names;
    feature_names.push_back(feature_name);
    // get the selected sample types
    std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
    for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
    // get the selected sample names
    Eigen::Tensor<std::string, 1> selected_sample_names = getSelectSampleNamesPlot();
    std::set<std::string> sample_names;
    for (int i = 0; i < selected_sample_names.size(); ++i) {
      if (!selected_sample_names(i).empty())
        sample_names.insert(selected_sample_names(i));
    }
    // get the selected transitions
    Eigen::Tensor<std::string, 1> selected_transitions = getSelectTransitionsPlot();
    std::set<std::string> component_names;
    for (int i = 0; i < selected_transitions.size(); ++i) {
      if (!selected_transitions(i).empty())
        component_names.insert(selected_transitions(i));
    }
    // get the selected transition groups
    Eigen::Tensor<std::string, 1> selected_transition_groups = getSelectTransitionGroupsPlot();
    std::set<std::string> component_group_names;
    for (int i = 0; i < selected_transition_groups.size(); ++i) {
      if (!selected_transition_groups(i).empty())
        component_group_names.insert(selected_transition_groups(i));
    }
    // Update the unique transitions
    feature_matrix_unique_transitions_ = getNSelectedTransitionsPlot() * getNSelectedFeatureMetaValuesPlot();
    // get the matrix of data
    if (sequence_handler.getSequence().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0)
    {
      Eigen::Tensor<std::string, 2> rows_out;
      Eigen::Tensor<float, 2> heatmap_value_data;
      SequenceParser::makeDataMatrixFromMetaValue(sequence_handler,
                                                  heatmap_value_data,
                                                  result.feat_heatmap_col_labels,
                                                  rows_out,
                                                  feature_names,
                                                  sample_types,
                                                  sample_names,
                                                  component_group_names,
                                                  component_names);
      const int n_rows = heatmap_value_data.dimension(0);
      // allocate space for the pivot table body and heatmap row labels
      result.feat_heatmap_row_labels.resize(n_rows);
      for (int row = 0; row < n_rows; ++row) {
        result.feat_heatmap_row_labels(row) = rows_out(row, 0);
      }
      // assign the heatmap data
      result.feat_heatmap_data.resize(heatmap_value_data.dimensions());
      result.feat_heatmap_data = heatmap_value_data.swap_layout().shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
      const Eigen::Tensor<float, 0> feat_value_data_maximum = heatmap_value_data.maximum();
      result.feat_value_max_ = feat_value_data_maximum(0);
      const Eigen::Tensor<float, 0> feat_value_data_minimum = heatmap_value_data.minimum();
      result.feat_value_min_ = feat_value_data_minimum(0);
    }
    else
    {
      result.feat_heatmap_data.setZero();
      result.feat_value_max_ = std::numeric_limits<float>::min();
      result.feat_value_min_ = std::numeric_limits<float>::max();
    }
    // Set the axes titles
    result.feat_heatmap_x_axis_title = "Injections";
    result.feat_heatmap_y_axis_title = "Transitions";
    result.selected_feature_ = feature_name;
    result.selected_sample_names_ = selected_sample_names;
    result.selected_transitions_ = selected_transitions;
    result.selected_transition_groups_ = selected_transition_groups;
  }
  bool SessionHandler::setCalibratorsScatterLinePlot(const SequenceHandler & sequence_handler, CalibrationData& result)
  {
    int n_points = 0;
    if (sequence_handler.getSequenceSegments().size() > 0 &&
      sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().size() > 0 &&
      sequence_handler.getSequenceSegments().at(0).getComponentsToConcentrations().size() > 0 &&
      sequence_handler.getSequenceSegments().at(0).getStandardsConcentrations().size() > 0) {
      // get the selected transitions
      Eigen::Tensor<std::string, 1> selected_transitions = getSelectTransitionsPlot();
      std::set<std::string> component_names;
      for (int i = 0; i < selected_transitions.size(); ++i) {
        if (!selected_transitions(i).empty())
          component_names.insert(selected_transitions(i));
      }
      if (result.conc_fit_data.size() != component_names.size() && result.conc_raw_data.size() != component_names.size())
      {
        // LOGD << "Making the calibrators data for plotting";
        // Update the axis titles and clear the data
        result.x_axis_title = "Concentration ratio (" + sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().at(0).getConcentrationUnits() + ")";
        result.y_axis_title = sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().at(0).getFeatureName() + " response";
        result.conc_min = 1e6;
        result.conc_max = 0;
        result.feature_min = 1e6;
        result.feature_max = 0;
        result.conc_fit_data.clear();
        result.feature_fit_data.clear();
        result.conc_raw_data.clear();
        result.feature_raw_data.clear();
        result.injections.clear();
        result.feature_concentrations.clear();
        result.outlier_conc_raw_data.clear();
        result.outlier_feature_raw_data.clear();
        result.outlier_injections.clear();
        result.outlier_feature_concentrations.clear();
        result.series_names.clear();
        for (const auto& sequence_segment : sequence_handler.getSequenceSegments()) {
          // Extract out raw data used to make the calibrators found in `StandardsConcentrations`
          std::map<std::string, std::pair<std::vector<float>, std::vector<std::string>>> stand_concs_map; // map of x_data and sample_name for a component
          for (const auto& stand_concs : sequence_segment.getStandardsConcentrations())
          {
            const float x_datum = float(stand_concs.actual_concentration / stand_concs.IS_actual_concentration / stand_concs.dilution_factor);
            auto found = stand_concs_map.emplace(stand_concs.component_name,
                                                  std::make_pair(
                                                    std::vector<float>({ x_datum }),
                                                    std::vector<std::string>({ stand_concs.sample_name })));
            if (!found.second)
            {
              stand_concs_map.at(stand_concs.component_name).first.push_back(x_datum);
              stand_concs_map.at(stand_concs.component_name).second.push_back(stand_concs.sample_name);
            }
          }
          // Make the line of best fit using the `QuantitationMethods`
          for (const auto& quant_method : sequence_segment.getQuantitationMethods())
          {
            // Skip components that have not been fitted with a calibration curve
            if (sequence_segment.getComponentsToConcentrations().count(quant_method.getComponentName()) > 0 &&
                component_names.count(quant_method.getComponentName()) > 0)
            { 
              // Make the line of best fit using the `QuantitationMethods`
              std::vector<float> y_fit_data;
              if (quant_method.getNPoints() > 0)
              {
                for (const auto& ratio : stand_concs_map.at(quant_method.getComponentName()).first) {
                  // TODO: encapsulate in its own method e.g. sequenceSegmentProcessor
                  // TODO: check that the calibration actually found a best fit (and set to all 0 if not)
                  // calculate the absolute concentration
                  OpenMS::TransformationModel::DataPoints data;
                  OpenMS::TransformationDescription tmd(data);
                  tmd.fitModel(quant_method.getTransformationModel(), quant_method.getTransformationModelParams());
                  float calculated_feature_ratio = tmd.apply(ratio);
                  // check for less than zero
                  if (calculated_feature_ratio < 0.0)
                  {
                    calculated_feature_ratio = 0.0;
                  }
                  y_fit_data.push_back(calculated_feature_ratio);
                  result.conc_min = std::min(ratio, result.conc_min);
                  result.conc_max = std::max(ratio, result.conc_max);
                  result.feature_min = std::min(calculated_feature_ratio, result.feature_min);
                  result.feature_max = std::max(calculated_feature_ratio, result.feature_max);
                }
                n_points += y_fit_data.size();
                if (n_points < max_nb_points) {
                  result.conc_fit_data.push_back(stand_concs_map.at(quant_method.getComponentName()).first);
                  result.feature_fit_data.push_back(y_fit_data);
                }
                else
                {
                  LOGD << "Stopped adding points to calibrators plot";
                  return false;
                }
              }
              // Extract out the points used to make the line of best fit in `ComponentsToConcentrations`
              std::vector<float> x_raw_data, y_raw_data;
              std::vector<std::string> injections;
              std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;
              OpenMS::AbsoluteQuantitation absQuant;
              for (const auto& point : sequence_segment.getComponentsToConcentrations().at(quant_method.getComponentName()))
              {
                auto ratio = float(point.actual_concentration / point.IS_actual_concentration / point.dilution_factor);
                x_raw_data.push_back(ratio);
                float y_datum = absQuant.calculateRatio(point.feature, point.IS_feature, quant_method.getFeatureName());
                y_raw_data.push_back(y_datum);
                injections.push_back(findSampleNameFromStandardConcentration(sequence_segment, quant_method.getComponentName(), point));
                feature_concentrations.push_back(point);
                result.feature_min = std::min(y_datum, result.feature_min);
                result.feature_max = std::max(y_datum, result.feature_max);
                result.conc_min = std::min(ratio, result.conc_min);
                result.conc_max = std::max(ratio, result.conc_max);
              }
              n_points += x_raw_data.size();
              // Extract out the points out of the line of best fit in `ComponentsToConcentrations`
              std::vector<float> outlier_x_raw_data, outlier_y_raw_data;
              std::vector<std::string> outlier_injections;
              std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> outlier_feature_concentrations;
              for (const auto& point : sequence_segment.getOutlierComponentsToConcentrations().at(quant_method.getComponentName())) {
                auto ratio = float(point.actual_concentration / point.IS_actual_concentration / point.dilution_factor);
                outlier_x_raw_data.push_back(ratio);
                float y_datum = absQuant.calculateRatio(point.feature, point.IS_feature, quant_method.getFeatureName());
                outlier_y_raw_data.push_back(y_datum);
                outlier_injections.push_back(findSampleNameFromStandardConcentration(sequence_segment, quant_method.getComponentName(), point));
                outlier_feature_concentrations.push_back(point);
                result.feature_min = std::min(y_datum, result.feature_min);
                result.feature_max = std::max(y_datum, result.feature_max);
                result.conc_min = std::min(ratio, result.conc_min);
                result.conc_max = std::max(ratio, result.conc_max);
              }
              n_points += outlier_x_raw_data.size();
              // add points
              if (n_points < max_nb_points) {
                result.conc_raw_data.push_back(x_raw_data);
                result.feature_raw_data.push_back(y_raw_data);
                result.injections.push_back(injections);
                result.feature_concentrations.push_back(feature_concentrations);
                result.outlier_conc_raw_data.push_back(outlier_x_raw_data);
                result.outlier_feature_raw_data.push_back(outlier_y_raw_data);
                result.outlier_injections.push_back(injections);
                result.outlier_feature_concentrations.push_back(outlier_feature_concentrations);
                result.series_names.push_back(quant_method.getComponentName());
              }
              else
              {
                LOGD << "Stopped adding points to calibrators plot";
                return false;
              }
            }
          }
        }
        // Sort data
        for (int j = 0; j < result.conc_fit_data.size(); ++j)
        {
          auto& fit_data_x = result.conc_fit_data[j];
          auto& fit_data_y = result.feature_fit_data[j];
          std::vector<std::pair<float,float>> sorting_vector;
          for (int i = 0; i < fit_data_x.size(); ++i)
          {
            const auto& x = fit_data_x[i];
            const auto& y = fit_data_y[i];
            const auto pair_x_y = std::make_pair(x, y);
            sorting_vector.push_back(pair_x_y);
          }
          std::sort(std::begin(sorting_vector), std::end(sorting_vector),
            [&](const auto& a, const auto& b)
          {
            return a.first > b.first;
          });
          fit_data_x.clear();
          fit_data_y.clear();
          for (const auto& p : sorting_vector)
          {
            fit_data_x.push_back(p.first);
            fit_data_y.push_back(p.second);
          }
        }
      }
    }
    return (n_points < max_nb_points);
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getInjectionExplorerHeader()
  {
    if (sequence_table.headers_.size())
      return sequence_table.headers_.slice(Eigen::array<Eigen::Index, 1>({ 0 }), Eigen::array<Eigen::Index, 1>({ 2 }));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 2> SessionHandler::getInjectionExplorerBody()
  {
    if (sequence_table.body_.size())
      return sequence_table.body_.slice(Eigen::array<Eigen::Index, 2>({ 0,0 }), Eigen::array<Eigen::Index, 2>({ sequence_table.body_.dimension(0), 2 }));
    else
      return Eigen::Tensor<std::string, 2>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getTransitionExplorerHeader()
  {
    if (transitions_table.headers_.size())
      return transitions_table.headers_.slice(Eigen::array<Eigen::Index, 1>({ 0 }), Eigen::array<Eigen::Index, 1>({ 2 }));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 2> SessionHandler::getTransitionExplorerBody()
  {
    if (transitions_table.body_.size())
      return transitions_table.body_.slice(Eigen::array<Eigen::Index, 2>({ 0,0 }), Eigen::array<Eigen::Index, 2>({ transitions_table.body_.dimension(0), 2 }));
    else
      return Eigen::Tensor<std::string, 2>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSpectrumExplorerHeader()
  {
    if (spectrum_table.headers_.size())
      return spectrum_table.headers_.slice(Eigen::array<Eigen::Index, 1>({ 0 }), Eigen::array<Eigen::Index, 1>({ 1 }));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 2> SessionHandler::getSpectrumExplorerBody()
  {
    if (spectrum_table.body_.size())
      return spectrum_table.body_.slice(Eigen::array<Eigen::Index, 2>({ 0,0 }), Eigen::array<Eigen::Index, 2>({ spectrum_table.body_.dimension(0), 1 }));
    else
      return Eigen::Tensor<std::string, 2>();
  }
  Eigen::Tensor<bool, 1> SessionHandler::getSequenceTableFilters()
  {
    if (sequence_table.body_.size() && injection_explorer_data.checkbox_body.size())
      return injection_explorer_data.checkbox_body.chip(2, 1);
    else
      return Eigen::Tensor<bool, 1>();
  }
  Eigen::Tensor<bool, 1> SessionHandler::getTransitionsTableFilters()
  {
    if (transitions_table.body_.size() && transition_explorer_data.checkbox_body.size())
      return transition_explorer_data.checkbox_body.chip(1, 1);
    else
      return Eigen::Tensor<bool, 1>();
  }
  Eigen::Tensor<bool, 1> SessionHandler::getSpectrumTableFilters()
  {
    if (spectrum_table.body_.size() && spectrum_explorer_data.checkbox_body.size())
      return spectrum_explorer_data.checkbox_body.chip(1, 1);
    else
      return Eigen::Tensor<bool, 1>();
  }

  Eigen::Tensor<bool, 1> SessionHandler::getFiltersTable(const Eigen::Tensor<std::string, 2>& to_filter) const
  {
    if (to_filter.size() && transition_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<std::string, 1> selected_transitions = getSelectTransitionsTable();
      Eigen::Tensor<std::string, 1> table_transitions = to_filter.chip(0, 1);
      Eigen::Tensor<bool, 1> table_filters(to_filter.dimension(0));
      table_filters.setConstant(true);
      for (int row = 0; row < to_filter.dimension(0); ++row) {
        if (std::count(selected_transitions.data(), selected_transitions.data() + selected_transitions.size(), table_transitions(row)) == 0)
          table_filters(row) = false;
      }
      return table_filters;
    }
    else
      return Eigen::Tensor<bool, 1>();
  }

  Eigen::Tensor<bool, 1> SessionHandler::getGroupFiltersTable(const Eigen::Tensor<std::string, 2>& to_filter) const
  {
    if (to_filter.size() && transition_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<std::string, 1> selected_transitions = getSelectTransitionGroupsTable();
      Eigen::Tensor<std::string, 1> table_transitions = to_filter.chip(0, 1);
      Eigen::Tensor<bool, 1> table_filters(to_filter.dimension(0));
      table_filters.setConstant(true);
      for (int row = 0; row < to_filter.dimension(0); ++row) {
        if (std::count(selected_transitions.data(), selected_transitions.data() + selected_transitions.size(), table_transitions(row)) == 0)
          table_filters(row) = false;
      }
      return table_filters;
    }
    else
      return Eigen::Tensor<bool, 1>();
  }

  std::set<std::string> SessionHandler::getSelectInjectionNamesWorkflow(const SequenceHandler& sequence_handler)
  {
    if (sequence_table.body_.size() && injection_explorer_data.checkbox_headers.size()) {
      Eigen::Tensor<bool, 1> selected_injections_workflow = injection_explorer_data.checkbox_body.chip(0, 1);
      std::set<std::string> injection_names;
      for (int row = 0; row < selected_injections_workflow.size(); ++row) {
        if (selected_injections_workflow(row)) {
          std::string injection_name = sequence_handler.getSequence().at(row).getMetaData().getInjectionName();
          injection_names.insert(injection_name);
        }
      }
      return injection_names;
    }
    else
      return std::set<std::string>();
  }
  std::set<std::string> SessionHandler::getSelectSequenceSegmentNamesWorkflow(const SequenceHandler & sequence_handler)
  {
    if (sequence_table.body_.size() && injection_explorer_data.checkbox_headers.size()) {
      Eigen::Tensor<bool, 1> selected_injections_workflow = injection_explorer_data.checkbox_body.chip(0, 1);
      std::set<std::string> sequence_segment_names;
      for (int row = 0; row < selected_injections_workflow.size(); ++row) {
        if (selected_injections_workflow(row)) {
          std::string sequence_segment_name = sequence_handler.getSequence().at(row).getMetaData().getSequenceSegmentName();
          sequence_segment_names.insert(sequence_segment_name);
        }
      }
      return sequence_segment_names;
    }
    else
      return std::set<std::string>();
  }
  
  std::set<std::string> SessionHandler::getSelectSampleGroupNamesWorkflow(const SequenceHandler& sequence_handler)
  {
    if (sequence_table.body_.size() && injection_explorer_data.checkbox_headers.size()) {
      Eigen::Tensor<bool, 1> selected_injections_workflow = injection_explorer_data.checkbox_body.chip(0, 1);
      std::set<std::string> sample_group_names;
      for (int row = 0; row < selected_injections_workflow.size(); ++row) {
        if (selected_injections_workflow(row)) {
          std::string sample_group_name = sequence_handler.getSequence().at(row).getMetaData().getSampleGroupName();
          sample_group_names.insert(sample_group_name);
        }
      }
      return sample_group_names;
    }
    else
      return std::set<std::string>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectSampleNamesTable()
  {
    if (sequence_table.body_.size() && injection_explorer_data.checkbox_headers.size())
      return (injection_explorer_data.checkbox_body.chip(2, 1)).select(sequence_table.body_.chip(1, 1), sequence_table.body_.chip(1, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectSampleNamesPlot()
  {
    if (sequence_table.body_.size() && injection_explorer_data.checkbox_headers.size())
      return (injection_explorer_data.checkbox_body.chip(1,1)).select(sequence_table.body_.chip(1,1), sequence_table.body_.chip(1, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectTransitionsTable() const
  {
    if (transitions_table.body_.size() && transition_explorer_data.checkbox_headers.size())
      return (transition_explorer_data.checkbox_body.chip(1, 1)).select(transitions_table.body_.chip(1, 1), transitions_table.body_.chip(1, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectTransitionGroupsTable() const
  {
    if (transitions_table.body_.size() && transition_explorer_data.checkbox_headers.size())
      return (transition_explorer_data.checkbox_body.chip(1, 1)).select(transitions_table.body_.chip(0, 1), transitions_table.body_.chip(0, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectTransitionsPlot()
  {
    if (transitions_table.body_.size() && transition_explorer_data.checkbox_headers.size())
      return (transition_explorer_data.checkbox_body.chip(0, 1)).select(transitions_table.body_.chip(1, 1), transitions_table.body_.chip(1, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectTransitionGroupsPlot()
  {
    if (transitions_table.body_.size() && transition_explorer_data.checkbox_headers.size())
      return (transition_explorer_data.checkbox_body.chip(0, 1)).select(transitions_table.body_.chip(0, 1), transitions_table.body_.chip(0, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectFeatureMetaValuesTable()
  {
    if (feature_table.body_.size() && feature_explorer_data.checkbox_headers.size())
      return (feature_explorer_data.checkbox_body.chip(1, 1)).select(feature_table.body_.chip(0, 1), feature_table.body_.chip(0, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectFeatureMetaValuesPlot()
  {
    if (feature_table.body_.size() && feature_explorer_data.checkbox_headers.size())
      return (feature_explorer_data.checkbox_body.chip(0, 1)).select(feature_table.body_.chip(0, 1), feature_table.body_.chip(0, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getSelectSpectrumPlot()
  {
    if (spectrum_table.body_.size() && spectrum_explorer_data.checkbox_headers.size())
      return (spectrum_explorer_data.checkbox_body.chip(0, 1)).select(spectrum_table.body_.chip(0, 1), spectrum_table.body_.chip(0, 1).constant(""));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  int SessionHandler::getNSelectedSampleNamesTable()
  {
    if (injection_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<int, 0> n_sample_names = injection_explorer_data.checkbox_body.chip(2, 1).cast<int>().sum();
      return n_sample_names(0);
    }
    else
      return 0;
  }
  int SessionHandler::getNSelectedSampleNamesPlot()
  {
    if (injection_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<int, 0> n_sample_names = injection_explorer_data.checkbox_body.chip(1, 1).cast<int>().sum();
      return n_sample_names(0);
    }
    else
      return 0;
  }
  int SessionHandler::getNSelectedTransitionsTable()
  {
    if (transition_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<int, 0> n_transitions = transition_explorer_data.checkbox_body.chip(1, 1).cast<int>().sum();
      return n_transitions(0);
    }
    else
      return 0;
  }
  int SessionHandler::getNSelectedTransitionsPlot()
  {
    if (transition_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<int, 0> n_transitions = transition_explorer_data.checkbox_body.chip(0, 1).cast<int>().sum();
      return n_transitions(0);
    }
    else
      return 0;
  }
  int SessionHandler::getNSelectedFeatureMetaValuesTable()
  {
    if (feature_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<int, 0> n_feature_names = feature_explorer_data.checkbox_body.chip(1, 1).cast<int>().sum();
      return n_feature_names(0);
    }
    else
      return 0;
  }
  int SessionHandler::getNSelectedFeatureMetaValuesPlot()
  {
    if (feature_explorer_data.checkbox_body.size()) {
      Eigen::Tensor<int, 0> n_feature_names = feature_explorer_data.checkbox_body.chip(0, 1).cast<int>().sum();
      return n_feature_names(0);
    }
    else
      return 0;
  }
}
