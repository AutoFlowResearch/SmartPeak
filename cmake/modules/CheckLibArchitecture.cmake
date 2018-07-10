
## Check if link libraries have right architecture (.lib can be either a static or link library for a dll)
## Motivation: if 32bit libs are accidentally linked against a 64bit SmartPeak, weird linker errors (symbol not found, etc) will appear.
##             These are really hard to track down, so we check in advance.
function(check_lib_architecture link_libs)

    if (MSVC)
      message(STATUS "  Checking architecture of link libraries ...")
      
      ## check for 'dumpbin.exe' which comes with every VS to extract architecture
      find_program(PROG_DUMPBIN "dumpbin" DOC "dumpbin allows to extract information from (link) libraries")
      if (NOT PROG_DUMPBIN)
        message(FATAL_ERROR "\n\nCould not find 'dumpbin.exe' (${PROG_DUMPBIN})! Please make sure it's in %PATH%.\n\n")
      endif()
      set(LIB_ARCH_TIMEOUT 3 CACHE STRING "Timeout in seconds for running dumpbin.exe to find lib architecture.")

      foreach(lib_file ${${link_libs}})
        if (${lib_file} MATCHES "\\.lib$") ## ends with '.lib'
          get_filename_component(LIB_VAR_N "${lib_file}" NAME)
          set(LIB_VAR "LIB_ARCH_CHECKED_${LIB_VAR_N}")
          if (${LIB_VAR} EQUAL 1)
            ## Architecture of ${LIB_VAR_N} was checked already. Force re-check by setting '${LIB_VAR}' to zero.
          else()
            set(${LIB_VAR} "0" CACHE STRING "Checking of link library ${LIB_VAR_N} architecture (32/64bit): 1 if done (will not be repeated), 0 otherwise.")
            
            ## call 'dumpbin.exe' on each lib
            execute_process(COMMAND "${PROG_DUMPBIN}" "/HEADERS" "${lib_file}"
                            RESULT_VARIABLE DUMP_RESULT
                            OUTPUT_VARIABLE PIPE_OUT_LIB
                            ERROR_VARIABLE PIPE_OUT_LIB
                            TIMEOUT ${LIB_ARCH_TIMEOUT}) ## three seconds should be enough to get the first lines, which suffice to know what we need; big libs may need much longer!
            ## do not check '${DUMP_RESULT} EQUAL 0' here since a timeout could set it to 'Process terminated due to timeout'
            ## get a shorter version for (faster) regex matching below
            string(LENGTH "${PIPE_OUT_LIB}" PIPE_OUT_LIB_LEN)
            if (${PIPE_OUT_LIB_LEN} LESS 601)
              set(PIPE_OUT_LIB_S "${PIPE_OUT_LIB}")
            else()
              string(SUBSTRING "${PIPE_OUT_LIB}" 0 600 PIPE_OUT_LIB_S)
            endif()
            ## check if substring is long enough
            if (NOT PIPE_OUT_LIB_S MATCHES "[Mm]achine")
              message(STATUS "  - Timeout before data could be acquired. Data was:\n'${PIPE_OUT_LIB_S}'\n--\nMake sure your machine is not too busy or increase 'LIB_ARCH_TIMEOUT'.")
            else()
              ## we expect consistent architectures, output can be
              ## 'Machine      : 8664 (x64)'
              ## or
              ## '8664 machine (x64)'
              ## (similar for x86)
              set(M_64bit "Machine      : 8664 \\(x64\\)|8664 machine \\(x64\\)")
              set(M_32bit "Machine      : 14C \\(x86\\)|14C machine \\(x86\\)")
              if ((SMARTPEAK_64BIT_ARCHITECTURE EQUAL 1) AND NOT (${PIPE_OUT_LIB_S} MATCHES ${M_64bit}))
                message(FATAL_ERROR "\n####Extracted from lib:\n####${PIPE_OUT_LIB_S}\n####\n\nWe detected an external 32-bit link library ('${lib_file}') which cannot be linked to this 64-bit SmartPeak! Find or compile a 64-bit version and run CMake again! Evidence see above. Expected substring regex was '${M_64bit}'.")
              elseif((SMARTPEAK_64BIT_ARCHITECTURE EQUAL 0) AND NOT(${PIPE_OUT_LIB_S} MATCHES ${M_32bit}))
                message(FATAL_ERROR "\n####Extracted from lib:####\n${PIPE_OUT_LIB_S}\n####\n\nWe detected an external 64-bit link library ('${lib_file}') which cannot be linked to this 32-bit SmartPeak! Find or compile a 32-bit version and run CMake again! Evidence see above. Expected substring was '${M_32bit}'.")
              endif()
              ## prevent this function from running its (expensive) body during next call to cmake
              set(${LIB_VAR} "1" CACHE STRING "Checking of link library ${LIB_VAR_N} architecture (32/64bit): 1 if done (will not be repeated), 0 otherwise." FORCE)
            endif() ## data was long enough
          endif() ## checked already
        endif() ## .lib-file
      endforeach(lib_file)
      message(STATUS "  Checking architecture of link libraries ... done")
    else()
      ## not implemented yet for Linux/MacOSX
    endif()
    
 endfunction()
