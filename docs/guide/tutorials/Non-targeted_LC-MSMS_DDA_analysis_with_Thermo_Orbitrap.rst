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

.. todo::
    The rest of the tutorial.
