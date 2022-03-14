### the directory name
set(directory include/SmartPeak/core/SequenceSegmentProcessors)

### list all header files of the directory here
set(sources_list_h
	EstimateFeatureBackgroundInterferences.h
	EstimateFeatureFilterValues.h
	EstimateFeatureQCValues.h
	EstimateFeatureRSDs.h
	FitCalibration.h
	LoadFeatureBackgroundEstimations.h
	LoadFeatureBackgroundFilters.h
	LoadFeatureBackgroundQCs.h
	LoadFeatureFilters.h
	LoadFeatureQCs.h
	LoadFeatureRSDEstimations.h
	LoadFeatureRSDFilters.h
	LoadFeatureRSDQCs.h
	LoadQuantitationMethods.h
	LoadStandardsConcentrations.h
	OptimizeCalibration.h
	StoreFeatureBackgroundEstimations.h
	StoreFeatureBackgroundFilters.h
	StoreFeatureBackgroundQCs.h
	StoreFeatureFilters.h
	StoreFeatureQCs.h
	StoreFeatureRSDEstimations.h
	StoreFeatureRSDFilters.h
	StoreFeatureRSDQCs.h
	StoreQuantitationMethods.h
	StoreStandardsConcentrations.h
	TransferLOQToFeatureFilters.h
	TransferLOQToFeatureQCs.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\core\\SequenceSegmentProcessors" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

