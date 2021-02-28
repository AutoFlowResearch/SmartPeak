SmartPeak Changelog
===================


v1.1a (2020-08-30)
------------------

New
~~~
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

New
~~~
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
