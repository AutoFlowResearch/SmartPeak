set(core_executables_list
	Helloworld_test
	MetaDataHandler_test
	MRMFeatureValidator_test
	RawDataHandler_test
	RawDataProcessor_test
	SampleGroupHandler_test
	InjectionHandler_test
	SequenceHandler_test
	SequenceProcessor_test
	SequenceSegmentHandler_test
	SequenceSegmentProcessor_test
	Utilities_test
)

set(io_executables_list
	CSVWriter_test
	FileReader_test
	OpenMSFile_test
	SequenceParser_test
	InputDataValidation_test
)

set(simulator_executables_list
	ChromatogramSimulator_test
	EMGModel_test
	PeakSimulator_test
)

### collect test executables
set(TEST_executables
	${core_executables_list}
	${io_executables_list}
	${simulator_executables_list}
)
