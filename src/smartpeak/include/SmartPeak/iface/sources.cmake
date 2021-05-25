### the directory name
set(directory include/SmartPeak/iface)

### list all header files of the directory here
set(sources_list_h
  IApplicationProcessorObserver.h
  IParametersObserver.h
  IProcessorDescription.h
  ISampleGroupProcessorObserver.h
  ISequenceObserver.h
  ISequenceProcessorObserver.h
  ISequenceSegmentObserver.h
  ISequenceSegmentProcessorObserver.h
  ITransitionsObserver.h
  IWorkflowObserver.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
  list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\iface" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})
