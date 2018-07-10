#!/bin/bash

function build_smartPeak_dependencies {
  cmake . -DBUILD_TYPE=$1

  if [ $? -ne 0 ]; then
    # we give it another try
    echo "1st attempt to build $1 failed .. retry"
    cmake . -DBUILD_TYPE=$1 -DNUMBER_OF_JOBS=4

    if [ $? -ne 0 ]; then
      echo "2nd attempt to build $1 failed .. abort"
      exit $?
    fi
  fi
}

# fetch smartPeak_dependencies and build seqan
git clone git://github.com/SmartPeak/smartPeak_dependencies/
pushd smartPeak_dependencies

# we build WildMagic
build_smartPeak_dependencies WILDMAGIC

# we build Sqlite as the versions shipped in Ubuntu are not recent enough
build_smartPeak_dependencies SQLITE

# leave smartPeak_dependencies
popd


# build custom cppcheck if we want to perform style tests
if [ "${ENABLE_STYLE_TESTING}" = "ON" ]; then
  git clone git://github.com/danmar/cppcheck.git
  pushd cppcheck
  git checkout 1.81
  CXX=clang++ make SRCDIR=build CFGDIR=`pwd`/cfg HAVE_RULES=yes -j4
  popd
fi


