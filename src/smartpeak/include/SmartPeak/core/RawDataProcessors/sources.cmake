### the directory name
set(directory include/SmartPeak/core/RawDataProcessors)

### list all header files of the directory here
set(sources_list_h
	CalculateIsotopicPurities.h
	CalculateMDVAccuracies.h
	CalculateMDVs.h
	CheckFeatures.h
	CheckFeaturesBackgroundInterferences.h
	CheckFeaturesRSDs.h
	ClearData.h
	ConstructTransitionsList.h
	DDA.h
	ExtractChromatogramWindows.h
	ExtractSpectraWindows.h
	FilterFeatures.h
	FilterFeaturesBackgroundInterferences.h
	FilterFeaturesRSDs.h
	FitFeaturesEMG.h
	IsotopicCorrections.h
	LoadAnnotations.h
	LoadFeatureFiltersRDP.h
	LoadFeatureQCsRDP.h
	LoadFeatures.h
	LoadParameters.h
	LoadRawData.h
	LoadTransitions.h
	LoadValidationData.h
	MapChromatograms.h
	MatchSpectra.h
	MergeFeatures.h
	MergeSpectra.h
	Pick2DFeatures.h
	Pick3DFeatures.h
	PickMRMFeatures.h
	PlotFeatures.h
	QuantifyFeatures.h
	SearchAccurateMass.h
	SearchSpectrum.h
	SelectFeatures.h
	StoreAnnotations.h
	StoreFeatureFiltersRDP.h
	StoreFeatureQCsRDP.h
	StoreFeatures.h
	StoreMSP.h
	StoreParameters.h
	StoreRawData.h
	StoreValidationData.h
	ValidateFeatures.h
	ZeroChromatogramBaseline.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\core\\RawDataProcessors" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

