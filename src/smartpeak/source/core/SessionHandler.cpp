// TODO: Add copyright

#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>
#include <SmartPeak/io/SequenceParser.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void SessionHandler::setSequenceDataAndFilters(const SequenceHandler & sequence_handler)
  {
    // Set the data for the sequence table
    setSequenceTable(sequence_handler);
    // Set the data for the injection explorer
    setInjectionExplorer();
  }
  void SessionHandler::setTransitionsDataAndFilters(const SequenceHandler & sequence_handler)
  {
    // Set the data for all tables that are based on the transitions
    setTransitionsTable(sequence_handler);
    setQuantMethodTable(sequence_handler);
    setComponentFiltersTable(sequence_handler);
    setComponentGroupFiltersTable(sequence_handler);
    setComponentQCsTable(sequence_handler);
    setComponentGroupQCsTable(sequence_handler);
    // Set the data for the transitions explorer
    setTransitionExplorer();
  }
  void SessionHandler::setInjectionExplorer(){//(const SequenceHandler& sequence_handler) {
    // Make the injection explorer headers
    if (injection_explorer_checkbox_headers.size() <= 0) {
      injection_explorer_checkbox_headers.resize(3);
      injection_explorer_checkbox_headers.setValues({ "workflow", "plot", "table" });
    }
    const int n_rows = sequence_table_body.dimension(0);
    // Make the injection explorer body
    if (injection_explorer_checkbox_body.size() <= 0 && n_rows > 0) {
      injection_explorer_checkbox_body.resize(n_rows, (int)injection_explorer_checkbox_headers.size());
      injection_explorer_checkbox_body.setConstant(true);
      for (int i = 1; i < injection_explorer_checkbox_body.dimension(0); ++i) injection_explorer_checkbox_body(i, 1) = false; // only the first injection
      injection_explorer_checked_rows.resize(n_rows);
      injection_explorer_checked_rows.setConstant(true);
    }
  }
  void SessionHandler::setTransitionExplorer() {
    // Make the transition explorer headers
    if (transition_explorer_checkbox_headers.size() <= 0) {
      transition_explorer_checkbox_headers.resize(3);
      transition_explorer_checkbox_headers.setValues({ "workflow", "plot", "table" });
    }
    const int n_rows = transitions_table_body.dimension(0);
    // Make the transition table body
    if (transition_explorer_checkbox_body.size() <= 0 && n_rows > 0) {
      transition_explorer_checkbox_body.resize(n_rows, (int)transition_explorer_checkbox_headers.size());
      transition_explorer_checkbox_body.setConstant(true);
      for (int i = 1; i < transition_explorer_checkbox_body.dimension(0); ++i) transition_explorer_checkbox_body(i, 1) = false; // only the first transition
      transition_explorer_checked_rows.resize(n_rows);
      transition_explorer_checked_rows.setConstant(true);
    }
  }
  void SessionHandler::setFeatureExplorer() {
    // Make the feature explorer headers
    if (feature_explorer_headers.size() <= 0) {
      feature_explorer_headers.resize(1);
      feature_explorer_headers.setValues({ "metavalue" });
      feature_explorer_checkbox_headers.resize(2);
      feature_explorer_checkbox_headers.setValues({ "plot", "table" });
    }
    const int n_cols = feature_explorer_headers.size();
    const int n_rows = metadatafloatToString.size();
    // Make the feature explorer body
    if (feature_explorer_body.size() <= 0 && n_rows > 0) {
      feature_explorer_body.resize(n_rows, n_cols);
      int col = 0, row = 0;
      for (const auto& metadata : metadatafloatToString) {
        feature_explorer_body(row, col) = metadata.second;
        col = 0;
        ++row;
      }
      feature_explorer_checkbox_body.resize(n_rows, (int)feature_explorer_checkbox_headers.size());
      feature_explorer_checkbox_body.setConstant(true);
      for (int i = 0; i < feature_explorer_checkbox_body.dimension(0); ++i) if (i!=2) feature_explorer_checkbox_body(i, 1) = false; // only calculated_concentration
      feature_explorer_checked_rows.resize(n_rows);
      feature_explorer_checked_rows.setConstant(true);
    }
  }
  void SessionHandler::setSequenceTable(const SequenceHandler & sequence_handler){
    // Make the sequence table headers
    if (sequence_table_headers.size() <= 0) {
      sequence_table_headers.resize(11);
      sequence_table_headers.setValues({
      "inj#", "sample_name", "sample_group_name" , "sequence_segment_name" , "sample_type", 
      "original_filename", "acq_method_name", "inj_volume", "inj_volume_units", "batch_name", // skipping optional members
      "acquisition_date_and_time" });
    }
    const int n_cols = sequence_table_headers.size();
    const int n_rows = sequence_handler.getSequence().size();
    // Make the sequence table body
    if (sequence_table_body.size() <= 0 && n_rows > 0) {
      sequence_table_body.resize(n_rows, n_cols);
      int col = 0, row = 0;
      for (const auto& injection : sequence_handler.getSequence()) {
        sequence_table_body(row, col) = std::to_string(injection.getMetaData().inj_number);
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().sample_name;
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().sample_group_name;
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().sequence_segment_name;
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().getSampleTypeAsString();
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().original_filename;
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().acq_method_name;
        ++col;
        sequence_table_body(row, col) = std::to_string(injection.getMetaData().inj_volume);
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().inj_volume_units;
        ++col;
        sequence_table_body(row, col) = injection.getMetaData().batch_name;
        ++col;
        // Skipping optional members
        sequence_table_body(row, col) = injection.getMetaData().getAcquisitionDateAndTimeAsString();
        col = 0;
        ++row;
      }
    }
  }
  void SessionHandler::setTransitionsTable(const SequenceHandler & sequence_handler)
  {
    // Make the transition table headers
    if (transitions_table_headers.size() <= 0) {
      transitions_table_headers.resize(9);
      transitions_table_headers.setValues({ // NOTE: only showing a subset
      "transition_group","transition_name","RetentionTime","PrecursorMz","ProductMz",
      "LabelType","quantifying_transition","identifying_transition","detecting_transition" });
    }
    const int n_cols = transitions_table_headers.size();
    // Make the transition table body
    if (sequence_handler.getSequence().size() > 0) {
      const auto& targeted_exp = sequence_handler.getSequence().at(0).getRawData().getTargetedExperiment();
      const int n_rows = targeted_exp.getTransitions().size();
      if (transitions_table_body.size() <= 0 && n_rows > 0) {
        transitions_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& transition : targeted_exp.getTransitions()) {
          transitions_table_body(row, col) = transition.getPeptideRef();
          ++col;
          transitions_table_body(row, col) = transition.getNativeID();
          ++col;
          transitions_table_body(row, col) = std::to_string(targeted_exp.getPeptideByRef(transition.getPeptideRef()).getRetentionTime());
          ++col;
          transitions_table_body(row, col) = std::to_string(transition.getPrecursorMZ());
          ++col;
          transitions_table_body(row, col) = std::to_string(transition.getProductMZ());
          ++col;
          //transitions_table_body(row,col) = transition.getProduct().getMetaValue("LabelType");
          transitions_table_body(row, col) = "?";
          ++col;
          transitions_table_body(row, col) = std::to_string(transition.isQuantifyingTransition());
          ++col;
          transitions_table_body(row, col) = std::to_string(transition.isIdentifyingTransition());
          ++col;
          transitions_table_body(row, col) = std::to_string(transition.isDetectingTransition());
          col = 0;
          ++row;
        }
      }
    }
  }
  void SessionHandler::setWorkflowTable(const std::vector<ApplicationHandler::Command>& commands)
  {
    // Make the workflow table headers
    if (workflow_table_headers.size() <= 0) {
      workflow_table_headers.resize(2);
      workflow_table_headers.setValues({ "step", "command" });
    }
    const int n_cols = workflow_table_headers.size();
    const int n_rows = commands.size();
    // Make the workflow table body
    if (workflow_table_body.size() != commands.size()) { // TODO: does not account for case of different commands of the same length!
      workflow_table_body.resize(n_rows, n_cols);
      int col = 0, row = 0;
      for (const auto& command : commands) {
        workflow_table_body(row, col) = std::to_string(row);
        ++col;
        workflow_table_body(row, col) = command.getName();
        col = 0;
        ++row;
      }
    }
  }
  void SessionHandler::setParametersTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequence().size() > 0) {
      // Make the parameters table headers
      if (parameters_table_headers.size() <= 0) {
        parameters_table_headers.resize(4);
        parameters_table_headers.setValues({ "function","name","type","value",/*"default","restrictions","description"*/ });
      }
      const int n_cols = parameters_table_headers.size();
      // Make the parameters table body
      const int n_funcs = sequence_handler.getSequence().at(0).getRawData().getParameters().size();
      if (parameters_table_body.size() <= 0 && n_funcs > 0) {
        int n_rows = 0;
        for (const auto& parameters : sequence_handler.getSequence().at(0).getRawData().getParameters()) {
          n_rows += parameters.second.size();
        }
        parameters_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& parameters : sequence_handler.getSequence().at(0).getRawData().getParameters()) {
          for (const auto& parameter : parameters.second) {
            parameters_table_body(row, col) = parameters.first;
            ++col;
            parameters_table_body(row, col) = parameter.at("name");
            ++col;
            parameters_table_body(row, col) = parameter.at("type");
            ++col;
            parameters_table_body(row, col) = parameter.at("value");
            col = 0;
            ++row;
          }
        }
      }
    }
  }
  void SessionHandler::setQuantMethodTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the quant_method table headers
      if (quant_method_table_headers.size() <= 0) {
        quant_method_table_headers.resize(20);
        quant_method_table_headers.setValues({
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
      const int n_cols = quant_method_table_headers.size();
      // Make the quant_method table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().size() * sequence_handler.getSequenceSegments().size();
      if (quant_method_table_body.size() <= 0 && n_rows > 0) {
        quant_method_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& seq_segment : sequence_handler.getSequenceSegments()) {
          for (const auto& quant_method : seq_segment.getQuantitationMethods()) {
            quant_method_table_body(row, col) = quant_method.getComponentName();
            ++col;
            quant_method_table_body(row, col) = quant_method.getFeatureName();
            ++col;
            quant_method_table_body(row, col) = seq_segment.getSequenceSegmentName();
            ++col;
            quant_method_table_body(row, col) = quant_method.getISName();
            ++col;
            quant_method_table_body(row, col) = quant_method.getConcentrationUnits();
            ++col;
            quant_method_table_body(row, col) = std::to_string(quant_method.getLLOD());
            ++col;
            quant_method_table_body(row, col) = std::to_string(quant_method.getULOD());
            ++col;
            quant_method_table_body(row, col) = std::to_string(quant_method.getLLOQ());
            ++col;
            quant_method_table_body(row, col) = std::to_string(quant_method.getULOQ());
            ++col;
            quant_method_table_body(row, col) = std::to_string(quant_method.getCorrelationCoefficient());
            ++col;
            quant_method_table_body(row, col) = std::to_string(quant_method.getNPoints());
            ++col;
            quant_method_table_body(row, col) = quant_method.getTransformationModel();
            ++col;
            quant_method_table_body(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("slope"));
            ++col;
            quant_method_table_body(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("intercept"));
            ++col;
            quant_method_table_body(row, col) = (std::string)quant_method.getTransformationModelParams().getValue("x_weight");
            ++col;
            quant_method_table_body(row, col) = (std::string)quant_method.getTransformationModelParams().getValue("y_weight");
            ++col;
            quant_method_table_body(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("x_datum_min"));
            ++col;
            quant_method_table_body(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("x_datum_max"));
            ++col;
            quant_method_table_body(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("y_datum_min"));
            ++col;
            quant_method_table_body(row, col) = std::to_string((double)quant_method.getTransformationModelParams().getValue("y_datum_max"));
            col = 0;
            ++row;
          }
        }
      }
    }
  }
  void SessionHandler::setStdsConcsTable(const SequenceHandler & sequence_handler)
  {
    // Make the stds_concs table headers
    if (stds_concs_table_headers.size() <= 0) {
      stds_concs_table_headers.resize(7);
      stds_concs_table_headers.setValues({
        "sample_name", "component_name", "IS_component_name", "actual_concentration",
          "IS_actual_concentration", "concentration_units", "dilution_factor" });
    }
    const int n_cols = stds_concs_table_headers.size();
    // Make the stds_concs table body
    if (sequence_handler.getSequenceSegments().size() > 0) {
      const auto& stand_concs = sequence_handler.getSequenceSegments().at(0).getStandardsConcentrations();
      const int n_rows = stand_concs.size();
      if (stds_concs_table_body.size() <= 0 && n_rows > 0) {
        stds_concs_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& stds_concs : stand_concs) {
          stds_concs_table_body(row, col) = stds_concs.sample_name;
          ++col;
          stds_concs_table_body(row, col) = stds_concs.component_name;
          ++col;
          stds_concs_table_body(row, col) = stds_concs.IS_component_name;
          ++col;
          stds_concs_table_body(row, col) = std::to_string(stds_concs.actual_concentration);
          ++col;
          stds_concs_table_body(row, col) = std::to_string(stds_concs.IS_actual_concentration);
          ++col;
          stds_concs_table_body(row, col) = stds_concs.concentration_units;
          ++col;
          stds_concs_table_body(row, col) = std::to_string(stds_concs.dilution_factor);
          col = 0;
          ++row;
        }
      }
    }
  }
  void SessionHandler::setComponentFiltersTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the comp_filters table headers
      if (comp_filters_table_headers.size() <= 0) {
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        comp_filters_table_headers.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) comp_filters_table_headers(i) = tmp.at(i);
      }
      const int n_cols = comp_filters_table_headers.size();

      // Make the comp_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_qcs.size();
      if (comp_filters_table_body.size() <= 0 && n_rows > 0) {
        comp_filters_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_qcs) {
          comp_filters_table_body(row, col) = comp_qcs.component_name;
          ++col;
          comp_filters_table_body(row, col) = std::to_string(comp_qcs.retention_time_l);
          ++col;
          comp_filters_table_body(row, col) = std::to_string(comp_qcs.retention_time_u);
          ++col;
          comp_filters_table_body(row, col) = std::to_string(comp_qcs.intensity_l);
          ++col;
          comp_filters_table_body(row, col) = std::to_string(comp_qcs.intensity_u);
          ++col;
          comp_filters_table_body(row, col) = std::to_string(comp_qcs.overall_quality_l);
          ++col;
          comp_filters_table_body(row, col) = std::to_string(comp_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_qcs.meta_value_qc) {
            comp_filters_table_body(row, col) = std::to_string(meta_data.second.first);
            ++col;
            comp_filters_table_body(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }
  void SessionHandler::setComponentGroupFiltersTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the comp_group_filters table headers
      if (comp_group_filters_table_headers.size() <= 0) {
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        comp_group_filters_table_headers.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) comp_group_filters_table_headers(i) = tmp.at(i);
      }
      const int n_cols = comp_group_filters_table_headers.size();
      // Make the comp_group_filters table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs.size();
      if (comp_group_filters_table_body.size() <= 0 && n_rows > 0) {
        comp_group_filters_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureFilter().component_group_qcs) {
          comp_group_filters_table_body(row, col) = comp_group_qcs.component_group_name;
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.retention_time_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.retention_time_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.intensity_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.intensity_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.overall_quality_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.overall_quality_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_heavy_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_heavy_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_light_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_light_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_detecting_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_detecting_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_quantifying_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_quantifying_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_identifying_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_identifying_u);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_transitions_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.n_transitions_u);
          ++col;
          comp_group_filters_table_body(row, col) = comp_group_qcs.ion_ratio_pair_name_1;
          ++col;
          comp_group_filters_table_body(row, col) = comp_group_qcs.ion_ratio_pair_name_2;
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.ion_ratio_l);
          ++col;
          comp_group_filters_table_body(row, col) = std::to_string(comp_group_qcs.ion_ratio_u);
          ++col;
          comp_group_filters_table_body(row, col) = comp_group_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_qcs.meta_value_qc) {
            comp_group_filters_table_body(row, col) = std::to_string(meta_data.second.first);
            ++col;
            comp_group_filters_table_body(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }
  void SessionHandler::setComponentQCsTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the comp_qcs table headers
      if (comp_qcs_table_headers.size() <= 0) {
        std::vector<std::string> tmp = { "component_name","retention_time_l","retention_time_u","intensity_l","intensity_u","overall_quality_l","overall_quality_u" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        comp_qcs_table_headers.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) comp_qcs_table_headers(i) = tmp.at(i);
      }
      const int n_cols = comp_qcs_table_headers.size();

      // Make the comp_qcs table body
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs.size();
      if (comp_qcs_table_body.size() <= 0 && n_rows > 0) {
        comp_qcs_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_qcs) {
          comp_qcs_table_body(row, col) = comp_qcs.component_name;
          ++col;
          comp_qcs_table_body(row, col) = std::to_string(comp_qcs.retention_time_l);
          ++col;
          comp_qcs_table_body(row, col) = std::to_string(comp_qcs.retention_time_u);
          ++col;
          comp_qcs_table_body(row, col) = std::to_string(comp_qcs.intensity_l);
          ++col;
          comp_qcs_table_body(row, col) = std::to_string(comp_qcs.intensity_u);
          ++col;
          comp_qcs_table_body(row, col) = std::to_string(comp_qcs.overall_quality_l);
          ++col;
          comp_qcs_table_body(row, col) = std::to_string(comp_qcs.overall_quality_u);
          ++col;
          for (const auto& meta_data : comp_qcs.meta_value_qc) {
            comp_qcs_table_body(row, col) = std::to_string(meta_data.second.first);
            ++col;
            comp_qcs_table_body(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }
  void SessionHandler::setComponentGroupQCsTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequenceSegments().size() > 0) {
      // Make the comp_group_qcs table headers
      if (comp_group_qcs_table_headers.size() <= 0) {
        std::vector<std::string> tmp = { "component_group_name", "retention_time_l", "retention_time_u", "intensity_l", "intensity_u", "overall_quality_l", "overall_quality_u",
          "n_heavy_l", "n_heavy_u", "n_light_l", "n_light_u", "n_detecting_l", "n_detecting_u", "n_quantifying_l", "n_quantifying_u", "n_identifying_l", "n_identifying_u", "n_transitions_l", "n_transitions_u",
          "ion_ratio_pair_name_1", "ion_ratio_pair_name_2", "ion_ratio_l", "ion_ratio_u", "ion_ratio_feature_name" };
        for (const auto& meta_data : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.at(0).meta_value_qc) {
          tmp.push_back("metaValue_" + meta_data.first + "_l");
          tmp.push_back("metaValue_" + meta_data.first + "_u");
        }
        comp_group_qcs_table_headers.resize((int)tmp.size());
        for (int i = 0; i < tmp.size(); ++i) comp_group_qcs_table_headers(i) = tmp.at(i);
      }
      const int n_cols = comp_group_qcs_table_headers.size();
      const int n_rows = sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs.size();
      // Make the comp_group_qcs table body
      if (comp_group_qcs_table_body.size() <= 0 && n_rows > 0) {
        comp_group_qcs_table_body.resize(n_rows, n_cols);
        int col = 0, row = 0;
        for (const auto& comp_group_qcs : sequence_handler.getSequenceSegments().at(0).getFeatureQC().component_group_qcs) {
          comp_group_qcs_table_body(row, col) = comp_group_qcs.component_group_name;
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.retention_time_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.retention_time_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.intensity_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.intensity_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.overall_quality_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.overall_quality_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_heavy_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_heavy_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_light_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_light_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_detecting_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_detecting_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_quantifying_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_quantifying_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_identifying_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_identifying_u);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_transitions_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.n_transitions_u);
          ++col;
          comp_group_qcs_table_body(row, col) = comp_group_qcs.ion_ratio_pair_name_1;
          ++col;
          comp_group_qcs_table_body(row, col) = comp_group_qcs.ion_ratio_pair_name_2;
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.ion_ratio_l);
          ++col;
          comp_group_qcs_table_body(row, col) = std::to_string(comp_group_qcs.ion_ratio_u);
          ++col;
          comp_group_qcs_table_body(row, col) = comp_group_qcs.ion_ratio_feature_name;
          ++col;
          for (const auto& meta_data : comp_group_qcs.meta_value_qc) {
            comp_group_qcs_table_body(row, col) = std::to_string(meta_data.second.first);
            ++col;
            comp_group_qcs_table_body(row, col) = std::to_string(meta_data.second.second);
            ++col;
          }
          col = 0;
          ++row;
        }
      }
    }
  }
  void SessionHandler::setFeatureTable(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequence().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0) {
      // Make the feature table headers and body
      if (feature_table_body.size() <= 0) {
        std::vector<std::string> meta_data; // TODO: options for the user to select what meta_data
        for (const std::pair<FeatureMetadata, std::string>& p : metadatafloatToString) meta_data.push_back(p.second);
        std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
        for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
        std::vector<std::vector<std::string>> table;
        std::vector<std::string> headers;
        SequenceParser::makeDataTableFromMetaValue(sequence_handler, table, headers, meta_data, sample_types);
        const int n_cols = feature_table_headers.size();
        const int n_rows = feature_table_body.size();
        feature_table_headers.resize(n_cols);
        feature_table_body.resize(n_rows, n_cols);
        for (int row = 0; row < n_rows; ++row) {
          for (int col = 0; col < n_cols; ++col) {
            if (row == 0) feature_table_headers(col) = headers.at(col);
            feature_table_body(row, col) = table.at(row).at(col);
          }
        }
      }
    }
  }
  void SessionHandler::setFeatureMatrix(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequence().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0) {
      // Make the feature_pivot table headers and body
      if (feat_value_data.size() <= 0) {
        // Create the initial data vectors and matrices from the feature map history
        std::vector<std::string> meta_data; // TODO: options for the user to select what meta_data
        for (const std::pair<FeatureMetadata, std::string>& p : metadatafloatToString) meta_data.push_back(p.second);
        //std::vector<std::string> meta_data = { "calculated_concentration" };
        std::set<SampleType> sample_types; // TODO: options for the user to select what sample_types
        for (const std::pair<SampleType, std::string>& p : sampleTypeToString) sample_types.insert(p.first);
        Eigen::Tensor<std::string, 2> rows_out;
        SequenceParser::makeDataMatrixFromMetaValue(sequence_handler, feat_value_data, feat_heatmap_col_labels, rows_out, meta_data, sample_types);
        setFeatureLinePlot();
        setFeatureHeatMap();
        // update the pivot table headers with the columns for the pivot table/heatmap rows
        feature_pivot_table_headers.resize((int)feat_heatmap_col_labels.size() + 3);
        feature_pivot_table_headers(0) = "feature_name";
        feature_pivot_table_headers(1) = "component_group_name";
        feature_pivot_table_headers(2) = "component_name";
        feature_pivot_table_headers.slice(Eigen::array<Eigen::Index, 1>({ 3 }), Eigen::array<Eigen::Index, 1>({ feat_heatmap_col_labels.size() })) = feat_heatmap_col_labels;
        const int n_cols = feature_pivot_table_headers.size();
        const int n_rows = feat_value_data.dimension(0);
        // allocate space for the pivot table body and heatmap row labels
        feat_heatmap_row_labels.resize(n_rows);
        feature_pivot_table_body.resize(n_rows, n_cols);
        // assign the pivot table body data and heatmap row labels
        int col = 0;
        for (int row = 0; row < n_rows; ++row) {
          feat_heatmap_row_labels(row) = rows_out(row, 0) + "::" + rows_out(row, 2);
          for (int j = 0; j < rows_out.dimension(1); ++j) {
            feature_pivot_table_body(row, col) = rows_out(row, j);
            ++col;
          }
          for (int j = 0; j < feat_value_data.dimension(1); ++j) {
            feature_pivot_table_body(row, col) = std::to_string(feat_value_data(row, j));
            ++col;
          }
          col = 0;
        }
      }
    }
  }
  void SessionHandler::setChromatogramScatterPlot(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequence().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getFeatureMapHistory().size() > 0 &&
      sequence_handler.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size() > 0) {
      // TODO: filter by transition name (i.e., getNativeID() == component_name)
      // TODO: filter by sample or injection name
      if (chrom_time_data.size() <= 0) { // TODO: better check for updates
      // Set the axes titles and min/max defaults
        chrom_x_axis_title = "Time (sec)";
        chrom_y_axis_title = "Intensity (au)";
        chrom_time_min = 1e6;
        chrom_time_max = 0;
        chrom_intensity_min = 1e6;
        chrom_intensity_max = 0;
        for (const auto& injection : sequence_handler.getSequence()) {
          // Extract out the raw data for plotting
          for (const auto& chromatogram : injection.getRawData().getChromatogramMap().getChromatograms()) {
            std::vector<float> x_data, y_data;
            for (const auto& point : chromatogram) {
              x_data.push_back(point.getRT());
              y_data.push_back(point.getIntensity());
              chrom_time_min = std::min((float)point.getRT(), chrom_time_min);
              chrom_intensity_min = std::min((float)point.getIntensity(), chrom_intensity_min);
              chrom_time_max = std::max((float)point.getRT(), chrom_time_max);
              chrom_intensity_max = std::max((float)point.getIntensity(), chrom_intensity_max);
            }
            chrom_time_data.push_back(x_data);
            chrom_intensity_data.push_back(y_data);
            chrom_series_names.push_back(injection.getMetaData().getSampleName() + "::" + chromatogram.getNativeID());
          }
          // Extract out the best left/right for plotting
          for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
            for (const auto& subordinate : feature.getSubordinates()) {
              if (subordinate.getMetaValue("used_") == "true") {
                if (subordinate.metaValueExists("leftWidth") && subordinate.metaValueExists("rightWidth")) {
                  std::vector<float> x_data, y_data;
                  x_data.push_back(subordinate.getMetaValue("leftWidth"));
                  y_data.push_back(0); // TODO: extract out chrom peak intensity
                  x_data.push_back(subordinate.getMetaValue("rightWidth"));
                  y_data.push_back(0); // TODO: extract out chrom peak intensity
                  chrom_time_data.push_back(x_data);
                  chrom_intensity_data.push_back(y_data);
                  chrom_series_names.push_back(injection.getMetaData().getSampleName() + "::" + (std::string)subordinate.getMetaValue("native_id") + "::" + (std::string)subordinate.getMetaValue("timestamp_"));
                }
              }
            }
          }
          // Extract out the smoothed points for plotting
          for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
            for (const auto& subordinate : feature.getSubordinates()) {
              if (subordinate.getMetaValue("used_") == "true") {
                std::vector<float> x_data, y_data;
                for (const auto& point : subordinate.getConvexHull().getHullPoints()) {
                  x_data.push_back(point.getX());
                  y_data.push_back(point.getY());
                  chrom_time_min = std::min((float)point.getX(), chrom_time_min);
                  chrom_intensity_min = std::min((float)point.getY(), chrom_intensity_min);
                  chrom_time_max = std::max((float)point.getX(), chrom_time_max);
                  chrom_intensity_max = std::max((float)point.getY(), chrom_intensity_max);
                }
                chrom_time_data.push_back(x_data);
                chrom_intensity_data.push_back(y_data);
                chrom_series_names.push_back(injection.getMetaData().getSampleName() + "::" + (std::string)subordinate.getMetaValue("native_id") + "::" + (std::string)subordinate.getMetaValue("timestamp_"));
              }
            }
          }
        }
      }
    }
  }
  void SessionHandler::setFeatureLinePlot()
  {
    // Set the axes titles and min/max defaults
    feat_line_x_axis_title = "Inj#";
    std::string feat_line_y_axis_title = "metadata (au)";
    feat_line_sample_min = 1e6; 
    feat_line_sample_max = 0; 
    feat_value_min = 1e6; 
    feat_value_max = 0;
    // make the injection index for the line plot
    const int n_samples = feat_heatmap_col_labels.size();
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
  void SessionHandler::setFeatureHeatMap()
  {
    // Set the axes titles
    feat_heatmap_x_axis_title = "Sample name";
    feat_heatmap_y_axis_title = "Component name";
    // assign the heatmap data
    feat_heatmap_data.resize(feat_value_data.dimensions());
    feat_heatmap_data = feat_value_data.swap_layout().shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
  }
  void SessionHandler::setCalibratorsScatterLinePlot(const SequenceHandler & sequence_handler)
  {
    if (sequence_handler.getSequenceSegments().size() > 0 &&
      sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().size() > 0 &&
      sequence_handler.getSequenceSegments().at(0).getComponentsToConcentrations().size() > 0 &&
      sequence_handler.getSequenceSegments().at(0).getStandardsConcentrations().size() > 0) {
      if (calibrators_conc_fit_data.size() <= 0 && calibrators_conc_raw_data.size() <= 0) {
        // Update the axis titles
        calibrators_x_axis_title = "Concentration (" + sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().at(0).getConcentrationUnits() + ")";
        calibrators_y_axis_title = sequence_handler.getSequenceSegments().at(0).getQuantitationMethods().at(0).getFeatureName() + " (au)";
        calibrators_conc_min = 1e6;
        calibrators_conc_max = 0;
        calibrators_feature_min = 1e6;
        calibrators_feature_max = 0;
        // TODO: filtering by the transition (i.e., getNativeID, getComponentName)
        for (const auto& sequence_segment : sequence_handler.getSequenceSegments()) {
          // Extract out raw data used to make the calibrators found in `StandardsConcentrations`
          std::map<std::string, std::pair<std::vector<float>, std::vector<std::string>>> stand_concs_map; // map of x_data and sample_name for a component
          for (const auto& stand_concs : sequence_segment.getStandardsConcentrations()) {
            // Skip components that have not been fitted with a calibration curve
            if (sequence_segment.getComponentsToConcentrations().count(stand_concs.component_name) > 0 &&
              sequence_segment.getComponentsToConcentrations().at(stand_concs.component_name).size() > 0) { // TODO: filter out components that have not been fitted
              const float x_datum = float(stand_concs.actual_concentration / stand_concs.IS_actual_concentration / stand_concs.dilution_factor);
              auto found = stand_concs_map.emplace(stand_concs.component_name,
                std::make_pair(std::vector<float>({ x_datum }),
                  std::vector<std::string>({ stand_concs.sample_name })));
              if (!found.second) {
                stand_concs_map.at(stand_concs.component_name).first.push_back(x_datum);
                stand_concs_map.at(stand_concs.component_name).second.push_back(stand_concs.sample_name);
              }
            }
          }
          // Make the line of best fit using the `QuantitationMethods`
          for (const auto& quant_method : sequence_segment.getQuantitationMethods()) {
            // Skip components that have not been fitted with a calibration curve
            if (sequence_segment.getComponentsToConcentrations().count(quant_method.getComponentName()) > 0 &&
              sequence_segment.getComponentsToConcentrations().at(quant_method.getComponentName()).size() > 0 &&
              (double)quant_method.getTransformationModelParams().getValue("slope") != 1.0) { // TODO: filter out components that have not been fitted
              // Make the line of best fit using the `QuantitationMethods
              std::vector<float> y_fit_data;
              for (const auto& ratio : stand_concs_map.at(quant_method.getComponentName()).first) {
                // TODO: encapsulate in its own method e.g. sequenceSegmentProcessor
                // TODO: check that the calibration actually found a best fit (and set to all 0 if not)
                // calculate the absolute concentration
                OpenMS::TransformationModel::DataPoints data;
                OpenMS::TransformationDescription tmd(data);
                tmd.fitModel(quant_method.getTransformationModel(), quant_method.getTransformationModelParams());
                float calculated_feature_ratio = tmd.apply(ratio);
                // check for less than zero
                if (calculated_feature_ratio < 0.0) calculated_feature_ratio = 0.0;
                y_fit_data.push_back(calculated_feature_ratio);
                calibrators_conc_min = std::min(ratio, calibrators_conc_min);
                calibrators_conc_max = std::max(ratio, calibrators_conc_max);
                calibrators_feature_min = std::min(calculated_feature_ratio, calibrators_feature_min);
                calibrators_feature_max = std::max(calculated_feature_ratio, calibrators_feature_max);
              }
              calibrators_conc_fit_data.push_back(stand_concs_map.at(quant_method.getComponentName()).first);
              calibrators_feature_fit_data.push_back(y_fit_data);
              // Extract out the points used to make the line of best fit in `ComponentsToConcentrations`
              std::vector<float> x_raw_data, y_raw_data;
              OpenMS::AbsoluteQuantitation absQuant;
              for (const auto& point : sequence_segment.getComponentsToConcentrations().at(quant_method.getComponentName())) {
                x_raw_data.push_back(float(point.actual_concentration / point.IS_actual_concentration / point.dilution_factor));
                float y_datum = absQuant.calculateRatio(point.feature, point.IS_feature, quant_method.getFeatureName());
                y_raw_data.push_back(y_datum);
                calibrators_feature_min = std::min(y_datum, calibrators_feature_min);
                calibrators_feature_max = std::max(y_datum, calibrators_feature_max);
              }
              calibrators_conc_raw_data.push_back(x_raw_data);
              calibrators_feature_raw_data.push_back(y_raw_data);
              calibrators_series_names.push_back(quant_method.getComponentName());
            }
          }
        }
      }
    }
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getInjectionExplorerHeader()
  {
    if (sequence_table_headers.size())
      return sequence_table_headers.slice(Eigen::array<Eigen::Index, 1>({ 0 }), Eigen::array<Eigen::Index, 1>({ 2 }));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 2> SessionHandler::getInjectionExplorerBody()
  {
    if (sequence_table_body.size())
      return sequence_table_body.slice(Eigen::array<Eigen::Index, 2>({ 0,0 }), Eigen::array<Eigen::Index, 2>({ sequence_table_body.dimension(0), 2 }));
    else
      return Eigen::Tensor<std::string, 2>();
  }
  Eigen::Tensor<std::string, 1> SessionHandler::getTransitionExplorerHeader()
  {
    if (transitions_table_headers.size())
      return transitions_table_headers.slice(Eigen::array<Eigen::Index, 1>({ 0 }), Eigen::array<Eigen::Index, 1>({ 2 }));
    else
      return Eigen::Tensor<std::string, 1>();
  }
  Eigen::Tensor<std::string, 2> SessionHandler::getTransitionExplorerBody()
  {
    if (transitions_table_body.size())
      return transitions_table_body.slice(Eigen::array<Eigen::Index, 2>({ 0,0 }), Eigen::array<Eigen::Index, 2>({ transitions_table_body.dimension(0), 2 }));
    else
      return Eigen::Tensor<std::string, 2>();
  }
}
