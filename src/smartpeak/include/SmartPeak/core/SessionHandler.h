#pragma once

#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <unsupported/Eigen/CXX11/Tensor>

namespace SmartPeak
{
  class SessionHandler {
  public:
    /*
    @brief Sets minimal amount of data needed for injection, transition, and feature filters.
      The method sets the SequenceTable, TransitionsTable, InjectionExplorer, TransitionExplorer, and FeatureExplorer

    @param[in] sequence_handler

    @returns true if all rows/columns were added and false if rows/columns were omitted due to performance
    */
    void setMinimalDataAndFilters(const SequenceHandler& sequence_handler);
    void setInjectionExplorer(); ///< set the InjectionExplorer-specific data
    void setTransitionExplorer(); ///< set the TransitionExplorer-specific data
    void setFeatureExplorer(); ///< set the FeatureExplorer-specific data
    void setSpectrumExplorer(); ///< set the SpectrumExplorer-specific data
    void setSequenceTable(const SequenceHandler& sequence_handler); ///< set the SequenceTable-specific data
    void setTransitionsTable(const SequenceHandler& sequence_handler); ///< set the TransitionsTable-specific data
    void setSpectrumTable(const SequenceHandler& sequence_handler); ///< set the SpectrumTable-specific data
    void setWorkflowTable(const std::vector<ApplicationHandler::Command>& commands); ///< set the WorkflowTable-specific data
    void setParametersTable(const SequenceHandler& sequence_handler); ///< set the ParametersTable-specific data
    void setQuantMethodTable(const SequenceHandler& sequence_handler); ///< set the QuantMethodTable-specific data
    void setStdsConcsTable(const SequenceHandler& sequence_handler); ///< set the StdsConcsTable-specific data
    void setComponentFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentFiltersTable-specific data
    void setComponentGroupFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupFiltersTable-specific data
    void setComponentQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentQCsTable-specific data
    void setComponentGroupQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupQCsTable-specific data
    void setComponentRSDFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentFiltersTable-specific data
    void setComponentGroupRSDFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupFiltersTable-specific data
    void setComponentRSDQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentQCsTable-specific data
    void setComponentGroupRSDQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupQCsTable-specific data
    void setComponentBackgroundFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentFiltersTable-specific data
    void setComponentGroupBackgroundFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupFiltersTable-specific data
    void setComponentBackgroundQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentQCsTable-specific data
    void setComponentGroupBackgroundQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupQCsTable-specific data
    void setComponentRSDEstimationsTable(const SequenceHandler& sequence_handler); ///< set the ComponentEstimationsTable-specific data
    void setComponentGroupRSDEstimationsTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupEstimationsTable-specific data
    void setComponentBackgroundEstimationsTable(const SequenceHandler& sequence_handler); ///< set the ComponentEstimationsTable-specific data
    void setComponentGroupBackgroundEstimationsTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupEstimationsTable-specific data
    /*
    @brief Sets the feature table

    @param[in] sequence_handler

    @returns true if all rows/columns were added and false if rows/columns were omitted due to performance
    */
    bool setFeatureTable(const SequenceHandler& sequence_handler);
    /*
    @brief Sets the Feature matrix data used for the matrix table, line plots, and heatmap

    @param[in] sequence_handler

    @returns true if all points were added and false if points were omitted due to performance
    */
    void setFeatureMatrix(const SequenceHandler& sequence_handler);
    /*
    @brief Sets the chromatogram data

    @param[in] sequence_handler

    @returns true if all points were added and false if points were omitted due to performance
    */
    bool setChromatogramScatterPlot(const SequenceHandler& sequence_handler);
    /*
    @brief Sets the spectrum data

    @param[in] sequence_handler

    @returns true if all points were added and false if points were omitted due to performance
    */
    bool setSpectrumScatterPlot(const SequenceHandler& sequence_handler);
    void setFeatureLinePlot();
    void setFeatureHeatMap();
    /*
    @brief Sets the data used for rendering the calibrators

    @param[in] sequence_handler

    @returns true if all points were added and false if points were omitted due to performance
    */
    bool setCalibratorsScatterLinePlot(const SequenceHandler& sequence_handler);

    Eigen::Tensor<std::string, 1> getInjectionExplorerHeader();
    Eigen::Tensor<std::string, 2> getInjectionExplorerBody();
    Eigen::Tensor<std::string, 1> getTransitionExplorerHeader();
    Eigen::Tensor<std::string, 2> getTransitionExplorerBody();
    Eigen::Tensor<std::string, 1> getSpectrumExplorerHeader();
    Eigen::Tensor<std::string, 2> getSpectrumExplorerBody();

    Eigen::Tensor<bool, 1> getSequenceTableFilters();
    Eigen::Tensor<bool, 1> getTransitionsTableFilters();
    Eigen::Tensor<bool, 1> getSpectrumTableFilters();
    Eigen::Tensor<bool, 1> getQuantMethodsTableFilters();
    Eigen::Tensor<bool, 1> getComponentFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentRSDFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentRSDQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupRSDFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupRSDQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentBackgroundFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentBackgroundQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupBackgroundFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupBackgroundQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentRSDEstimationsTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupRSDEstimationsTableFilters();
    Eigen::Tensor<bool, 1> getComponentBackgroundEstimationsTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupBackgroundEstimationsTableFilters();

    std::set<std::string> getSelectInjectionNamesWorkflow(const SequenceHandler& sequence_handler);
    std::set<std::string> getSelectSequenceSegmentNamesWorkflow(const SequenceHandler& sequence_handler);
    std::set<std::string> getSelectSampleGroupNamesWorkflow(const SequenceHandler& sequence_handler);
    Eigen::Tensor<std::string, 1> getSelectSampleNamesTable(); // Should be injection name?
    Eigen::Tensor<std::string, 1> getSelectSampleNamesPlot(); // Should be injection name?
    Eigen::Tensor<std::string, 1> getSelectTransitionsTable();
    Eigen::Tensor<std::string, 1> getSelectTransitionGroupsTable();
    Eigen::Tensor<std::string, 1> getSelectTransitionsPlot();
    Eigen::Tensor<std::string, 1> getSelectTransitionGroupsPlot();
    Eigen::Tensor<std::string, 1> getSelectFeatureMetaValuesTable();
    Eigen::Tensor<std::string, 1> getSelectFeatureMetaValuesPlot();
    Eigen::Tensor<std::string, 1> getSelectSpectrumPlot();

    int getNSelectedSampleNamesTable();
    int getNSelectedSampleNamesPlot();
    int getNSelectedTransitionsTable();
    int getNSelectedTransitionsPlot();
    int getNSelectedFeatureMetaValuesTable();
    int getNSelectedFeatureMetaValuesPlot();

    // data for the injection explorer
    Eigen::Tensor<std::string, 1> injection_explorer_checkbox_headers;
    Eigen::Tensor<bool, 2> injection_explorer_checkbox_body;
    Eigen::Tensor<bool, 1> injection_explorer_checked_rows;
    // data for the transition explorer
    Eigen::Tensor<std::string, 1> transition_explorer_checkbox_headers;
    Eigen::Tensor<bool, 2> transition_explorer_checkbox_body;
    Eigen::Tensor<bool, 1> transition_explorer_checked_rows;
    // data for the spectrum explorer
    Eigen::Tensor<std::string, 1> spectrum_explorer_checkbox_headers;
    Eigen::Tensor<bool, 2> spectrum_explorer_checkbox_body;
    Eigen::Tensor<bool, 1> spectrum_explorer_checked_rows;
    // data for the feature explorer
    Eigen::Tensor<std::string, 1> feature_explorer_headers; // feature_metavalue_name
    Eigen::Tensor<std::string, 1> feature_explorer_checkbox_headers;
    Eigen::Tensor<std::string, 2> feature_explorer_body;
    Eigen::Tensor<bool, 2> feature_explorer_checkbox_body;
    Eigen::Tensor<bool, 1> feature_explorer_checked_rows;
    // data for the sequence table
    Eigen::Tensor<std::string, 1> sequence_table_headers;
    Eigen::Tensor<std::string, 2> sequence_table_body;
    // data for the transitions table
    Eigen::Tensor<std::string, 1> transitions_table_headers;
    Eigen::Tensor<std::string, 2> transitions_table_body;
    // data for the spectrum table
    Eigen::Tensor<std::string, 1> spectrum_table_headers;
    Eigen::Tensor<std::string, 2> spectrum_table_body;
    // data for the workflow table
    Eigen::Tensor<std::string, 1> workflow_table_headers;
    Eigen::Tensor<std::string, 2> workflow_table_body;
    // data for the parameters table
    Eigen::Tensor<std::string, 1> parameters_table_headers;
    Eigen::Tensor<std::string, 2> parameters_table_body;
    // data for the quant_method table
    Eigen::Tensor<std::string, 1> quant_method_table_headers;
    Eigen::Tensor<std::string, 2> quant_method_table_body;
    // data for the stds_concs table
    Eigen::Tensor<std::string, 1> stds_concs_table_headers;
    Eigen::Tensor<std::string, 2> stds_concs_table_body;
    // data for the comp_filters table
    Eigen::Tensor<std::string, 1> comp_filters_table_headers;
    Eigen::Tensor<std::string, 2> comp_filters_table_body;
    // data for the comp_group_filters table
    Eigen::Tensor<std::string, 1> comp_group_filters_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_filters_table_body;
    // data for the comp_qcs table
    Eigen::Tensor<std::string, 1> comp_qcs_table_headers;
    Eigen::Tensor<std::string, 2> comp_qcs_table_body;
    // data for the comp_group_qcs table
    Eigen::Tensor<std::string, 1> comp_group_qcs_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_qcs_table_body;
    // data for the comp_rsd_filters table
    Eigen::Tensor<std::string, 1> comp_rsd_filters_table_headers;
    Eigen::Tensor<std::string, 2> comp_rsd_filters_table_body;
    // data for the comp_group_rsd_filters table
    Eigen::Tensor<std::string, 1> comp_group_rsd_filters_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_rsd_filters_table_body;
    // data for the comp_rsd_qcs table
    Eigen::Tensor<std::string, 1> comp_rsd_qcs_table_headers;
    Eigen::Tensor<std::string, 2> comp_rsd_qcs_table_body;
    // data for the comp_group_rsd_qcs table
    Eigen::Tensor<std::string, 1> comp_group_rsd_qcs_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_rsd_qcs_table_body;
    // data for the comp_background_filters table
    Eigen::Tensor<std::string, 1> comp_background_filters_table_headers;
    Eigen::Tensor<std::string, 2> comp_background_filters_table_body;
    // data for the comp_group_background_filters table
    Eigen::Tensor<std::string, 1> comp_group_background_filters_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_background_filters_table_body;
    // data for the comp_background_qcs table
    Eigen::Tensor<std::string, 1> comp_background_qcs_table_headers;
    Eigen::Tensor<std::string, 2> comp_background_qcs_table_body;
    // data for the comp_group_background_qcs table
    Eigen::Tensor<std::string, 1> comp_group_background_qcs_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_background_qcs_table_body;
    // data for the comp_rsd_estimations table
    Eigen::Tensor<std::string, 1> comp_rsd_estimations_table_headers;
    Eigen::Tensor<std::string, 2> comp_rsd_estimations_table_body;
    // data for the comp_group_rsd_estimations table
    Eigen::Tensor<std::string, 1> comp_group_rsd_estimations_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_rsd_estimations_table_body;
    // data for the comp_background_estimations table
    Eigen::Tensor<std::string, 1> comp_background_estimations_table_headers;
    Eigen::Tensor<std::string, 2> comp_background_estimations_table_body;
    // data for the comp_group_background_estimations table
    Eigen::Tensor<std::string, 1> comp_group_background_estimations_table_headers;
    Eigen::Tensor<std::string, 2> comp_group_background_estimations_table_body;
    // data for the feature table
    Eigen::Tensor<std::string, 1> feature_table_headers;
    Eigen::Tensor<std::string, 2> feature_table_body;
    // data for the feature_pivot table
    Eigen::Tensor<std::string, 1> feature_pivot_table_headers;
    Eigen::Tensor<std::string, 2> feature_pivot_table_rows;
    Eigen::Tensor<std::string, 2> feature_pivot_table_body;
    // data for the chromatogram scatter plot
    std::vector<std::vector<float>> chrom_time_hull_data, chrom_intensity_hull_data;
    std::vector<std::vector<float>> chrom_time_raw_data, chrom_intensity_raw_data;
    std::vector<std::string> chrom_series_hull_names,chrom_series_raw_names;
    std::string chrom_x_axis_title;
    std::string chrom_y_axis_title;
    float chrom_time_min, chrom_time_max, chrom_intensity_min, chrom_intensity_max;
    std::pair<float, float> chrom_time_range = std::make_pair(0, 1800);
    // data for the spectrum scatter plot
    std::vector<std::vector<float>> spec_mz_hull_data, spec_intensity_hull_data;
    std::vector<std::vector<float>> spec_mz_raw_data, spec_intensity_raw_data;
    std::vector<std::string> spec_series_hull_names, spec_series_raw_names;
    std::string spec_x_axis_title;
    std::string spec_y_axis_title;
    float spec_mz_min, spec_mz_max, spec_intensity_min, spec_intensity_max;
    std::pair<float, float> spec_mz_range = std::make_pair(0, 2000);
    // data for the feature line plot
    Eigen::Tensor<float, 2> feat_sample_data, feat_value_data;
    Eigen::Tensor<std::string, 1> feat_line_series_names;
    std::string feat_line_x_axis_title;
    std::string feat_line_y_axis_title;
    float feat_line_sample_min, feat_line_sample_max, feat_value_min, feat_value_max;
    // data for the feature heatmap (rows/colum labels are derived from feature pivot table)
    Eigen::Tensor<float, 2, Eigen::RowMajor> feat_heatmap_data; // same as feat_value_data but rowMajor
    Eigen::Tensor<std::string, 1> feat_heatmap_row_labels, feat_heatmap_col_labels;
    std::string feat_heatmap_x_axis_title;
    std::string feat_heatmap_y_axis_title;
    // data for the calibrators scatter/line plot
    std::vector<std::vector<float>> calibrators_conc_raw_data, calibrators_feature_raw_data;
    std::vector<std::vector<float>> calibrators_conc_fit_data, calibrators_feature_fit_data;
    std::vector<std::string> calibrators_series_names;
    std::string calibrators_x_axis_title;
    std::string calibrators_y_axis_title;
    float calibrators_conc_min , calibrators_conc_max, calibrators_feature_min, calibrators_feature_max;
  private:
    std::set<std::string> chrom_series_hull_names_;
    std::set<std::string> spec_series_hull_names_;
    int feature_table_unique_samples_transitions_ = 0; // used to decide when to update the feature table data
    int feature_matrix_unique_transitions_ = 0; // used to decide when to update the feature matrix data
  };
}
