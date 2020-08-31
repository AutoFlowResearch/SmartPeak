### the directory name
set(directory include/SmartPeak/pipelines)

### list all header files of the directory here
set(sources_list_h
	FIAMS_FullScan_Unknown_example.h
	GCMS_SIM_Unknown_example.h
	HPLC_UV_Standards_example.h
	HPLC_UV_Unknown_example.h
	LCMS_MRM_Standards_example.h
	LCMS_MRM_Unknown_example.h
	LCMS_MRM_Validation_example.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\pipelines" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

