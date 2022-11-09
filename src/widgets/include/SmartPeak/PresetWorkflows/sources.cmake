### the directory name
set(directory include/SmartPeak/core/PresetWorkflows)

### list all header files of the directory here
set(sources_list_h
	PresetWorkflow.h
	FIAMS_Unknowns.h
	GCMS_Full_Scan_Unknowns.h
	GCMS_SIM_Unknowns.h
	HPLC_UV_Standards.h
	HPLC_UV_Unknowns.h
	LCMS_DDA_Spectra_Library_Construction.h
	LCMS_DDA_Spectra_Library_Matching.h
	LCMS_DDA_Transitions_Library_Construction.h
	LCMS_MRM_Standards.h
	LCMS_MRM_Unknowns.h
	LCMS_MRM_Validation_LP.h
	LCMS_MRM_Validation_QMIP.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\core\\PresetWorkflows" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

