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
    void setInjectionExplorer(); ///< set the InjectionExplorer-speciic data
    void setTransitionExplorer(); ///< set the TransitionExplorer-speciic data
    void setFeatureExplorer(); ///< set the FeatureExplorer-speciic data
    void setSequenceTable(const SequenceHandler& sequence_handler); ///< set the SequenceTable-speciic data
    void setTransitionsTable(const SequenceHandler& sequence_handler); ///< set the SequenceTable-speciic data
    void setWorkflowTable(const std::vector<ApplicationHandler::Command>& commands); ///< set the WorkflowTable-speciic data
    void setParametersTable(const SequenceHandler& sequence_handler); ///< set the ParametersTable-speciic data
    void setQuantMethodTable(const SequenceHandler& sequence_handler); ///< set the QuantMethodTable-speciic data
    void setStdsConcsTable(const SequenceHandler& sequence_handler); ///< set the StdsConcsTable-speciic data
    void setComponentFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentFiltersTable-speciic data
    void setComponentGroupFiltersTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupFiltersTable-speciic data
    void setComponentQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentQCsTable-speciic data
    void setComponentGroupQCsTable(const SequenceHandler& sequence_handler); ///< set the ComponentGroupQCsTable-speciic data
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

    Eigen::Tensor<bool, 1> getSequenceTableFilters();
    Eigen::Tensor<bool, 1> getTransitionsTableFilters();
    Eigen::Tensor<bool, 1> getQuantMethodsTableFilters();
    Eigen::Tensor<bool, 1> getComponentFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentQCsTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupFiltersTableFilters();
    Eigen::Tensor<bool, 1> getComponentGroupQCsTableFilters();

    std::set<std::string> getSelectInjectionNamesWorkflow(const SequenceHandler& sequence_handler);
    Eigen::Tensor<std::string, 1> getSelectSampleNamesTable(); // Should be injection name?
    Eigen::Tensor<std::string, 1> getSelectSampleNamesPlot(); // Should be injection name?
    Eigen::Tensor<std::string, 1> getSelectTransitionsTable();
    Eigen::Tensor<std::string, 1> getSelectTransitionGroupsTable();
    Eigen::Tensor<std::string, 1> getSelectTransitionsPlot();
    Eigen::Tensor<std::string, 1> getSelectFeatureMetaValuesTable();
    Eigen::Tensor<std::string, 1> getSelectFeatureMetaValuesPlot();

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
  };
}
