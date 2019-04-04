#!/usr/bin/env bash

run_example()
{
    filename=$1
    echo -e "${filename} &> ${filename}_output.txt\n"
    time ./"${filename}" &> "${filename}_output.txt"
    RET_VALUE=$?
    if [ $RET_VALUE -eq 0 ]
    then
        echo "${filename}: PASS"
    else
        echo "${filename}: FAIL"
    fi
    echo -e "\n\n"
    return $RET_VALUE
}

# Assumption: SmartPeak2 build folder
cd ~/SmartPeak2_build/bin || return 1

{
  (run_example GCMS_SIM_Unknown_test)&
  (run_example HPLC_UV_Standards_test)&
  (run_example HPLC_UV_Unknown_test)&
  (run_example LCMS_MRM_QCs_test)&
  (run_example LCMS_MRM_Standards_test)&
  (run_example LCMS_MRM_Unknown_test)&
} |
tee examples_log.txt

# When adding a new example, update the comparison n==NUMBER_OF_EXAMPLES
grep -c "PASS" examples_log.txt | ( read n && (( n==6 )) )
