Tutorials & Demos
=============================================================================

This section covers a breadth of walkthroughs using different datasets and analytical techniques.

Data Steps
----------

SmartPeak is optimised to process data set folders with the following structure :

#. featureFilterComponentGroups.csv
	This file contains component group names and their properties such as ``retention_time`` and ``intensity`` with the
	header in the following format :
	``component_group_name,n_heavy_l ,n_heavy_u,n_light_l,n_light_u,n_detecting_l,n_detecting_u,n_quantifying_l,``
	``n_quantifying_u,n_identifying_l,n_identifying_u,n_transitions_l,n_transitions_u,ion_ratio_pair_name_1,ion_ratio_pair_name_2,
	``ion_ratio_l,ion_ratio_u,retention_time_l,retention_time_u,intensity_l,intensity_u,overall_quality_l,``
	``overall_quality_u,retention_time_l_,retention_time_u_``.
	A sample row may look like the following :
	``Serotonin,0,10,0,10,0,10,0,10,0,10,0,10,,,,,1.37,2.37,-1.00E+12,1.00E+15,-1.00E+12,1.00E+12,82.2,142.2``.

#. featureFilterComponents.csv
	This file contains component names and their properties such as ``retention_time`` and ``intensity``. The header is defined
	as follows :
	``component_name,intensity_l,intensity_u,overall_quality_l,overall_quality_u,retention_time_l,retention_time_u``.
	A sample row may be written as follows :
	``Serotonin,100,1.00E+15,-500,1.00E+12,0.87,2.87``
	

#. featureQCComponentGroups.csv
	This file contains QC component group names and their properties such as ``retention_time`` and ``intensity``. The header is defined
	as follows :
	``component_group_name,n_heavy_l ,n_heavy_u,n_light_l,n_light_u,n_detecting_l,n_detecting_u,n_quantifying_l,``
	``n_quantifying_u,n_identifying_l,n_identifying_u,n_transitions_l,n_transitions_u,ion_ratio_pair_name_1,``
	``ion_ratio_pair_name_2,ion_ratio_l,ion_ratio_u,retention_time_l,retention_time_u,intensity_l,intensity_u,overall_quality_l,overall_quality_u``.
	A sample row may be written as follows :
	``Serotonin,0,10,0,10,1,10,1,10,0,10,1,10,,,,,1.37,2.37,0,1.00E+15,-500,1.00E+12``.
	

#. featureQCComponents.csv
	* Purpose : This file contains QC component  names and their properties such as ``retention_time`` and ``intensity``. The header is defined
	as follows :
	``component_name,metaValue_peak_apex_int_l,metaValue_peak_apex_int_u,metaValue_logSN_l,metaValue_logSN_u,``
	``metaValue_total_width_l,metaValue_total_width_u,metaValue_width_at_50_l,metaValue_width_at_50_u,``
	``metaValue_tailing_factor_l,metaValue_tailing_factor_u,metaValue_asymmetry_factor_l,metaValue_asymmetry_factor_u,``
	``metaValue_baseline_delta_2_height_l,metaValue_baseline_delta_2_height_u,metaValue_points_across_baseline_l,``
	``metaValue_points_across_baseline_u,metaValue_points_across_half_height_l,metaValue_points_across_half_height_u,metaValue_logSN_l,``
	``metaValue_logSN_u,intensity_l,intensity_u,overall_quality_l,overall_quality_u``.
	A sample row may be written as follows : 
	``Serotonin,0,5.00E+06,1,1.00E+06,0.1,1,0.001,0.25,0,2,0.8,2.5,-0.25,0.25,20,500,5,500,1,1.00E+06,0,1.00E+15,0,1.00E+12``.
	
#. features
	This folder contains features info in the 
	`featureXML <https://raw.githubusercontent.com/OpenMS/OpenMS/develop/share/OpenMS/SCHEMAS/FeatureXML_1_9.xsd>`_ format.

#. mzML
	This folder contains `mzML <https://www.psidev.info/mzML>`_ files which descripes raw spectrometer value, a single mzML file usually encapsulates all the
	information extracted from a single MS run.

#. parameters.csv
	This file contains a list of parameters for the RawDataProcessor functions, the values stored are organised by the header :
	``function,name,type,value,default,restrictions,description,used_,comment_,comparator``, a sample entry may look like this :
	``MRMMapping,precursor_tolerance,float,0.0009,0.1,0.1,Precursor tolerance when mapping (in Th),TRUE,,``.

#. quantitationMethods.csv
	This file contains information about various quantitation methods and their values. The header is standarised as follows
	``IS_name,component_name,feature_name,concentration_units,llod,ulod,lloq,uloq,correlation_coefficient,``
	``actual_concentration,n_points,transformation_model,transformation_model_param_slope,transformation_model_param_intercept,``
	``transformation_model_param_x_weight,transformation_model_param_y_weight,transformation_model_param_x_datum_min,``
	``transformation_model_param_x_datum_max,transformation_model_param_y_datum_min,transformation_model_param_y_datum_max``.
	A sample entry in the file may looks like this :
	``s7p.s7p_1.Heavy,s7p.s7p_1.Light,peak_apex_int,uM,,,,,,,,linear,1.88269,1.93E-04,,,1.00E-15,1.00E+15,1.00E-15,1.00E+15``

#. sequence.csv
	A list of injections are store in this file following the header schema :
	``sample_name,sample_group_name,sequence_segment_name,sample_type,original_filename,batch_name,rack_number,``
	``plate_number,pos_number,inj_number,dilution_factor,inj_volume,inj_volume_units,operator_name,acq_method_name,``
	``proc_method_name,acquisition_date_and_time,scan_polarity,scan_mass_low,scan_mass_high``.
	A sample entry may look like the following :
	``170808_Jonathan_yeast_Sacc2_1x,group1,sequence1,Unknown,170808_Jonathan_yeast_Sacc2_1x,BatchName,,,,2,,3,uL,,MethodName,,,,,``.

#. standardsConcentrations.csv
	This file contains a list of sample names and component names along with concentration values and dilution factors. The header has
	the following format : 
	``sample_name,component_name,IS_component_name,actual_concentration,IS_actual_concentration,concentration_units,dilution_factor``.
	With a sample row that may look like the following :
	``0p5ug,Serotonin,,0.5,1,ug/mL,1``

#. traML.csv
	This file contains a list of proteins with peptide names, the header follows the following format :
	``ProteinName,FullPeptideName,transition_group_id,transition_name,RetentionTime,Annotation,PrecursorMz,MS1 Res,``
	``ProductMz,MS2 Res,Dwell,Fragmentor,Collision Energy,Cell Accelerator Voltage,LibraryIntensity,decoy,PeptideSequence,``
	``LabelType,PrecursorCharge,FragmentCharge,FragmentType,FragmentSeriesNumber,quantifying_transition,identifying_transition,detecting_transition``.
	With a sample entry that may look like the following :
	``Serotonin,,Serotonin,Serotonin,1.87,112.2,0,Unit,0,Unit,,,,,1,0,,Light,1,1,,1,TRUE,FALSE,TRUE``.

#. workflow.csv
	The workflow steps, which are parsed by SmartPeak to process the data, are listed in this file under the column
	``command_name``. A full list of the commands can be found in :ref:`Workflow Commands`.


Targeted quantitation with HPLC data
------------------------------------

This tutorial walks you through the workflow for analyzing targeted HPLC data starting from input file generation, 
to processing the data in SmartPeak, to reviewing the data in SmartPeak, to reporting the results for later use.

Objectives
~~~~~~~~~~

#. Obtaining the SOP for the workflow.
#. Choosing a data set for demonstrating the workflow.
#. Creating an optimized SmartPeak input templates for running the workflow.

The Workflows include
~~~~~~~~~~~~~~~~~~~~

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

For HPLC UV Standards analysis, the following steps are saved into the ``workflow.csv`` file. Alternatively,
steps can be replaced, added or deleted direclty from SmartPeakGUI. A detailed explanation of each command step
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

	To run the analysis, please follow the steps for :ref:`Using SmartPeak GUI` or :ref:`Using SmartPeak CLI`
	to execute the workflow steps and review the results including plotting.

#. Reporting the results

	To export the results, please follow the :ref:`Export report`.




Targeted quantitation with LC-MS/MS 5500 QTRAP RapidRIP
-------------------------------------------------------



Targeted quantitation with LC-MS/MS 6500 Polar
----------------------------------------------



Targeted quantitation with LC-MS/MS 6500 Non-polar
--------------------------------------------------



Targeted flux analysis with LC-MS/MS 5500 QTRAP
----------------------------------------------



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


