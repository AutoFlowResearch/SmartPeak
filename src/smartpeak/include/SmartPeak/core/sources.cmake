### the directory name
set(directory include/SmartPeak/core)

### list all header files of the directory here
set(sources_list_h
	CastValue.h
	Filenames.h
	Helloworld.h
	MetaDataHandler.h
	RawDataHandler.h
	RawDataProcessor.h
	SampleGroupHandler.h
	InjectionHandler.h
	SequenceHandler.h
	SequenceProcessor.h
	SequenceSegmentHandler.h
	SequenceSegmentProcessor.h
	Table.h
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

