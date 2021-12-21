Targeted flux analysis with LC-MS/MS Agilent Lipidomics
-------------------------------------------------------

This tutorial walks you through the workflow for analyzing targeted flux analysis 
using LC-MS/MS data starting from input file generation, to processing the data in SmartPeak, 
to reviewing the data in SmartPeak, to reporting the results for later use.

Objectives
~~~~~~~~~~

#. Obtaining the SOP for the workflow.
#. Choosing a data set for demonstrating the workflow.
#. Creating an optimized SmartPeak input templates for running the workflow.

The Workflows include
~~~~~~~~~~~~~~~~~~~~~

#. Processing Unknowns
#. Reviewing the results

Steps
~~~~~

The tutorial includes the following steps :

#. Setting up the input files

	The data set used can be found here 
	`LCMS Targeted Flux Analysis <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/LCMS_TARGETED_FLUX>`_.

	The dataset includes a ``parameters.csv`` file containing parameter settings for ``MRMFeatureFinderScoring``, ``MRMFeatureSelector``, 
	``MRMFeatureValidator``, ``ReferenceDataMethods``, ``MRMFeatureSelector``, ``MRMMapping``, ``FeaturePlotter``, ``SequenceSegmentPlotter``
	and ``AbsoluteQuantitation``.


#. Defining the workflow in SmartPeak

For the targeted flux analysis via LC-MS/MS, the following steps are saved 
into the ``workflow.csv`` file. Alternatively, steps can be replaced, 
added or deleted direclty from SmartPeakGUI within the "workflow" tap in the right pane. 
A detailed explanation of each command step
can be found in :ref:`Workflow Commands`.

	* LOAD_RAW_DATA
	* MAP_CHROMATOGRAMS
	* PICK_MRM_FEATURES
	* FILTER_FEATURES
	* VALIDATE_FEATURES
	* STORE_FEATURES

	The workflow pipeline is initialized by loading the raw data followed 
	by mapping the chromatograms to the loaded set og transitions. Once done, the peak picking routine
	will be executed on the SRM/MRM features followed by filtering the transitions and transition
	groups based on the user defined criteria. Before storing the features for the sample group to disk as a 
	``featureXML`` file, the selected features are compared with the reference dataset.

	To plot the intensities over time for given injections and transitions, view the "chromatogram" from the "view" menu
	then select the injections and transitions to plot from their respective tabs on the left. The following shows the chromatogram
	for one injection using 6pgc and 23dpg transitions and their intensity differences over time.

	.. image:: ../images/lcms_targeted_chromatogram.png

	The Spectra for the two injection samples can be inspected after all workflow steps had been run, to do so please
	click on view and then "Spectra". From the Injections tab check "Plot/Unplot All" select all injection samples and 
	plot the mass to charge ratio relative to their respective intensities as shown below:

	.. image:: ../../images/lcms_targeted_flux_spectra.png

	#. Reporting the results

	To export the results, select "Report" from the "Actions" which will show the 
	"Create Report" window:

	.. image:: ../../images/lcms_targeted_flux_exports.png

	Based in the data you wish to export, select the desired "Sample types" from the left pane
	and select the "Metadata" from the right pane then click on one of the buttons below to create
	the report with the selected items in the csv format. 
	
	Other options to export the data includes "Group Feature DB" and "Group Pivot Table". The Group Feature DB
	includes all ``sample_group_name``, ``component_group_name``, ``component_name`` and any other additional metadata
	such as ``peak_apex_int``, ``peak_area``, and ``mz`` values. While the Group Pivot Table would include ``component_name``
	``component_group_name``, ``meta_value`` such as ``peak_apex_int`` and ``RT``, and ``CM`` values.
	
	More details on exporting the results can be found in :ref:`Export report`.

	The resulting ``featureXML`` and ``mzTab`` are saved in the features folder. ``featureXML`` files describe the spectra
	data for a given injection sample which includes a list of features with a set of ``UserParam`` for each feature such as
	``PeptideRef``, ``native_id`` and ``scan_polarity``.
  
  
