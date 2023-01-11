### the directory name
set(directory include/SmartPeak/ui)

### list all header files of the directory here
set(sources_list_h
	AboutWidget.h
	CalibratorsPlotWidget.h
	ChromatogramPlotWidget.h
	ChromatogramTICPlotWidget.h
	ChromatogramXICPlotWidget.h
	CreateSessionWidget.h
	ExplorerWidget.h
	FilePicker.h
	GraphicDataVizWidget.h
	GuiAppender.h
	Heatmap2DWidget.h
	Help.h
	ImEntry.h
	InfoWidget.h
	LayoutLoader.h
	LoadSessionWizard.h
	LogWidget.h
	OptionsWidget.h
	ParameterEditorWidget.h
	ParametersTableWidget.h
	Plotter.h
	PresetWorkflowWidget.h
	Report.h
	RunWorkflowWidget.h
	SampleTypeEditorWidget.h
	SequenceGroupsEditorWidget.h
	SequenceTableWidget.h
	SessionFilesWidget.h
	SetInputOutputWidget.h
	SpectraMSMSPlotWidget.h
	SpectraPlotWidget.h
	SplitWindow.h
	StatisticsWidget.h
	UIUtilities.h
	Widget.h
	WindowSizesAndPositions.h
	WorkflowWidget.h
	WorkflowStepWidget.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\ui" FILES ${sources_h})

set(SmartPeakWidgets_sources_h ${SmartPeakWidgets_sources_h} ${sources_h})
