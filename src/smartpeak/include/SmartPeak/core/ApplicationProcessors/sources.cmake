### the directory name
set(directory include/SmartPeak/core/ApplicationProcessors)

### list all header files of the directory here
set(sources_list_h
	BuildCommandsFromNames.h
	CreateCommand.h
	LoadFilenames.h
	LoadPropertiesHandlers.h
	LoadSession.h
	SavePropertiesHandlers.h
	SaveSession.h
	sources.cmake
	StoreFilenames.h
)

### add path to the filenames
set(sources_h)
foreach(i ${sources_list_h})
	list(APPEND sources_h ${directory}/${i})
endforeach(i)

### source group definition
source_group("Header Files\\SmartPeak\\core\\ApplicationProcessors" FILES ${sources_h})

set(SmartPeak_sources_h ${SmartPeak_sources_h} ${sources_h})

