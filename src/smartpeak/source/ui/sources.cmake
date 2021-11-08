### the directory name
set(directory source/ui)

### list all filenames of the directory here
set(sources_list
	AboutWidget.cpp
	CalibratorsPlotWidget.cpp
	ChromatogramPlotWidget.cpp
	ChromatogramTICPlotWidget.cpp
	ChromatogramXICPlotWidget.cpp
	CreateSessionWidget.cpp
	ExplorerWidget.cpp
	FilePicker.cpp
	GraphicDataVizWidget.cpp
	GuiAppender.cpp
	Heatmap2DWidget.cpp
	ImEntry.cpp
	InfoWidget.cpp
	LoadSessionWizard.cpp
	LogWidget.cpp
	ParameterEditorWidget.cpp
	ParametersTableWidget.cpp
	Plotter.cpp
	Report.cpp
	RunWorkflowWidget.cpp
	SampleTypeEditorWidget.cpp
	SequenceGroupsEditorWidget.cpp
	SequenceTableWidget.cpp
	SessionFilesWidget.cpp
	SetInputOutputWidget.cpp
	SpectraMSMSPlotWidget.cpp
	SpectraPlotWidget.cpp
	SplitWindow.cpp
	StatisticsWidget.cpp
	Widget.cpp
	WindowSizesAndPositions.cpp
	WorkflowWidget.cpp
	WorkflowStepWidget.cpp
)

### add path to the filenames
set(sources)
foreach(i ${sources_list})
	list(APPEND sources ${directory}/${i})
endforeach(i)

### pass source file list to the upper instance
set(SmartPeak_sources ${SmartPeak_sources} ${sources})

### source group definition
source_group("Source Files\\ui" FILES ${sources})

