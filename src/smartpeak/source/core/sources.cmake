### the directory name
set(directory source/core)

### list all filenames of the directory here
set(sources_list
	AppStateProcessor.cpp
	CastValue.cpp
	FeatureMetadata.cpp
	Filenames.cpp
	Helloworld.cpp
	MetaDataHandler.cpp
	RawDataHandler.cpp
	RawDataProcessor.cpp
	SampleGroupHandler.cpp
	InjectionHandler.cpp
	SampleType.cpp
	SequenceHandler.cpp
	SequenceProcessor.cpp
	SequenceSegmentHandler.cpp
	SequenceSegmentProcessor.cpp
	Utilities.cpp
)

### add path to the filenames
set(sources)
foreach(i ${sources_list})
	list(APPEND sources ${directory}/${i})
endforeach(i)

### pass source file list to the upper instance
set(SmartPeak_sources ${SmartPeak_sources} ${sources})

### source group definition
source_group("Source Files\\core" FILES ${sources})

