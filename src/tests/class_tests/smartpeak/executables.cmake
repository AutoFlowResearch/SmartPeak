set(core_executables_list
	ApplicationProcessor_test
	CastValue_test
	Filenames_test
	Helloworld_test
	MetaDataHandler_test
	RawDataHandler_test
	RawDataProcessor_test
	SampleGroupHandler_test
	SampleGroupProcessor_test
	InjectionHandler_test
	SequenceHandler_test
	SequenceProcessor_test
	SequenceSegmentHandler_test
	SequenceSegmentProcessor_test
	SessionHandler_test
	Utilities_test
	Parameters_test
)

set(io_executables_list
	CSVWriter_test
	FileReader_test
	SequenceParser_test
	InputDataValidation_test
)

set(algorithm_executables_list
	MRMFeatureValidator_test
)

set(ui_executables_list
	GuiAppender_test
	WindowSizesAndPositions_test
)

### collect test executables
set(TEST_executables
	${core_executables_list}
	${io_executables_list}
	${algorithm_executables_list}
	${ui_executables_list}
)
