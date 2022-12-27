Non-targeted analysis using LC-MS/MS DDA acquisition
----------------------------------------------------

This tutorial walks you through the workflow for analyzing non-targeted liquid chromatography high resolution mass spectrometry (LC-MS/MS) data-dependent acquisition (DDA) 
data starting from input file generation, to processing the data in SmartPeak, 
to reviewing the data in SmartPeak, to reporting the results.

.. image:: ../../images/MassSpecSchemas-DDA.png

Objectives
~~~~~~~~~~

#. Obtaining the SOP for the workflow.
#. Choosing a data set for demonstrating the workflow.
#. Creating an optimized SmartPeak input templates for running the workflow.

The Workflows include
~~~~~~~~~~~~~~~~~~~~~

#. Defining the accurate mass search database
#. Processing Unknowns based MS1 accurate mass and creating a transition library based on MS2 product ion scans
#. Processing Unknowns based MS1 accurate mass and creating a spectral database based on MS2 product ion scans
#. Processing Unknowns based on MS2 product ion scans using an existing spectral database
#. Reviewing the results

Notes
~~~~~

The algorithm parameters used in the following workflows have been highly tuned for feature detection using Time of Flight (Tof) technologies.  
Slight modifications to the algorithm parameters in the ``TargetedSpectraExtractor`` section is needed for Orbitrap technology and other acquisition method specific parameters.

Steps
~~~~~

The tutorial includes the following steps :

#. Setting up the input files

	The data set used can be found in 
	`LC-MS/MS-DDA <https://github.com/AutoFlowResearch/SmartPeak/tree/develop/src/examples/data/DDA>`_ 
	for the transition library and spectral database generation, and spectral library matching workflows.

	The dataset includes a ``CHEMISTRY`` folder which contains HMDB (Human Metabolome Database) mapping files organized as follows:

		#. HMDB2StructMapping.tsv

		This tab seperated file contains mapping of HMDB IDs, IUPAC Name, Compound Summary (Synonyms), Canonical SMILES and InChl.

		#. HMDBMappingFile.tsv

		This tab seperated file contains the Monoisotopic Molecular Weight and the Chemical Formula mapped to their respective HMDB ID.

		#. negative_adducts.tsv

		This file contains negative ion modes including the charge.

		#. positive_adducts.tsv

		This file contains positive ion modes including the charge.

		The above files provided in the example are appropriate for applications involving human serum and other biosamples.  For applications involving other organisms such as bacteria (e.g., E. coli), the use of organism specific databases are recommended to reduce the number of false positives. 
		See `FIA_MS_database_construction.ipynb <https://github.com/AutoFlowResearch/BFAIR/blob/develop/docs/examples/FIA_MS_database_construction_example.ipynb>`_ for an example notebook demonstrating how to convert the metabolites in a genome-scale reconstruction to SmartPeak accurate mass mapping files.
		
	Furthermore, the ``parameters.csv`` file contains the following settings for this workflow:

	.. table:: DDA Settings in parameters.csv
		:widths: auto

		======================== ================================================= =============================================== ======
        function                 name                                              value                                           type
		======================== ================================================= =============================================== ======
		FeatureFindingMetabo     report_chromatograms                              TRUE                                            bool
		FeatureFindingMetabo     report_convex_hulls                               TRUE                                            bool
		FeatureFindingMetabo     use_smoothed_intensities                          FALSE                                           bool
		Pick3DFeatures           enable_elution                                    FALSE                                           bool
		Pick3DFeatures           max_traces                                        1000                                            int
		Pick3DFeatures           force_processing                                  FALSE                                           bool
		TargetedSpectraExtractor AccurateMassSearchEngine:db:mapping               ['CHEMISTRY/HMDBMappingFileGermicidinA.tsv']    list
		TargetedSpectraExtractor AccurateMassSearchEngine:db:struct                ['CHEMISTRY/HMDB2StructMappingGermicidinA.tsv'] list
		TargetedSpectraExtractor AccurateMassSearchEngine:positive_adducts         CHEMISTRY/PositiveAdducts.tsv                   string
		TargetedSpectraExtractor AccurateMassSearchEngine:negative_adducts         CHEMISTRY/NegativeAdducts.tsv                   string
		TargetedSpectraExtractor relative_allowable_product_mass                   50                                              float
		TargetedSpectraExtractor AccurateMassSearchEngine:ionization_mode          auto                                            string
		TargetedSpectraExtractor AccurateMassSearchEngine:mass_error_value         10                                              float
		TargetedSpectraExtractor rt_window                                         60                                              float
		TargetedSpectraExtractor mz_tolerance                                      0.001                                           float
		TargetedSpectraExtractor AccurateMassSearchEngine:keep_unidentified_masses TRUE                                            bool
		======================== ================================================= =============================================== ======

#. Defining the workflow in SmartPeak

For LC-MS/MS-DDA transition library generation analysis, the following steps are saved into the ``workflow.csv`` file.
Alternatively, steps can be replaced, added or deleted direclty from SmartPeakGUI. 
A detailed explanation of each command step can be found in :ref:`Workflow Commands`.

	.. list-table:: workflow_LCMS_DDA_transition.csv
	  :header-rows: 1

	  * - workflow_step
	  * - LOAD_RAW_DATA
	  * - PICK_3D_FEATURES
	  * - SEARCH_SPECTRUM_MS1
	  * - MERGE_FEATURES_MS1
	  * - EXTRACT_SPECTRA_NON_TARGETED
	  * - SEARCH_SPECTRUM_MS2
	  * - MERGE_FEATURES_MS2
	  * - CONSTRUCT_TRANSITIONS_LIST
	  * - STORE_FEATURES

	The calibration curve for each transition's quantitation method can be inspected after all workflow steps have been run, to do so please
	click on view and then "Calibrators". From the menu select ser-L.ser-L_1.Light
	as ``component`` to plot its concentration curves within the given concentration range as shown below:

	.. image:: ../images/calibrators.png

	To inspect the features for the selected transition groups, select "Features (line)" from the view menu
	then open the features tab (can be opened from the view menu as well) to select the "asymetry_factors" and "logSN"
	in the plot column. The line plot illistrates the value for each transition group and feature as shown below:

	.. image:: ../../images/lcms_srm_standards_features_line.png

.. todo::
    The rest of the tutorial.

#. Defining the workflow in SmartPeak

For LC-MS/MS-DDA spectral database generation analysis, the following steps are saved into the ``workflow.csv`` file.
Alternatively, steps can be replaced, added or deleted direclty from SmartPeakGUI. 
A detailed explanation of each command step can be found in :ref:`Workflow Commands`.

	.. list-table:: workflow_LCMS_DDA_spectral.csv
	  :header-rows: 1

	  * - workflow_step
	  * - LOAD_RAW_DATA
	  * - PICK_3D_FEATURES
	  * - SEARCH_SPECTRUM_MS1
	  * - MERGE_FEATURES_MS1
	  * - EXTRACT_SPECTRA_NON_TARGETED
	  * - STORE_MSP
	  * - STORE_FEATURES

.. todo::
    The rest of the tutorial.

#. Defining the workflow in SmartPeak

For LC-MS/MS-DDA spectral database matching analysis, the following steps are saved into the ``workflow.csv`` file.
Alternatively, steps can be replaced, added or deleted direclty from SmartPeakGUI. 
A detailed explanation of each command step can be found in :ref:`Workflow Commands`.

	.. list-table:: workflow_LCMS_DDA_spectra.csv
	  :header-rows: 1

	  * - workflow_step
	  * - LOAD_RAW_DATA
	  * - PICK_3D_FEATURES
	  * - EXTRACT_SPECTRA_NON_TARGETED
	  * - MATCH_SPECTRA
	  * - STORE_FEATURES

.. todo::
    The rest of the tutorial.
