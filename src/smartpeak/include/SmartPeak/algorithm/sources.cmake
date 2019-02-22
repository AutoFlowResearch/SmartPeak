### the directory name
set(directory include/SmartPeak/algorithm)

### list all header files of the directory here
set(sources_list_h
	MRMFeatureValidator.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\algorithm" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

