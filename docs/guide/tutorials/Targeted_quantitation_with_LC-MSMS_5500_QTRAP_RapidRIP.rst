Targeted quantitation using LC-MS/MS-SRM acquisition
----------------------------------------------------

.. image:: ../../images/MassSpecSchemas-SRM.png

This tutorial walks you through the workflow for analyzing targeted single reaction monitoring (SRM) data
starting from input file generation, to processing the data in SmartPeak, 
to reviewing the data in SmartPeak, to reporting the results.

Objectives
~~~~~~~~~~

#. Obtaining the SOP for the workflow.
#. Choosing a data set for demonstrating the workflow.
#. Creating an optimized SmartPeak input templates for running the workflow.

The Workflows include
~~~~~~~~~~~~~~~~~~~~~

#. Calculating the calibration curves to generate quantitation methods for each component using Standard samples
#. Processing Unknown samples using the quantitation methods

Notes
~~~~~

The algorithm parameters used in the following workflows have been highly tuned for feature detection using the Sciex 5500 QTRAP and 6500+ systems with Shimadzu and Agilent HPLC systems.  
With that said, we have found the algorithm parameters to generalize well to most liquid chromatography coupled to mass spectrometry systems.

Steps
~~~~~

The tutorial includes the following steps :

#. Setting up the input files

The data set used can be found in 
`LCMS_SRM_Standards <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/LCMS_MRM_Standards>`_, 
`LCMS_SRM_QCs <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/LCMS_MRM_QCs>`_, and 
`LCMS_SRM_Unknowns <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/LCMS_MRM_Unknowns>`_ 
for the LC-MS/MS-SRM Standards, QCs, and Unknowns, respectively.

#. Defining the workflow in SmartPeak

For LC-MS/MS-SRM Standards analysis, the following steps are saved 
into the ``workflow.csv`` file. Alternatively, steps can be replaced, 
added or deleted direclty from SmartPeakGUI. 
A detailed explanation of each command step
can be found in :ref:`Workflow Commands`.

	.. list-table:: workflow_LCMSSRM_Standards.csv
	  :header-rows: 1

	  * - workflow_step
	  * - LOAD_RAW_DATA
	  * - MAP_CHROMATOGRAMS
	  * - PICK_MRM_FEATURES
	  * - CHECK_FEATURES
	  * - SELECT_FEATURES
	  * - CALCULATE_CALIBRATION
	  * - STORE_QUANTITATION_METHODS
	  * - QUANTIFY_FEATURES
	  * - STORE_FEATURES

	The calibration curve for each transition's quantitation method can be inspected after all workflow steps have been run, to do so please
	click on view and then "Calibrators". From the transition tab select Antranilicacid and Indole
	as ``transition_group`` to plot their concentration curves within the given concentration range as
	shown below:

	.. image:: ../../images/hplc_uv_standards_calibration_curve.png

	To inspect the features for the selected transition groups, select "Features (line)" from the view menu
	then open the features tab (can be opened from the view menu as well) to select the "asymetry_factors" and "logSN"
	in the plot column. The line plot illistrates the value for each transition group and feature as shown below:

	.. image:: ../../images/hplc_uv_standards_features_line.png

	The features can also be plotted as a heatmap, under "view" select "Features (heatmap)" then select the "left_width"
	feature to display transition groups as a heatmap and compare the values from the same injection as shown below:

	.. image:: ../../images/hplc_uv_standards_features_heatmap.png

	The workflow step ``STORE_QUANTITATION_METHODS`` writes the calibration model for each transition, an excerpt can be seen below:

	.. table:: Generated sequence1_quantitationMethods.csv
		:widths: auto

		=============== =================== ============= =================== ==== ==== ==== ==== ======================= ======== ==================== =================================== ====================================== ====================================== =================================== ====================================== ====================================== =============================================== ================================ ====================================
		IS_name         component_name      feature_name  concentration_units llod ulod lloq uloq correlation_coefficient n_points transformation_model transformation_model_param_y_weight transformation_model_param_y_datum_min transformation_model_param_y_datum_max transformation_model_param_x_weight transformation_model_param_x_datum_min transformation_model_param_x_datum_max transformation_model_param_symmetric_regression transformation_model_param_slope transformation_model_param_intercept
		=============== =================== ============= =================== ==== ==== ==== ==== ======================= ======== ==================== =================================== ====================================== ====================================== =================================== ====================================== ====================================== =============================================== ================================ ====================================
		_               Antranilicacid      intensity     ug/mL               0.0  0.0  0.5  2500 0.998679668124795       7        linear               ln(y)                               -1.0e15                                1.0e15                                 ln(x)                               -1.0e15                                1.0e15                                 FALSE                                           1.353587567241049                0.369814545757549
		_               Indole              intensity     ug/mL               0.0  0.0  0.5  50.0 0.998763546720702       6        linear               ln(y)                               -1.0e15                                1.0e15                                 ln(x)                               -1.0e15                                1.0e15                                 FALSE                                           0.995574540930201                3.242340261658038
		=============== =================== ============= =================== ==== ==== ==== ==== ======================= ======== ==================== =================================== ====================================== ====================================== =================================== ====================================== ====================================== =============================================== ================================ ====================================

	This file is used to apply the predefined calibration model to each transition by running the ``QUANTIFY_FEATURES`` workflow step.


The workflow steps for LC-MS/MS-SRM Unknowns are :

	.. list-table:: workflow_LCMSSRM_Unknowns.csv
	  :header-rows: 1

	  * - workflow_step
	  * - LOAD_RAW_DATA
	  * - MAP_CHROMATOGRAMS
	  * - PICK_MRM_FEATURES
	  * - QUANTIFY_FEATURES
	  * - CHECK_FEATURES
	  * - SELECT_FEATURES
	  * - STORE_FEATURES

	To inspect the features for the selected transition groups, select "Features (line)" from the view menu
	then open the features tab (can be opened from the view menu as well) to select the "asymetry_factors" and "logSN"
	in the plot column. The line plot illistrates the value for each transition group and feature as shown below:

	.. image:: ../../images/hplc_uv_unknowns_features_line.png

	The features can also be plotted as a heatmap, under "view" select "Features (heatmap)" then select the "asymetry_factors"
	feature to display transition groups as a heatmap and compare the values from the same injection as shown below:

	.. image:: ../../images/hplc_uv_unknowns_features_heatmap.png

	To plot the intensities over time for given injections and transitions, view the "chromatogram" from the "view" menu
	then select the injections and transitions to plot from their respective tabs on the left. The following shows the chromatogram
	for two injections using Antranilicacid and 5-HTP2 transitions and their intensity differences over time.

	.. image:: ../../images/hplc_uv_unknowns_chromatogram.png

#. Running the workflow in SmartPeak

	To run the analysis, please follow the steps for 
	:ref:`Using SmartPeak GUI` or :ref:`Using SmartPeak CLI`
	to execute the workflow steps, review the results, and report the results.

#. Reporting the results

	To export the results, select "Report" from the "Actions" which will show the 
	"Create Report" window:

	.. image:: ../../images/hplc_uv_standards_exports.png

	Based in the data you wish to export, select the desired "Sample types" from the left pane
	and select the "Metadata" from the right pane then click on of the buttons below to create
	the report with the selected items in the csv format. More details on exporting the results can be found 
	in :ref:`Export report`.