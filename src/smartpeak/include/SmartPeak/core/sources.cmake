### the directory name
set(directory include/SmartPeak/core)

### list all header files of the directory here
set(sources_list_h
	ApplicationHandler.h
	ApplicationProcessor.h
	ApplicationProcessorObservable.h
	CastValue.h
	ConsoleHandler.h
	EventDispatcher.h
	Filenames.h
	FeaturesObservable.h
	FeatureFiltersUtils.h
	FeatureFiltersUtilsMode.h
	FeatureMetadata.h
	Helloworld.h
	InjectionHandler.h
	LibraryObservable.h
	MetaDataHandler.h
	Parameters.h
	ParametersObservable.h
	ProgressInfo.h
	RawDataHandler.h
	RawDataProcessor.h
	SampleGroupHandler.h
	SampleGroupProcessor.h
	SampleGroupProcessorObservable.h
	SampleType.h
	SequenceHandler.h
	SequenceObservable.h
	SequenceProcessor.h
	SequenceProcessorObservable.h
	SequenceSegmentHandler.h
	SequenceSegmentObservable.h
	SequenceSegmentProcessor.h
	SequenceSegmentProcessorObservable.h
	SessionHandler.h
	ServerAppender.h
	SessionLoaderGenerator.h
	SharedProcessors.h
	Server.h
	TransitionsObservable.h
	Utilities.h
	WorkflowManager.h
	WorkflowObservable.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\core" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

