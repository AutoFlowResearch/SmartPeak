set(core_executables_list
	SequenceProcessor_test
	Task_test
)

set(io_executables_list
	CSVWriter_test
	ParametersParser_test
	PlotExporter_test
	SelectDilutionsParser_test
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
