SmartPeak Parameters
====================

This page describes SmartPeak parameters, options and names.

.. _sample-types:

Sample Types
------------

.. list-table::
    :widths: 25 75
    :header-rows: 1

    * - Type
      - Description
    * - Unknown
      - todo
    * - Standard
      - todo
    * - QC
      - todo
    * - Blank
      - todo
    * - Double Blank
      - todo
    * - Solvent
      - todo
    * - Unrecognized
      - todo

.. _metadata:

Metadata
--------

.. list-table::
    :widths: 25 75
    :header-rows: 1

    * - Type
      - Description
    * - asymmetry_factor
      - todo
    * - baseline_delta_2_height
      - todo
    * - calculated_concentration
      - todo
    * - logSN
      - todo
    * - peak_apex_int
      - todo
    * - peak_area
      - todo
    * - points_across_baseline
      - todo
    * - points_across_half_height
      - todo
    * - QC_transition_pass
      - todo
    * - QC_transition_message
      - todo
    * - QC_transition_score
      - todo
    * - QC_transition_group_pass
      - todo
    * - QC_transition_group_message
      - todo
    * - QC_transition_group_score
      - todo
    * - tailing_factor
      - todo
    * - total_width
      - todo
    * - width_at_50
      - todo
    * - RT
      - todo
    * - leftWidth
      - todo
    * - rightWidth
      - todo
    * - scan_polarity
      - todo
    * - description
      - todo
    * - modifications
      - todo
    * - chemical_formula
      - todo
    * - mz
      - todo
    * - charge
      - todo
    * - mz_error_ppm
      - todo
    * - mz_error_Da
      - todo
    * - average_accuracy
      - todo
    * - absolute_difference
      - todo
    
.. _workflow-commands:

Workflow Commands
-----------------

Raw Data Methods
~~~~~~~~~~~~~~~~

.. list-table:: 
    :widths: 25 75
    :header-rows: 1

    * - Type
      - Description
    * - LOAD_RAW_DATA
      - Read in raw data mzML file from disk.
    * - LOAD_FEATURES
      - Read in the features from disk.
    * - PICK_MRM_FEATURES
      - Run the peak picking algorithm for SRMs/MRMs.
    * - FILTER_FEATURES
      - Filter transitions and transitions groups based on a user defined criteria.
    * - SELECT_FEATURES
      - Run the peak selection/alignment algorithm.
    * - VALIDATE_FEATURES
      - Compare selected features to a reference data set.
    * - QUANTIFY_FEATURES
      - Apply a calibration model defined in quantitationMethods to each transition.
    * - CHECK_FEATURES
      - Flag and score transitions and transition groups based on a user defined criteria.
    * - STORE_FEATURES
      - Write the features to disk.
    * - MAP_CHROMATOGRAMS
      - Map chromatograms to the loaded set of transitions.
    * - ZERO_CHROMATOGRAM_BASELINE
      - Normalize the lowest chromatogram intensity to zero.
    * - EXTRACT_CHROMATOGRAM_WINDOWS
      - Extract out specified chromatogram windows using the componentFeatureFilters.
    * - FIT_FEATURES_EMG
      - Reconstruct a peak from available data points.
    * - FILTER_FEATURES_RSDS
      - Filter transitions and transitions groups based on a user defined criteria.
    * - CHECK_FEATURES_RSDS
      - Flag and score transitions and transition groups based on a user defined criteria.
    * - FILTER_FEATURES_BACKGROUND_INTERFERENCES
      - Filter transitions and transitions groups based on a user defined criteria.
    * - CHECK_FEATURES_BACKGROUND_INTERFERENCES
      - Flag and score transitions and transition groups based on a user defined criteria.
    * - EXTRACT_SPECTRA_WINDOWS
      - Extract out specified spectra windows based on the user parameters.
    * - MERGE_SPECTRA
      - Merge all spectra along the time axis.
    * - PICK_2D_FEATURES
      - Run the peak picking algorithm for MS1 spectra.
    * - PICK_3D_FEATURES
      - Pick 3D Features.
    * - SEARCH_ACCURATE_MASS
      - Run the accurate mass search algorithm.
    * - MERGE_FEATURES
      - Create merged features from accurate mass search results.
    * - LOAD_ANNOTATIONS
      - Read in the annotations from disk.
    * - STORE_ANNOTATIONS
      - Write the annotations to disk.
    * - CLEAR_DATA
      - Clear raw and processed data.
    * - STORE_RAW_DATA
      - Store the processed raw data mzML file to disk.
    * - CALCULATE_MDVS
      - Calculate MDVs.
    * - ISOTOPIC_CORRECTIONS
      - Perform Isotopic Corrections.
    * - CALCULATE_MDV_ISOTOPIC_PURITIES
      - Calculate MDV Isotopic Purities.
    * - CALCULATE_MDV_ACCURACIES
      - Compare MDVs to Theoretical.


Sequence Segment Methods
~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: 
    :widths: 25 75
    :header-rows: 1

    * - Type
      - Description
    * - CALCULATE_CALIBRATION
      - Determine the optimal relationship between known sample concentration and measured intensity.
    * - STORE_QUANTITATION_METHODS
      - Write each transitions calibration model to disk for later use.
    * - LOAD_QUANTITATION_METHODS
      - Load each transitions calibration model defined in quantitationMethods from disk.
    * - ESTIMATE_FEATURE_FILTER_VALUES
      - Estimate default FeatureQC parameter values for the feature filters from Standard and QC samples.
    * - ESTIMATE_FEATURE_QC_VALUES
      - Estimate default FeatureQC parameter values for the feature QCs from Standard and QC samples.
    * - TRANSFER_LOQ_TO_FEATURE_FILTERS
      - Transfer the upper (u)/lower (l) limits of quantitation (LOQ) values from the quantitation methods to the calculated concentration bounds of the feature filters.
    * - TRANSFER_LOQ_TO_FEATURE_QCS
      - Transfer the upper (u)/lower (l) limits of quantitation (LOQ) values from the quantitation methods to the calculated concentration bounds of the feature filters.
    * - ESTIMATE_FEATURE_RSDS
      - Estimate the %RSD for component and component group feature filter attributes from pooled QC samples.
    * - ESTIMATE_FEATURE_BACKGROUND_INTERFERENCES
      - Estimate the %BackgroundInterferences for component and component group feature filter ion intensity attributes from Blank samples.
    * - STORE_FEATURE_FILTERS
      - Store the component and component group filters to disk.
    * - LOAD_FEATURE_FILTERS
      - Load the component and component group filters from file.
    * - STORE_FEATURE_QCS
      - Store the component and component group QCs to disk.
    * - LOAD_FEATURE_QCS
      - Load the component and component group QCs from file.
    * - STORE_FEATURE_RSD_FILTERS
      - Store the component and component group percent RSD filters to disk.
    * - LOAD_FEATURE_RSD_FILTERS
      - Load the component and component group percent RSD filters from file.
    * - STORE_FEATURE_RSD_QCS
      - Store the component and component group percent RSD QCs to disk.
    * - LOAD_FEATURE_RSD_QCS
      - Load the component and component group percent RSD QCs from file.
    * - STORE_FEATURE_BACKGROUND_FILTERS
      - Store the component and component group percent Background Interference filters to disk.
    * - LOAD_FEATURE_BACKGROUND_FILTERS
      - Load the component and component group percent Background Interference filters from file.
    * - STORE_FEATURE_BACKGROUND_QCS
      - Store the component and component group percent Background Interference QCs to disk.
    * - LOAD_FEATURE_BACKGROUND_QCS
      - Load the component and component group percent Background Interference QCs from file.
    * - STORE_FEATURE_RSD_ESTIMATIONS
      - Store the component and component group percent RSD estimations to disk.
    * - LOAD_FEATURE_RSD_ESTIMATIONS
      - Load the component and component group percent RSD estimations from file.
    * - STORE_FEATURE_BACKGROUND_ESTIMATIONS
      - Store the component and component group percent Background Interference estimations to disk.
    * - LOAD_FEATURE_BACKGROUND_ESTIMATIONS
      - Load the component and component group percent Background Interference estimations from file.


Sample Group Methods
~~~~~~~~~~~~~~~~~~~~

.. list-table:: 
    :widths: 25 75
    :header-rows: 1

    * - Type
      - Description
    * - MERGE_INJECTIONS
      - Merge multiple injections of the same sample.
    * - LOAD_FEATURES_SAMPLE_GROUP
      - Load the features for the sample group.
    * - STORE_FEATURES_SAMPLE_GROUP
      - Store the features for the sample group.
    

.. _integrity-checks:

Integrity Checks
----------------

.. list-table:: 
    :widths: 25 75
    :header-rows: 1

    * - Type
      - Description
    * - SAMPLE
      - todo
    * - COMP
      - todo
    * - COMP_GROUP
      - todo
    * - IS
      - todo
    