SmartPeak Changelog
===================


v1.2.0 (2020-03-15)
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
