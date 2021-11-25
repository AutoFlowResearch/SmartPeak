### the directory name
set(directory source/core/RawDataProcessors)

### list all filenames of the directory here
set(sources_list
	CalculateIsotopicPurities.cpp
	CalculateMDVAccuracies.cpp
	CalculateMDVs.cpp
	CheckFeatures.cpp
	CheckFeaturesBackgroundInterferences.cpp
	CheckFeaturesRSDs.cpp
	ClearData.cpp
	DDA.cpp
	ExtractChromatogramWindows.cpp
	ExtractSpectraWindows.cpp
	FilterFeatures.cpp
	FilterFeaturesBackgroundInterferences.cpp
	FilterFeaturesRSDs.cpp
	FitFeaturesEMG.cpp
	IsotopicCorrections.cpp
	LoadAnnotations.cpp
	LoadFeatureFiltersRDP.cpp
	LoadFeatureQCsRDP.cpp
	LoadFeatures.cpp
	LoadParameters.cpp
	LoadRawData.cpp
	LoadTransitions.cpp
	LoadValidationData.cpp
	MapChromatograms.cpp
	MergeFeatures.cpp
	MergeSpectra.cpp
	PickMRMFeatures.cpp
	PickMS1Features.cpp
	PickMS2Features.cpp
	PlotFeatures.cpp
	QuantifyFeatures.cpp
	SearchAccurateMass.cpp
	SearchSpectrum.cpp
	SelectFeatures.cpp
	StoreAnnotations.cpp
	StoreFeatureFiltersRDP.cpp
	StoreFeatureQCsRDP.cpp
	StoreFeatures.cpp
	StoreParameters.cpp
	StoreRawData.cpp
	StoreValidationData.cpp
	ValidateFeatures.cpp
	ZeroChromatogramBaseline.cpp
)

### add path to the filenames
set(sources)
foreach(i ${sources_list})
	list(APPEND sources ${directory}/${i})
endforeach(i)

### pass source file list to the upper instance
set(SmartPeak_sources ${SmartPeak_sources} ${sources})

### source group definition
source_group("Source Files\\core\\RawDataProcessors" FILES ${sources})

