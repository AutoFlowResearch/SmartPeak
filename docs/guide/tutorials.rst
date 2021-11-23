Tutorials & Demos
=============================================================================

This section covers a breadth of walkthroughs using different datasets and analytical techniques.

Structure of Data Sets
----------------------

SmartPeak is optimised to process data set folders with the following structure :

#. featureFilterComponentGroups.csv
	This file contains component group names and their properties such as ``retention_time`` and ``intensity``. 
	The header including a sample entry is shown below:

	.. table:: featureFilterComponentGroups.csv Headers
		:widths: auto

		==================== ========= ========= ========= ========= ============= ============= =============== =============== =============== =============== =============== =============== ===================== ===================== =========== =========== ================ ================ =========== =========== ================= ================= ================= =================
		component_group_name n_heavy_l n_heavy_u n_light_l n_light_u n_detecting_l n_detecting_u n_quantifying_l n_quantifying_u n_identifying_l n_identifying_u n_transitions_l n_transitions_u ion_ratio_pair_name_1 ion_ratio_pair_name_2 ion_ratio_l ion_ratio_u retention_time_l retention_time_u intensity_l intensity_u overall_quality_l overall_quality_u retention_time_l_ retention_time_u_
		==================== ========= ========= ========= ========= ============= ============= =============== =============== =============== =============== =============== =============== ===================== ===================== =========== =========== ================ ================ =========== =========== ================= ================= ================= =================
		Serotonin            0         10        0         10        0             10            0               10              0               10              0               10                                                                                  1.37             2.37             -1.00E+12   1.00E+15    -1.00E+12         1.00E+12          82.2              142.2
		\-                   \-        \-        \-        \-        \-            \-            \-              \-              \-              \-              \-              \-              \-                    \-                    \-          \-          \-               \-               \-          \-          \-                \-                \-                \-
		==================== ========= ========= ========= ========= ============= ============= =============== =============== =============== =============== =============== =============== ===================== ===================== =========== =========== ================ ================ =========== =========== ================= ================= ================= =================

|

#. featureFilterComponents.csv
	This file contains component names and their properties such as ``retention_time`` and ``intensity``. 
	The header including a sample entry is shown below:

	.. table:: featureFilterComponents.csv Headers
		:widths: auto

		==================== =========== =========== ================= ================= ================ ================
		component_name       intensity_l intensity_u overall_quality_l overall_quality_u retention_time_l retention_time_u
		==================== =========== =========== ================= ================= ================ ================
		Serotonin            100         1.00E+15    -500              1.00E+12          0.87             2.87
		\-                   \-          \-          \-                \-                \-               \-
		==================== =========== =========== ================= ================= ================ ================

|

#. featureQCComponentGroups.csv
	This file contains QC component group names and their properties such as ``retention_time`` and ``intensity``. 
	The header including a sample entry is shown below:

	.. table:: featureFilterComponents.csv Headers
		:widths: auto

	==================== ========= ========= ========= ========= ============= ============= =============== =============== =============== =============== =============== =============== ===================== ===================== =========== =========== ================ ================ =========== =========== ================= =================
	component_group_name n_heavy_l n_heavy_u n_light_l n_light_u n_detecting_l n_detecting_u n_quantifying_l n_quantifying_u n_identifying_l n_identifying_u n_transitions_l n_transitions_u ion_ratio_pair_name_1 ion_ratio_pair_name_2 ion_ratio_l ion_ratio_u retention_time_l retention_time_u intensity_l intensity_u overall_quality_l overall_quality_u
	==================== ========= ========= ========= ========= ============= ============= =============== =============== =============== =============== =============== =============== ===================== ===================== =========== =========== ================ ================ =========== =========== ================= =================
	Serotonin            0         10        0         10        1             10            1               10              0               10              1               10                                                                                  1.37             2.37             0           1.00E+15    -500              1.00E+12
	\-                   \-        \-        \-        \-        \-            \-            \-              \-              \-              \-              \-              \-              \-                    \-                    \-          \-          \-               \-               \-          \-          \-                \-
	==================== ========= ========= ========= ========= ============= ============= =============== =============== =============== =============== =============== =============== ===================== ===================== =========== =========== ================ ================ =========== =========== ================= =================	

|

#. featureQCComponents.csv
	This file contains feature QC component names and their properties such as ``metaValue_peak_apex_int_l`` and ``metaValue_logSN_l``. 
	The header including a sample entry is shown below:

	.. table:: featureQCComponents.csv Headers
		:widths: auto

	============== ========================= ========================= ================= ================= ======================= ======================= ======================= ======================= ========================== ========================== ============================ ============================ =================================== =================================== ================================== ================================== ===================================== ===================================== ================= ================= =========== =========== ================= =================
	component_name metaValue_peak_apex_int_l metaValue_peak_apex_int_u metaValue_logSN_l metaValue_logSN_u metaValue_total_width_l metaValue_total_width_u metaValue_width_at_50_l metaValue_width_at_50_u metaValue_tailing_factor_l metaValue_tailing_factor_u metaValue_asymmetry_factor_l metaValue_asymmetry_factor_u metaValue_baseline_delta_2_height_l metaValue_baseline_delta_2_height_u metaValue_points_across_baseline_l metaValue_points_across_baseline_u metaValue_points_across_half_height_l metaValue_points_across_half_height_u metaValue_logSN_l metaValue_logSN_u intensity_l intensity_u overall_quality_l overall_quality_u
	============== ========================= ========================= ================= ================= ======================= ======================= ======================= ======================= ========================== ========================== ============================ ============================ =================================== =================================== ================================== ================================== ===================================== ===================================== ================= ================= =========== =========== ================= =================
	Serotonin      0                         5.00E+06                  1                 1.00E+06          0.1                     1                       0.001                   0.25                    0                          2                          0.8                          2.5                          -0.25                               0.25                                20                                 500                                5                                     500                                   1                 1.00E+06          0           1.00E+15    0                 1.00E+12
	\-             \-                        \-                        \-                \-                \-                      \-                      \-                      \-                      \-                         \-                         \-                           \-                           \-                                  \-                                  \-                                 \-                                 \-                                    \-                                    \-                \-                \-          \-          \-                \-
	============== ========================= ========================= ================= ================= ======================= ======================= ======================= ======================= ========================== ========================== ============================ ============================ =================================== =================================== ================================== ================================== ===================================== ===================================== ================= ================= =========== =========== ================= =================

|	

#. features
	This folder contains features info either in the 
	`featureXML <https://raw.githubusercontent.com/OpenMS/OpenMS/develop/share/OpenMS/SCHEMAS/FeatureXML_1_9.xsd>`_ format or in 
	`Chromeleon CDS7 TXT <https://www.thermofisher.com/order/catalog/product/CHROMELEON7>`_ format. 
	Features contain the processed results of a single sample after applying a workflow and running the command ``STORE_FEATURES`` and
	can be stored at both the sample (i.e., injection) or the sample group (merged injections) level.
|

#. mzML
	This folder contains mass spectrometry data files in the most widely open-source format : `mzML <https://www.psidev.info/mzML>`_ 
	which descripes raw spectrometer value, a single mzML file usually encapsulates all the information extracted from a single MS run.

	Converting raw files to the ``mzML`` format
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	`Proteowizard <https://proteowizard.sourceforge.io/download.html>`_ is a toolkit to view and convert mass spectrometry
	data. The tools kit includes SeeMS and MSConvert to visualize MS data and convert to and from ``mzML`` file format respectively.

	Once installed, you will be presented with the following window:

	.. image:: ../images/msconvert.png

	To convert mass spectrometry files, please follow the steps below:

	#. Select the file you wish to convert.
	#. Add the selected file to the list of files to process.
	#. Select the output directory where the converted files will be saved.
	#. Select the output format, providing an extension to the file in not mandatory.
	#. Check/uncheck compression and packaging options if desired.
	#. Select how many files you wish to convert at once, then hit the "Start" button.


|

#. parameters.csv
	This file contains a list of parameters for the workflow steps as well as various other application settings,
	the header including a sample entry is shown below:

	.. table:: parameters.csv Headers
		:widths: auto

		========== =================== ===== ====== ======= ============ ======================================== ===== ========= ==========
		function   name                type  value  default restrictions description used_                              comment_  comparator
		========== =================== ===== ====== ======= ============ ======================================== ===== ========= ==========
		MRMMapping precursor_tolerance float 0.0009 0.1     0.1          Precursor tolerance when mapping (in Th) TRUE
		\-         \-                  \-    \-     \-      \-           \-                                       \-    \-        \-
		========== =================== ===== ====== ======= ============ ======================================== ===== ========= ==========

|

#. quantitationMethods.csv
	This file contains information about various quantitation methods and their values,
	and is required for absolute quantitation.
	The header including a sample entry is shown below:

	.. table:: quantitationMethods.csv Headers
		:widths: auto

		=============== =================== ============= =================== ==== ==== ==== ==== ======================= ==================== ======== ==================== ================================ ==================================== =================================== =================================== ====================================== ====================================== ====================================== ======================================
		IS_name         component_name      feature_name  concentration_units llod ulod lloq uloq correlation_coefficient actual_concentration n_points transformation_model transformation_model_param_slope transformation_model_param_intercept transformation_model_param_x_weight transformation_model_param_y_weight transformation_model_param_x_datum_min transformation_model_param_x_datum_max transformation_model_param_y_datum_min transformation_model_param_y_datum_max
		=============== =================== ============= =================== ==== ==== ==== ==== ======================= ==================== ======== ==================== ================================ ==================================== =================================== =================================== ====================================== ====================================== ====================================== ======================================
		s7p.s7p_1.Heavy s7p.s7p_1.Light     peak_apex_int uM                                                                                            linear               1.88269                          1.93E-04                                                                                                     1.00E-15                               1.00E+15                               1.00E-15                               1.00E+15
		\-              \-                  \-            \-                  \-   \-   \-   \-   \-                      \-                   \-       \-                   \-                               \-                                   \-                                  \-                                  \-                                     \-                                     \-                                     \-
		=============== =================== ============= =================== ==== ==== ==== ==== ======================= ==================== ======== ==================== ================================ ==================================== =================================== =================================== ====================================== ====================================== ====================================== ======================================

|

#. sequence.csv
	This file contains information about all the injections in the data set and their values,
	fields such as `sample_name` and `original_filename` refer to the files names stored in the `mzML` folder.
	The header including a sample entry is shown below:

	.. table:: parameters.csv Headers
		:widths: auto

		============================== ================= ===================== =========== ============================== ========== =========== ============ ========== ========== =============== ========== ================ ============= =============== ================ ========================= ============= ============= ==============
		sample_name                    sample_group_name sequence_segment_name sample_type original_filename              batch_name rack_number plate_number pos_number inj_number dilution_factor inj_volume inj_volume_units operator_name acq_method_name proc_method_name acquisition_date_and_time scan_polarity scan_mass_low scan_mass_high
		============================== ================= ===================== =========== ============================== ========== =========== ============ ========== ========== =============== ========== ================ ============= =============== ================ ========================= ============= ============= ==============
		170808_Jonathan_yeast_Sacc2_1x group1            sequence1             Unknown     170808_Jonathan_yeast_Sacc2_1x BatchName                                      2                          3          uL                             MethodName
		\-                             \-                \-                    \-          \-                             \-         \-          \-           \-         \-         \-              \-         \-               \-            \-              \-               \-                        \-            \-            \-
		============================== ================= ===================== =========== ============================== ========== =========== ============ ========== ========== =============== ========== ================ ============= =============== ================ ========================= ============= ============= ==============

|

#. standardsConcentrations.csv
	This file contains information about concentration values fot the provided samples/components,
	this file is required for automated calibration curve fitting.
	The header including a sample entry is shown below:

	.. table:: standardsConcentrations.csv Headers
		:widths: auto

		================= =================== =================== ==================== ======================= =================== ===============
		sample_name       component_name      IS_component_name   actual_concentration IS_actual_concentration concentration_units dilution_factor
		================= =================== =================== ==================== ======================= =================== ===============
		150516_CM1_Level1 23dpg.23dpg_1.Light 23dpg.23dpg_1.Heavy 0                    1                       uM                  1
		\-                \-                  \-                  \-                   \-                      \-                  \-
		================= =================== =================== ==================== ======================= =================== ===============

|

#. traML.csv
	This file contains a summary of the proteins or metabolites with transition names and other related information,
	the header including a sample entry is shown below:

	.. table:: traML.csv Headers
		:widths: auto

		=========== =============== =================== =================== ============= ========== =========== ======= ========= ======= ===== ========== ================ ======================== ================ ===== =============== ========= =============== ============== ============ ==================== ====================== ====================== ====================
		ProteinName FullPeptideName transition_group_id transition_name     RetentionTime Annotation PrecursorMz MS1 Res ProductMz MS2 Res Dwell Fragmentor Collision Energy Cell Accelerator Voltage LibraryIntensity decoy PeptideSequence LabelType PrecursorCharge FragmentCharge FragmentType FragmentSeriesNumber quantifying_transition identifying_transition detecting_transition
		=========== =============== =================== =================== ============= ========== =========== ======= ========= ======= ===== ========== ================ ======================== ================ ===== =============== ========= =============== ============== ============ ==================== ====================== ====================== ====================
		arg-L                       arg-L               arg-L.arg-L_1.Heavy 45.85610358              179         Unit    136       Unit                                                               1                0                     Heavy     1               1                           1                    TRUE                   FALSE                  TRUE
		\-          \-              \-                  \-                  \-            \-         \-          \-      \-        \-      \-    \-         \-               \-                       \-               \-    \-              \-        \-              \-             \-           \-                   \-                     \-                     \-
		=========== =============== =================== =================== ============= ========== =========== ======= ========= ======= ===== ========== ================ ======================== ================ ===== =============== ========= =============== ============== ============ ==================== ====================== ====================== ====================

|

#. workflow.csv
	The workflow steps, which are parsed by SmartPeak to process the data, 
	are listed in this file under the column ``command_name``. 
	A full list of the commands can be found in :ref:`Workflow Commands`.

|

Targeted quantitation with HPLC data
------------------------------------

This tutorial walks you through the workflow for analyzing targeted HPLC data
starting from input file generation, to processing the data in SmartPeak, 
to reviewing the data in SmartPeak, to reporting the results for later use.

Objectives
~~~~~~~~~~

#. Obtaining the SOP for the workflow.
#. Choosing a data set for demonstrating the workflow.
#. Creating an optimized SmartPeak input templates for running the workflow.

The Workflows include
~~~~~~~~~~~~~~~~~~~~~

#. Calculating the calibration curves using Standards
#. Processing Unknowns

Steps
~~~~~

The tutorial includes the following steps :

#. Setting up the input files

The data set used can be found in 
`HPLC_UV_Standards <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/HPLC_UV_Standards>`_ and
`HPLC_UV_Unknowns <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/HPLC_UV_Unknowns>`_ 
for the HPLC UV Standards and HPLC UV Unknowns respectively.

#. Defining the workflow in SmartPeak

For HPLC UV Standards analysis, the following steps are saved 
into the ``workflow.csv`` file. Alternatively, steps can be replaced, 
added or deleted direclty from SmartPeakGUI. 
A detailed explanation of each command step
can be found in :ref:`Workflow Commands`.

	* LOAD_RAW_DATA
	* MAP_CHROMATOGRAMS
	* EXTRACT_CHROMATOGRAM_WINDOWS
	* ZERO_CHROMATOGRAM_BASELINE
	* PICK_MRM_FEATURES
	* CHECK_FEATURES
	* SELECT_FEATURES
	* CALCULATE_CALIBRATION
	* STORE_QUANTITATION_METHODS
	* QUANTIFY_FEATURES
	* STORE_FEATURES

	The calibration curve can be inspected after all workflow steps had been run, to do so please
	click on view and then "Calibrators". From the transition tab select Antranilicacid and Indole
	as ``transition_group`` to plot their concentration curves within the given concentration range as
	shown below:

	.. image:: ../images/hplc_uv_standards_calibration_curve.png

	To inspect the features for the selected transition groups, select "Features (line)" from the view menu
	then open the features tab (can be opened from the view menu as well) to select the "asymetry_factors" and "logSN"
	in the plot column. The line plot illistrates the value for each transition group and feature as shown below:

	.. image:: ../images/hplc_uv_standards_features_line.png

	The features can also be plotted as a heatmap, under "view" select "Features (heatmap)" then select the "left_width"
	feature to display transition groups as a heatmap and compare the values from the same injection as shown below:

	.. image:: ../images/hplc_uv_standards_features_heatmap.png

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


The workflow steps for HPLC UV Unknowns are :

	* LOAD_RAW_DATA
	* MAP_CHROMATOGRAMS
	* EXTRACT_CHROMATOGRAM_WINDOWS
	* ZERO_CHROMATOGRAM_BASELINE
	* PICK_MRM_FEATURES
	* QUANTIFY_FEATURES
	* CHECK_FEATURES
	* SELECT_FEATURES
	* STORE_FEATURES

#. Running the workflow in SmartPeak

	To run the analysis, please follow the steps for 
	:ref:`Using SmartPeak GUI` or :ref:`Using SmartPeak CLI`
	to execute the workflow steps and review the results including plotting.

#. Reporting the results

	To export the results, select "Report" from the "Actions" which will show the 
	"Create Report" window:

	.. image:: ../images/hplc_uv_standards_exports.png

	Based in the data you wish to export, select the desired "Sample types" from the left pane
	and select the "Metadata" from the right pane then click on of the buttons below to create
	the report with the selected items in the csv format. More details on exporting the results can be found 
	in :ref:`Export report`.




Targeted quantitation with LC-MS/MS 5500 QTRAP RapidRIP
-------------------------------------------------------



Targeted flux analysis with LC-MS/MS 5500 QTRAP
-----------------------------------------------



Targeted flux analysis with GC-MS full-scan Agilent
---------------------------------------------------



Targeted flux analysis with GC-MS SIM Agilent
---------------------------------------------



Non-targeted FIA-MS analysis with Thermo Orbitrap
-------------------------------------------------



Non-targeted LC-MS/MS DDA analysis with Thermo Orbitrap
-------------------------------------------------------



Non-targeted LC-MS/MS DIA analysis with Thermo Orbitrap
-------------------------------------------------------


