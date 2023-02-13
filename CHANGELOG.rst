SmartPeak Changelog
===================

v1.27.0 (2023-02-12)
--------------------

Features
~~~~~~~~
- Updated documentation  `(#554) <https://github.com/AutoFlowResearch/SmartPeak/pull/554>`_
- Preset Workflow `(#553) <https://github.com/AutoFlowResearch/SmartPeak/pull/553>`_

v1.26.0 (2022-08-29)
--------------------

Fix
~~~
- Using latest OpenMS modifications related to DDA implementation

v1.25.0 (2022-08-02)
--------------------

Features
~~~~~~~~
- Added option to display only spectrogram data that contains convexhull `(#545) <https://github.com/AutoFlowResearch/SmartPeak/pull/545>`_

Fix
~~~
- Optimized commands creations `(#543) <https://github.com/AutoFlowResearch/SmartPeak/pull/543>`_
- prioritized the use of subordinate metadata when choosing between feature level and subordinate level `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_
- update to feature map after call to targetedMatching `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_
- added rawDataHandler member for Library `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_
- matchSpectra requirements for msp `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_
- changed getRequirements to getFilenameRequirements `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_
- renamed library to spectra library `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_
- updated RawDataHandler pointer logic for spectra_library `(#544) <https://github.com/AutoFlowResearch/SmartPeak/pull/544>`_

v1.24.0 (2022-05-25)
--------------------

Features
~~~~~~~~
- Removed scan explorer window `(#540) <https://github.com/AutoFlowResearch/SmartPeak/pull/540>`_

Fix
~~~
- Fix StoreFeatureBackgroundEstimations and generalized export to other storing processors `(#536) <https://github.com/AutoFlowResearch/SmartPeak/pull/536>`_
- Fix/missing report metadata values `(#541) <https://github.com/AutoFlowResearch/SmartPeak/pull/541>`_

v1.23.0 (2022-04-28)
--------------------

Features
~~~~~~~~
- DDA Implementation `(#534) <https://github.com/AutoFlowResearch/SmartPeak/pull/534>`_
    - Added "LCMS DDA Transitions Library Construction" workflow
    - Added "LCMS DDA Spectra Library Construction" workflow
    - Added "LCMS DDA Spectra Library Matching" workflow
    - PickMS1Features renamed Pick2DFeatures 
    - PickMS2Features renamed Pick3DFeatures

v1.22.0 (2022-04-12)
--------------------

Features
~~~~~~~~
- Number of running threads is now an input on the Run Workflow Dialog `(#529) <https://github.com/AutoFlowResearch/SmartPeak/pull/529>`_
- Log messages improved `(#528) <https://github.com/AutoFlowResearch/SmartPeak/pull/528>`_
- Create output folder `(#527) <https://github.com/AutoFlowResearch/SmartPeak/pull/527>`_

Fix
~~~
- Fix loading and exporting for FeatureBackgroundEstimations, FeatureRSDEstimations, QuantitationMethods and StandardsConcentrations `(#532) <https://github.com/AutoFlowResearch/SmartPeak/pull/532>`_
- Fix inclusion/exclusion of points for multiple Calibration display `(#531) <https://github.com/AutoFlowResearch/SmartPeak/pull/531>`_

v1.21.0 (2022-03-21)
--------------------

Features
~~~~~~~~
- Possibility to export Quantitations Methods `(#522) <https://github.com/AutoFlowResearch/SmartPeak/pull/522>`_
- Improving Calibration curve window: Possibility to exclude points, show chromatogram, editable parameters (WARNING: CALCULATE_CALIBRATION has been renamed OPTIMIZE_CALIBRATION) `(#515) <https://github.com/AutoFlowResearch/SmartPeak/pull/515>`_
- Show Inputs and Outputs in Workflow Step display `(#520) <https://github.com/AutoFlowResearch/SmartPeak/pull/520>`_
- Transformation_model_param_x_weight and transformation_model_param_y_weight do not accept empty values (replaced by 'x' and 'y') `(#519) <https://github.com/AutoFlowResearch/SmartPeak/pull/519>`_
- New Workflow Steps display `(#517) <https://github.com/AutoFlowResearch/SmartPeak/pull/517>`_
- Check missing or wrong columns for Quantitations Methods and Standards Concentrations files `(#512) <https://github.com/AutoFlowResearch/SmartPeak/pull/512>`_
- Check csv files BOM `(#513) <https://github.com/AutoFlowResearch/SmartPeak/pull/513>`_

v1.20.0 (2022-01-27)
--------------------

Features
~~~~~~~~
- Improving Calibration curve display: show parameters, zooming, outlier points `(#507) <https://github.com/AutoFlowResearch/SmartPeak/pull/507>`_
- Improving logs: wrapping, auto scroll and line hovering `(#509) <https://github.com/AutoFlowResearch/SmartPeak/pull/509>`_
- Improving zooming on plots `(#508) <https://github.com/AutoFlowResearch/SmartPeak/pull/508>`_
- Using native system file picker (ImGui flavor still available as an option) `(#505) <https://github.com/AutoFlowResearch/SmartPeak/pull/505>`_
- Adding server building files `(#496) <https://github.com/AutoFlowResearch/SmartPeak/pull/496>`_

v1.19.0 (2022-01-10)
--------------------

Features
~~~~~~~~
- Walkthrough : Non-targeted FIA-MS analysis with Thermo Orbitrap `(#460) <https://github.com/AutoFlowResearch/SmartPeak/pull/460>`_
- doc : Targeted flux analysis with LC-MS/MS Agilent Lipidomics `(#473) <https://github.com/AutoFlowResearch/SmartPeak/pull/473>`_
- Walkthrough: Targeted flux analysis with GC-MS full-scan Agilent `(#476) <https://github.com/AutoFlowResearch/SmartPeak/pull/476>`_
- Marking highest values in 2D Plots `(#475) <https://github.com/AutoFlowResearch/SmartPeak/pull/475>`_
- Report Doalog box now confirms success or error once the report is created `(#480) <https://github.com/AutoFlowResearch/SmartPeak/pull/480>`_
- Distributed Mode `(#437) <https://github.com/AutoFlowResearch/SmartPeak/pull/437>`_

Fix
~~~
- Fixed session error due to regex `(#478) <https://github.com/AutoFlowResearch/SmartPeak/pull/478>`_
- Fix some file picker don't popup when selecting file in the New Session dialog box `(#479) <https://github.com/AutoFlowResearch/SmartPeak/pull/479>`_

Changes
~~~~~~~
- upgrade CI macOS build to 13.1 `(#472) <https://github.com/AutoFlowResearch/SmartPeak/pull/472>`_

v1.18.0 (2021-12-16)
--------------------

Features
~~~~~~~~
- ReadTheDocs Walkthrough : Targeted quantitation with HPLC data `(#454) <https://github.com/AutoFlowResearch/SmartPeak/pull/454>`_
- UI Splitting and Docking `(#464) <https://github.com/AutoFlowResearch/SmartPeak/pull/464>`_
- Support for small screens `(#466) <https://github.com/AutoFlowResearch/SmartPeak/pull/466>`_
- Enabled viewports `(#467) <https://github.com/AutoFlowResearch/SmartPeak/pull/467>`_
- Increased max points to be displayed to 10 millions `(#470) <https://github.com/AutoFlowResearch/SmartPeak/pull/470>`_

Fix
~~~
- Fixing SVG exports and deprecating HTML support `(#458) <https://github.com/AutoFlowResearch/SmartPeak/pull/458>`_
- Fixing FIAMS workflow and quantitationMethods.csv override `(#461) <https://github.com/AutoFlowResearch/SmartPeak/pull/461>`_
- Fixed Cancel button size for directory selection dialog box `(#469) <https://github.com/AutoFlowResearch/SmartPeak/pull/469>`_

v1.17.0 (2021-11-16)
--------------------

Features
~~~~~~~~
- Save layout in session `(#452) <https://github.com/AutoFlowResearch/SmartPeak/pull/452>`_

Fix
~~~
- Read the Docs maintenance `(#447) <https://github.com/AutoFlowResearch/SmartPeak/pull/447>`_
- Display error in info panel when user select incorrect file for session file. `(#456) <https://github.com/AutoFlowResearch/SmartPeak/pull/456>`_
- Fix/fix sample group processing - processors can run into infinite loop. `(#450) <https://github.com/AutoFlowResearch/SmartPeak/pull/450>`_
- Enabled OpenMP for MacOS builds (fix crash when running multithreaded workflows) `(#455) <https://github.com/AutoFlowResearch/SmartPeak/pull/455>`_

Changes
~~~~~~~
- Modifying start menu title to include SmartPeakGUI version `(#451) <https://github.com/AutoFlowResearch/SmartPeak/pull/451>`_

v1.16.0 (2021-10-25)
--------------------

Features
~~~~~~~~
- SmartPeak support for exporting heatmaps as PNGs `(#433) <https://github.com/AutoFlowResearch/SmartPeak/pull/433>`_
- Display Errors in Info Panel `(#439) <https://github.com/AutoFlowResearch/SmartPeak/pull/439>`_

Fix
~~~
- Fixed computation of merge injections `(#434) <https://github.com/AutoFlowResearch/SmartPeak/pull/434>`_
- Fix DDA parameters `(#435) <https://github.com/AutoFlowResearch/SmartPeak/pull/435>`_
- Statistics Widget: display message if no sample/transition is selected `(#438) <https://github.com/AutoFlowResearch/SmartPeak/pull/438>`_
- Min/Max zooming sliders issues `(#440) <https://github.com/AutoFlowResearch/SmartPeak/pull/440>`_
- Fixed export of PivotTable `(#444) <https://github.com/AutoFlowResearch/SmartPeak/pull/444>`_
- Fix export of Pivot Table `(#445) <https://github.com/AutoFlowResearch/SmartPeak/pull/445>`_
- Fix/select dilution `(#446) <https://github.com/AutoFlowResearch/SmartPeak/pull/446>`_

v1.15.0 (2021-10-11)
--------------------

Features
~~~~~~~~
- "Select All" button when generating Report `(#427) <https://github.com/AutoFlowResearch/SmartPeak/pull/427>`_
- Added options to SmartPeakCLI `(#429) <https://github.com/AutoFlowResearch/SmartPeak/pull/429>`_
    - --input-file (-f): Override input file.
    - --parameter (-p): Override parameter.
    - --mzml (-z): Overrides the default location for the mzML directory.
    - --reports-out-dir (-ro): Overrides the default location for reports.
    - --output-features (-o): Overrides the default location for output features.
    - --input-features (-i): Overrides the default location for input features.
- Create Report: Pivotable and FeatureDB button to use FilePickerHandler `(#431) <https://github.com/AutoFlowResearch/SmartPeak/pull/431>`_

Fix
~~~
- Fixed number of transitions does not appear after loading `(#430) <https://github.com/AutoFlowResearch/SmartPeak/pull/430>`_

Changes
~~~~~~~
- Changed options to SmartPeakCLI `(#429) <https://github.com/AutoFlowResearch/SmartPeak/pull/429>`_
    - --output (-o) is now replaced by either --output-features or --reports-out-dir
    - --disable-progressbar short form has been replaced from -p to -pg
    - --integrity short form has been replaced from -i to -t

v1.14.0 (2021-09-30)
--------------------

Features
~~~~~~~~
- Implementation of user sessions `(#423) <https://github.com/AutoFlowResearch/SmartPeak/pull/423>`_

Fix
~~~
- Fixed some metadata not appearing in PivotTable `(#424) <https://github.com/AutoFlowResearch/SmartPeak/pull/424>`_
- Fixed failure to load featureQCComponents.csv when featureQCComponentGroups.csv is not provided `(#423) <https://github.com/AutoFlowResearch/SmartPeak/pull/423>`_

v1.13.0 (2021-09-13)
--------------------

Features
~~~~~~~~
- SmartPeak support for exporting workflow results as plots `(#412) <https://github.com/AutoFlowResearch/SmartPeak/pull/412>`_
- Improving Feature Table `(#385) <https://github.com/AutoFlowResearch/SmartPeak/pull/385>`_
- Multithreading options for SequenceProcessor methods `(#401) <https://github.com/AutoFlowResearch/SmartPeak/pull/401>`_
 
Fix
~~~
- Fixed sorted data for Calibrator plot. `(#413) <https://github.com/AutoFlowResearch/SmartPeak/pull/413>`_
- Fixed Pivot table header with component name and groups `(#410) <https://github.com/AutoFlowResearch/SmartPeak/pull/410>`_

v1.12.0 (2021-08-13)
--------------------

Features
~~~~~~~~
- Colouring logs `(#406) <https://github.com/AutoFlowResearch/SmartPeak/pull/406>`_

Changes
~~~~~~~
- Chromatogram's time-ranger in compact form `(#397) <https://github.com/AutoFlowResearch/SmartPeak/pull/397>`_  
- CI: moved to python 3.7.2 to fix a change in typing module `(#400) <https://github.com/AutoFlowResearch/SmartPeak/pull/400>`_  
- Enhancing issue filing usability `(#403) <https://github.com/AutoFlowResearch/SmartPeak/pull/403>`_  
- Rework of Filenames class `(#404) <https://github.com/AutoFlowResearch/SmartPeak/pull/404>`_  
 
Fix
~~~
- Fixed flooding of log when workflow fails to be loaded `(#399) <https://github.com/AutoFlowResearch/SmartPeak/pull/399>`_
- Fixed handling of windows separators in CLI. `(#402) <https://github.com/AutoFlowResearch/SmartPeak/pull/402>`_
- Fixed ProcessSequence Fails with SIGABRT `(#398) <https://github.com/AutoFlowResearch/SmartPeak/pull/398>`_

v1.11.0 (2021-07-16)
--------------------

Features
~~~~~~~~
- DDA Workflow and Support for visualization of non-targeted LC-MS/MS data `(#386) <https://github.com/AutoFlowResearch/SmartPeak/pull/386>`_
- Automatic, accurate selection of data based on dilution and compound `(#394) <https://github.com/AutoFlowResearch/SmartPeak/pull/394>`_

v1.10.0 (2021-07-08)
--------------------

Fix
~~~
- Added headless CLI to package `(#391) <https://github.com/AutoFlowResearch/SmartPeak/pull/391>`_
- Fixed show-back of log and info panel, once previously hidden. `(#389) <https://github.com/AutoFlowResearch/SmartPeak/pull/389>`_
- Display error if required column is missing in the parameters file. `(#388) <https://github.com/AutoFlowResearch/SmartPeak/pull/388>`_

v1.9.0 (2021-07-07)
-------------------

Features
~~~~~~~~
- SmartPeak headless mode. `(#365) <https://github.com/AutoFlowResearch/SmartPeak/pull/365>`_  

Fix
~~~
- Report error for invalid value in parameter.csv. `(#387) <https://github.com/AutoFlowResearch/SmartPeak/pull/387>`_

v1.8.0 (2021-07-01)
-------------------

Features
~~~~~~~~
- Added support of NA keyword in the sequence file. `(#376) <https://github.com/AutoFlowResearch/SmartPeak/pull/376>`_  

Changes
~~~~~~~
- Change testing framework to googletest `(#374) <https://github.com/AutoFlowResearch/SmartPeak/pull/374>`_  

Fix
~~~
- Fixed display of native_id for scans in explorer view `(#379) <https://github.com/AutoFlowResearch/SmartPeak/pull/379>`_
- Removing trailing zeros in FilePicker `(#377) <https://github.com/AutoFlowResearch/SmartPeak/pull/377>`_
- Fixed MergeSpectra when spectrum has no peak `(#382) <https://github.com/AutoFlowResearch/SmartPeak/pull/382>`_

v1.7.0 (2021-06-07)
-------------------

Features
~~~~~~~~
- Reworked FilePicker Handlers `(#371) <https://github.com/AutoFlowResearch/SmartPeak/pull/371>`_  

Fix
~~~
- Fixed crash when loading multiple sequences `(#373) <https://github.com/AutoFlowResearch/SmartPeak/pull/373>`_
- Fixed set path in FilePicker `(#372) <https://github.com/AutoFlowResearch/SmartPeak/pull/372>`_

v1.6.0 (2021-05-25)
-------------------

Features
~~~~~~~~
- Display progress information `(#367) <https://github.com/AutoFlowResearch/SmartPeak/pull/367>`_  
- Ability to export Sequence `(#368) <https://github.com/AutoFlowResearch/SmartPeak/pull/368>`_  
- Advanced explorer views part 2 `(#344) <https://github.com/AutoFlowResearch/SmartPeak/pull/344>`_  

  * Adding search functionality to ParametersTable
  * Sorting functionalities for ParametersTable
  * Plot-Stepper for chromatograms and feature line plots
  * Plot/Unplot All
  * App wide tooltip info
  * Adding quick help for various UI elements

Fix
~~~
- Fixing Crashes associated with `getPathnameContent` when starting GUI on macOS `(#279) <https://github.com/AutoFlowResearch/SmartPeak/pull/379>`_
- Fixed crash when updating parameters list `(#369) <https://github.com/AutoFlowResearch/SmartPeak/pull/368>`_

v1.5.0 (2021-05-07)
-------------------

Features
~~~~~~~~
- Ability to export Parameters `(#360) <https://github.com/AutoFlowResearch/SmartPeak/pull/360>`_  

Changes
~~~~~~~
- Compilation now uses C++17 standard.

Fix
~~~
- Fixed too much points to be displayed on Chromatograms `(#358) <https://github.com/AutoFlowResearch/SmartPeak/pull/358>`_
- Fixed crash on Linux when opening Transitions Tab `(#364) <https://github.com/AutoFlowResearch/SmartPeak/pull/364>`_

v1.4.0 (2021-04-26)
-------------------

Features
~~~~~~~~
- Enabling CPP Linting for SmartPeak `(#343) <https://github.com/AutoFlowResearch/SmartPeak/pull/343>`_  
- Enhanced error message when loading sequence fails `(#351) <https://github.com/AutoFlowResearch/SmartPeak/pull/351>`_  
  
Changes
~~~~~~~
- Feat/aut722 Rework for a more modular GUI `(#355) <https://github.com/AutoFlowResearch/SmartPeak/pull/355>`_  

  * Moved Run Workflow window in its own widget
  * Moved About dialog box in its own widget
  * Moved Log into its own widget
  * Added `visible_` flag in Widget
  * Moved SetWindowsize in Widget
  * Made CalibratorsPlotWidget and LinePlot2DWidgets retainable.
  * Report: removed `draw_` and use Widget's `visible_` instead
  * RunworkflowWidget is resizable
  
Fix
~~~
- Fixed compilation with OpenMS, related with Parameters changes `(#357) <https://github.com/AutoFlowResearch/SmartPeak/pull/357>`_
- FIAMS_FullScan_Unknown_test.exe assertion failure `(#348) <https://github.com/AutoFlowResearch/SmartPeak/pull/348>`_

v1.3.0 (2021-04-09)
-------------------

Features
~~~~~~~~
- Advanced Explorer Views 1 `(#324) <https://github.com/AutoFlowResearch/SmartPeak/pull/324>`_

  * Advanced Explorer Views - adding sorting functionality and viewing of File Name
  * Updated about popup [skip ci]
  * Adding searching functionality
  * Sorting and searching for injection/transition/feature tables
  * Selective searching for tables with dropdown menu
  * Disallow sorting for all-checked/all-unchecked columns
  * Sorting and searching for various tables in main window
  * Final enhancements to FilePicker, ExplorerTables and MainTables
  * Preventing filenames from being appended twice when opening via button click
  * Adding reusable table functions for searching/sorting
  * Adding unittest for Widget

- Make parameters editable `(#337) <https://github.com/AutoFlowResearch/SmartPeak/pull/337>`_  
- Enhancing FilePicker entries to be displayed in human readable format `(#339) <https://github.com/AutoFlowResearch/SmartPeak/pull/339>`_
- Added Statistics and more informations in the info panel `(#341) <https://github.com/AutoFlowResearch/SmartPeak/pull/341>`_
- Added SmartPeak documentation
- Adding working examples for calculating MDVs `(#326) <https://github.com/AutoFlowResearch/SmartPeak/pull/326>`_  

Changes
~~~~~~~
- Refactor of Feature line and heatmap plots `(#349) <https://github.com/AutoFlowResearch/SmartPeak/pull/349>`_  
  
Fix
~~~
- Fix formatting of changelog for emails and github releases `(#340) <https://github.com/AutoFlowResearch/SmartPeak/pull/340>`_
- Fixed crash when launching workflow with no injection `(#332) <https://github.com/AutoFlowResearch/SmartPeak/pull/332>`_
- Fixed Windows SmartPeak install: executable is not available from start menu `(#338) <https://github.com/AutoFlowResearch/SmartPeak/pull/338>`_
  
  * Add cmake nsis configuration for creating application links
  * Add icon to executable on windows
  * Update cmake nsis config to include icon to installer and launch the application when installation complete
  * Configure default CPACK_GENERATOR for windows

v1.2.0 (2021-03-15)
-------------------

Features
~~~~~~~~
- Provide smartpeak tool python package for continuous integration. `(#324) <https://github.com/AutoFlowResearch/SmartPeak/pull/324>`_
- Peak picking for MS2 RawDataProcessor method. `(#315) <https://github.com/AutoFlowResearch/SmartPeak/pull/315>`_
- Include/Pass version tag to SmartPeak code. `(#320) <https://github.com/AutoFlowResearch/SmartPeak/pull/320>`_
  
  * Add `Utilities::getSmartPeakVersion()` static method.
  * Include version information in gui and logs.
  
- Description of the workflow steps displayed to the user. `(#321) <https://github.com/AutoFlowResearch/SmartPeak/pull/321>`_

  * Description of the workflow steps displayed to the user.
  * Disabled edition when workflow is running. Update application handler in main thread when workflow finished to run.
  
- Add getID, getName and getDescription to SequenceProcessor `(#304) <https://github.com/AutoFlowResearch/SmartPeak/pull/304>`_
- Ability to save and load previous workflows. `(#307) <https://github.com/AutoFlowResearch/SmartPeak/pull/307>`_

  * Move commands to `SequenceHandlers`, remove the workflow modal dialog. Save/Load in menu.
  * Remove unused format check.
  * Improve error handling, avoid supplicate creations of commands.
  * Use `BuildCommandsFromNames`, fixed const signed parameter.

- Implementation of `SpectraPlotWidget`, split Widgets in their own files.
- SmartPeak support for isotopic labeling experiments. `(#283) <https://github.com/AutoFlowResearch/SmartPeak/pull/283>`_

  * Fix single entries not shown w/ file Picker.
  * Parse `isotopic_purity_values` from a dedicated field.
  * Add param for `CalculateMDVAccuracies`.
  * Finalise SmartPeak support for isotopic labeling experiments.
  * Update `TRAML` file with SumFormula.
  * Update `RawDataProcessor` with modified `OpenMS::IsotopeLabelingMDVs`.

- Advanced plotting. `(#298) <https://github.com/AutoFlowResearch/SmartPeak/pull/298>`_

  * Fix Sliders. Disable/Enable legend. Lines and scatter use same colors.
  * More stability to display while running workflow.
  * Sliders along with legend checkbox are now part of the chromatogram widget.
  * Add compact view.
  
- Allow for resizing the GUI bottom, left and upper panes. `(#297) <https://github.com/AutoFlowResearch/SmartPeak/pull/297>`_
- Signing SmartPeak with DTU-issued certificate. `(#296) <https://github.com/AutoFlowResearch/SmartPeak/pull/296>`_
- Add exporter to GUI and remove previously generated sequences in examples data. `(#288) <https://github.com/AutoFlowResearch/SmartPeak/pull/288>`_

  * Add `Xcalibur` writer to sequence parser.
  * Add new application processor for `XCalibur`.

- Add `makeSequenceFileMasshunter`. `(#287) <https://github.com/AutoFlowResearch/SmartPeak/pull/287>`_

  * Fix bug in directory and add to the gui export menu.

- Add method for making and writing an analyst sequence file along with tests. `(#285) <https://github.com/AutoFlowResearch/SmartPeak/pull/285>`_

  * Add exporter to the GUI.
  * Change member naming convention in SequenceProcessor classes.
  * Change filenames to use an explicit param for the mzML filename using the filename member of the raw data metadata.
  * Change sequenceParser to insert the injection name if no original filename value is provided.
  * Update to SequenceParser to reflect change in filenaming semantics for mzML.

- Add addition of filename to featureMap when loading the featureMap. `(#294) <https://github.com/AutoFlowResearch/SmartPeak/pull/294>`_


Changes
~~~~~~~
- Switch to ImGui v1.81 Release. `(#328) <https://github.com/AutoFlowResearch/SmartPeak/pull/328>`_
- MergeFeatures `(#295) <https://github.com/AutoFlowResearch/SmartPeak/pull/295>`_

  * Refactor `SearchAccurateMass` into two methods for searching the accurate mass and the other for making the consensus features from the individual adducts.
  * Change weighted average to sum in `MakeConsensusFeatures`.
  * Change `FeatureMap` creation step of Search accurate mass to make subordinates instead of features for compatibility with downstream quantitation and filtering.
  * Rename `MakeConsensusFeatures` to `MergeFeatures` and updated the FIA-MS default workflow.
  * Update `MergeInjections` to set the subordinate even merging at the Feature level.

- Migrate all `ImGui::columns` to `ImGui::tables` in the GUI. `(#286) <https://github.com/AutoFlowResearch/SmartPeak/pull/286>`_

  * Switch to imgui v1.79, Boost to 1.73.
  * Various adjustments to the current ImGui tables API.
  * Resolve fatal error: reference is not a tree when checking out ImGui's specific commit.
  * `ImGui::Tables` - removing inner borders in FilePicker.

- Update `ImGui::Tables` API. `(#281) <https://github.com/AutoFlowResearch/SmartPeak/pull/281>`_

  * Append missing include for `OpenSwathAlgo`.
  * Update GUI to latest ImGui API tables.

- Refresh parameters table only when parameters change. `(#317) <https://github.com/AutoFlowResearch/SmartPeak/pull/317>`_

  * Refresh parameters table only when user parameters or workflow change
  * Use notification to update the view.
  * Move `ParametersTableWidget` to its own File.
  * Move observers to sequenceHandlers.

- Refactor handling user parameters `(#302) <https://github.com/AutoFlowResearch/SmartPeak/pull/302>`_

  * Parameter do not use anymore map/vectors/maps, but ParameterSet, container of FunctionParameter, containers of Parameter.
  * It's still possible to initialize with the old structure of map/vector/map.
  * Parameter can be initialized from an OpenMS Parameter (to create the schema).
  * Parameter can be assigned to another Parameter, the schema, allowing validation.
  * Commands return the list of Parameter they need to run (getParameterSchema()), allowing validation when setting up the workflow, not when running it.
  * Parameter Panel show different colors: user overridden parameters, the default parameters (schema), and the unused (not appearing from the schema).
  * Parameter Panel colors invalid values in red. Mouse over it shows some indication: constraint, expected Type.


Fix
~~~
- Fix `Chromatogram` stops updating after having selected all plots.
- Not specifying some user parameters will not prevent processors to run. `(#329) <https://github.com/AutoFlowResearch/SmartPeak/pull/329>`_
- Fix `HPLC UV Unknowns` preset. `(#325) <https://github.com/AutoFlowResearch/SmartPeak/pull/325>`_
- Log not written when GUI is launched from folder that requires Admin privileges to write to `(#301) <https://github.com/AutoFlowResearch/SmartPeak/pull/301>`_

  * Add static method `SmartPeak::Utilities::getLogFilepath()` for dynamic path to log.
  * Redirect logging path for GUI.
  * Handle errors and log message to console.

- Fixed chromatogram and spectra range reset when selecting different components. `(#314) <https://github.com/AutoFlowResearch/SmartPeak/pull/314>`_
- File name not shown when selected. `(#312) <https://github.com/AutoFlowResearch/SmartPeak/pull/312>`_
  
  * Displaying selected file name in the designated field & adding double-click-to-open feature.
  * Add possibility to create new file from FilePicker.
  
- Extra warnings in FIA-MS workflow. `(#311) <https://github.com/AutoFlowResearch/SmartPeak/pull/311>`_
  
  * Prioritize the use of subordinate metadata when choosing between feature level and subordinate level.
  * Add extra logging to warn the user when the extract_spectra step for FIA-MS fails due to missing the RT that the spectra was acquired.
  
- Fixed Compilation issue with Commands, set LoadRawData parameter constraint to "ChromeleonFile" instead of "Chromeleon". `(#308) <https://github.com/AutoFlowResearch/SmartPeak/pull/308>`_
- Exception handling in QuantitationMethods. `(#306) <https://github.com/AutoFlowResearch/SmartPeak/pull/306>`_
  
  * Add additional exception to catch misc OpenMS exceptions that interupt the workflow.

- Changed racked code to match ideosyncrocy in numbers less than 10.
- Replaced sample_name with injection_name in Analyst conversion.
- Bug in HPLC data processing `(#284) <https://github.com/AutoFlowResearch/SmartPeak/pull/284>`_
  
  * Artificially scale the chromatograms when loading HPLC data derived from a .txt file.

- Small/minor aesthetic fix to tables. `(#282) <https://github.com/AutoFlowResearch/SmartPeak/pull/282>`_
- SequenceSegmentProcess exporters. `(#280) <https://github.com/AutoFlowResearch/SmartPeak/pull/280>`_

  * Update Estimation methods to use filter templates as the template.
  * Bad values copy in EstimateFeaturesRSD.
  * Add extra debug info to SelectFeatures.
  * Add sections in Filename for new store/load pairs in sequenceSegmentProcessor.
  * Add new store/load pairs to sequenceSegmentProcessor for estimations of RSDs and Background.
  * Update SharedProcessors with new sequenceSegmentProcessor store/load pairs.
  * Bad GC-MS full scan workflow presets.
  * Add views for filter and qc tables to the SessionHandler.


Other
~~~~~
- Refactor initialization of shared pointers.
- Set FeatureMetadataSize dynamically.
- Update SessionHandler_test for the new entries in metadataToString and metadataToString.
- Update metadataToString and metadataToString with average_accuracy & absolute_difference.


v1.1a (2020-08-30)
------------------

Features
~~~~~~~~
- Add support for computing more accurate peak area metrics that will be used for downstream analysis and filtering in FIA-MS to pickms1features.
- Add consensus calculations to `SearchAccurateMass`.
- Add support for sample group handler in add sample to sequence.
- Add `SampleGroupProcessor`.
- Add `MergeInjections` with test for the case of subordinates.
- Add option to record the convex hull during `PickMS1Features`.
- Add hull points and best left/right widths to `PickMS1Features`.
- Add support for spectra explorer.
- Support for plotting spectra and additional support for defining the ranges of chromatograms and spectra as well as plotting features wtihout raw data and vice versa.
- Example data and preliminary code for FIA-MS workflow.
- Add documentation for ReadtheDocs with Sphinx/Doxygen.
- Working NSIS setup but it is missing all of the dependency libraries and executables.
- Implement `SequenceProcessorMultithread`.
- Top and bottom windows follow host's window resize.'
- Integrate recent changes from AppWindow to GUI: Changes to AppWindow were lost after the AppWindow files removal.
- Ask for pathnames before running workflow.
- Implement class `WorkflowManager`: workflow is copied back to the main app.
- Clear FilePicker's filter on entering a directory.
- Implement Info tab functionality, related to QuickInfo menu.
- Add widget Report to store a csv file with FeatureSummary or SequenceSummary.


Changes
~~~~~~~
- Update FIAMS parameters and TraML templates.
- Packaging for macOS (dmg), Linux (deb), Windows (exe), CI for macOS and CI for Windows.
- Add support for feature level feature table and matrix reporting along with optimization of feature table and feature matrix updating in the GUI.
- Update `PickMS1Features` and `SearchAccurateMass` so that all necessary feature metadata needed for downstream filtering, QC, and analytics is captured in the `FeatureMap` and saved to the `FeatureMapHistory`.

Fix
~~~
- Prioritize the use of subordinate metadata when choosing between feature level and subordinate level.
- Expand `MergeInjections` tests and isolated/corrected several bugs.
- Major bugs in `SampleGroupProcessor`; Add `makeFeatureMapFromHistory` for better handling of input FeatureXML files.
- Bug in `UpdateFEatureMapHistory`.
- Bug in `mergeInjections` where missing injection data caused the feature to be removed prematurely.
- Bug in `FeatureMatrix`.
- Bug in filtering by injection name or sequence segment name.
- Add options for running sequence processor with specified injections or segments.
- EMG processor and app state fixes.
- About window not using a popup/modal because it relies on hacks/workarounds.


v1.0a (2020-07-16)
------------------

Features
~~~~~~~~
- GUI can load a session from a sequence.
- Implement processor `LoadSessionFromSequence`.
- Add `FilePicker`
- Add `AppStateProcessor` logic to `FilePicker`
- `getPathnameContent` uses class Table, fetches name, size, type and date of entries
- File type filter.
- Navigate directories, show their content.
- Add `AppState` to `AppWindow`.
- Drag and Drop steps.
- Add Workflow widget to the application.
- Add processor class `BuildCommandsFromIds` to `AppStateProcessor`.
- Use single-pane navigation.
- Add "go up" directory button.
- Do not insert "." and ".." to folder content.
- Navigate directories, show their content.
- Add to gui: `Report`, `Run workflow`.
- Add validation check on Report checkboxes. At least one of both column should be selected.
- Add widget `Report` to store a csv file with `FeatureSummary` or `SequenceSummary`.
- Add Workflow widget to the application.
- Implement class `GuiAppender`.
- CLI and pivot export.
- Load files in a separate thread, and disable buttons until it's done.
- Add `EMGProcessor`.
- Add in features line and heatmap plots.
- Feature explorer pane.
- CLI can export integration start/end for FeatureSummary.csv and SequenceSummary.csv.

Changes
~~~~~~~
- Install dependency `libboost-filesystem-dev` apt package.
- Improve `AppStateProcessor` hierarchy, remove unnecessary structs, make them private methods.
- `AppStateProcessor`'s methods become structs/classes.
- Separate CLI functionalities into `AppState` and `AppStateProcessor`.
- `AppState` does not keep info about selected metadata and sample types.
- Simplify CLI menu entries.

Fix
~~~
- Implicit conversion from char to `plog::util::nstring`.
- Indentation in `AppWindow::HelpMarker()`.
- Avoid shadowing menu item with text below/under (level, z-index wise) it in workflow steps. Use Button instead of Text.
- `FilePicker` does not call OpenPopup at every frame.
- Add support for OpenMS shared data and removing fixed path names.
- Enable selecting all metadata in CLI app,
- Main arguments in GUI.
- Cmake configuration for OpenGL (Linux).
- CommandLine: reset fetures and raw data directories after loading a new sequence file.


v0.1.0 (2019-04-08)
-------------------

First release of SmartPeak
