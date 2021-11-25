### the directory name
set(directory source/core/SequenceSegmentProcessors)

### list all filenames of the directory here
set(sources_list
	CalculateCalibration.cpp
	EstimateFeatureBackgroundInterferences.cpp
	EstimateFeatureFilterValues.cpp
	EstimateFeatureQCValues.cpp
	EstimateFeatureRSDs.cpp
	LoadFeatureBackgroundEstimations.cpp
	LoadFeatureBackgroundFilters.cpp
	LoadFeatureBackgroundQCs.cpp
	LoadFeatureFilters.cpp
	LoadFeatureQCs.cpp
	LoadFeatureRSDEstimations.cpp
	LoadFeatureRSDFilters.cpp
	LoadFeatureRSDQCs.cpp
	LoadQuantitationMethods.cpp
	LoadStandardsConcentrations.cpp
	StoreFeatureBackgroundEstimations.cpp
	StoreFeatureBackgroundFilters.cpp
	StoreFeatureBackgroundQCs.cpp
	StoreFeatureFilters.cpp
	StoreFeatureQCs.cpp
	StoreFeatureRSDEstimations.cpp
	StoreFeatureRSDFilters.cpp
	StoreFeatureRSDQCs.cpp
	StoreQuantitationMethods.cpp
	StoreStandardsConcentrations.cpp
	TransferLOQToFeatureFilters.cpp
	TransferLOQToFeatureQCs.cpp
)

### add path to the filenames
set(sources)
foreach(i ${sources_list})
	list(APPEND sources ${directory}/${i})
endforeach(i)

### pass source file list to the upper instance
set(SmartPeak_sources ${SmartPeak_sources} ${sources})

### source group definition
source_group("Source Files\\core\\SequenceSegmentProcessors" FILES ${sources})

