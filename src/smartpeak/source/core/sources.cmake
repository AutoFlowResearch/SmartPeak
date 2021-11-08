### the directory name
set(directory source/core)

### list all filenames of the directory here
set(sources_list
	ApplicationHandler.cpp
	ApplicationProcessor.cpp
	CastValue.cpp
	ConsoleHandler.cpp
	EventDispatcher.cpp
	FeatureFiltersUtils.cpp
	FeatureMetadata.cpp
	Filenames.cpp
	Helloworld.cpp
	InjectionHandler.cpp
	MetaDataHandler.cpp
	Parameters.cpp
	ProgessInfo.cpp
	RawDataHandler.cpp
	RawDataProcessor.cpp
	SampleGroupHandler.cpp
	SampleGroupProcessor.cpp
	SampleType.cpp
	SequenceHandler.cpp
	SequenceProcessor.cpp
	SequenceSegmentHandler.cpp
	SequenceSegmentProcessor.cpp
	SessionHandler.cpp
	SessionLoaderGenerator.cpp
	SharedProcessors.cpp
	Utilities.cpp
	WorkflowManager.cpp
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

