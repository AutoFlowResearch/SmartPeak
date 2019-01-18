### the directory name
set(directory include/SmartPeak/core)

### list all header files of the directory here
set(sources_list_h
	Filenames.h
	Helloworld.h
	LCMS_MRM_Standards_example.h
	LCMS_MRM_Unknown_example.h
	LCMS_MRM_Validation_example.h
	MetaDataHandler.h
	MRMFeatureValidator.h
	RawDataHandler.h
	RawDataProcessor.h
	SampleGroupHandler.h
	SampleHandler.h
	SequenceHandler.h
	SequenceProcessor.h
	SequenceSegmentHandler.h
	SequenceSegmentProcessor.h
	Utilities.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\core" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

