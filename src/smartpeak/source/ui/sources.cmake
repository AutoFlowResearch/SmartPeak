### the directory name
set(directory source/ui)

### list all filenames of the directory here
set(sources_list
	FilePicker.cpp
	GuiAppender.cpp
	Report.cpp
	Widget.cpp
	Workflow.cpp
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

