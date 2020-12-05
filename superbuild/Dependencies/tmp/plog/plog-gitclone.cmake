
if(NOT "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Stamp/plog/plog-gitinfo.txt" IS_NEWER_THAN "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Stamp/plog/plog-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Stamp/plog/plog-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/plog"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/plog'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  clone --no-checkout "https://github.com/SergiusTheBest/plog.git" "plog"
    WORKING_DIRECTORY "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/SergiusTheBest/plog.git'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"  checkout 5f69a631086bf038e2c0feee4319a485c34fa863 --
  WORKING_DIRECTORY "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/plog"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: '5f69a631086bf038e2c0feee4319a485c34fa863'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  submodule update --recursive --init 
    WORKING_DIRECTORY "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/plog"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Source/plog'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Stamp/plog/plog-gitinfo.txt"
    "C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Stamp/plog/plog-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/Users/dmccloskey/Documents/GitHub/SmartPeak/superbuild/Dependencies/Stamp/plog/plog-gitclone-lastrun.txt'")
endif()

