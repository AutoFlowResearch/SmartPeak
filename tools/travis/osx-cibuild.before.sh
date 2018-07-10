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


# tap the science tap
brew tap homebrew/science
brew tap homebrew/versions
brew update
brew install glpk coinmp

# fetch smartPeak_dependencies
git clone git://github.com/SmartPeak/smartPeak_dependencies/
pushd smartPeak_dependencies

# we build WildMagic
build_smartPeak_dependencies WILDMAGIC

# leave smartPeak_dependencies
popd
