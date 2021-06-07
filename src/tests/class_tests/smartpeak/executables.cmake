set(core_executables_list
	ApplicationManager_test
	ApplicationHandler_test
	ApplicationProcessor_test
	ApplicationSettings_test
	CastValue_test
	ConsoleHandler_test
	EventDispatcher_test
	Filenames_test
	Helloworld_test
	ImEntry_test
	InjectionHandler_test
	MetaDataHandler_test
	Parameters_test
	ParametersObservable_test
	ProgressInfo_test
	RawDataHandler_test
	RawDataProcessor_test
	SampleGroupHandler_test
	SampleGroupProcessor_test
	SequenceHandler_test
	SequenceProcessor_test
	SequenceSegmentHandler_test
	SequenceSegmentProcessor_test
	SessionHandler_test
	Task_test
	Utilities_test
	WorkflowObservable_test
)

set(io_executables_list
	CSVWriter_test
	ParametersParser_test
	SequenceParser_test
	InputDataValidation_test
)

set(algorithm_executables_list
	MRMFeatureValidator_test
)

set(ui_executables_list
	GuiAppender_test
	WindowSizesAndPositions_test
  Widget_test
)

### collect test executables
set(TEST_executables
	${core_executables_list}
	${io_executables_list}
	${algorithm_executables_list}
	${ui_executables_list}
)
