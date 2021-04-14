#!/usr/bin/env bash

if [ $# -eq 0 ]
then
    echo "Error: please provide the path to the binaries"
    echo "Example: ./run_example.sh ~/SmartPeak_build/bin"
    exit 1
fi

# workaround for cygwin error in appveyor:
# https://github.com/appveyor/ci/issues/1956#issuecomment-492008640
command_prefix=""

if [ $# -eq 2 ]
then
    command_prefix=$2
fi

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

cd $1 || return 1

echo "build directory: $1"
echo "unix binaries: $command_prefix"

{
  (run_example GCMS_SIM_Unknown_test)&
  (run_example HPLC_UV_Standards_test)&
  (run_example HPLC_UV_Unknown_test)&
  (run_example LCMS_MRM_QCs_test)&
#  (run_example LCMS_MRM_Standards_test)&
  (run_example LCMS_MRM_Unknown_test)&
} |
${command_prefix}tee examples_log.txt

# When adding a new example, update the comparison n==NUMBER_OF_EXAMPLES
${command_prefix}grep -c "PASS" examples_log.txt | ( read n && (( n==5 )) )
