set(core_executables_list
	Helloworld_test
	RawDataHandler_test
	SampleGroupHandler_test
)

set(io_executables_list
	CSVWriter_test
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
