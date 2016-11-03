#!/bin/bash

errexit()
{
    echo Error: $1
    exit 1
}


function tools()
{
  g++ --version
  gcov --version
  lcov --version
  valgrind --version
  doxygen --version
  dot -V
  lscpu
  gcovr --version
}

function artifacts()
{
  pushd artifacts
  tar xzvf librdkafka-0.9.1.tar.gz
  popd
}

function libs()
{
  pushd libs
  make V=y
  popd
}

function libs_test()
{
  pushd libs
  make V=y $1 GTEST=../artifacts/code/googletest/build/usr/local test
  make V=y runtest
  popd
}

function prototype()
{
  pushd prototype2
  make V=y NOKAFKA=y
  popd
}

function prototype_test()
{
  pushd prototype2
  make V=y $1 GTEST=../artifacts/code/googletest/build/usr/local test
  make V=y runtest
  make doxygen
  popd
}

function coverage()
{
  mkdir -p gcovr
  gcovr -v -r . -x -e '.*Test.cpp' --exclude-directories='*artifacts*' -o gcovr/coverage.xml
  gcovr -v -r . --html --html-details -e '.*Test.cpp' --exclude-directories='*artifacts*' -o coverage.html
}


tools
artifacts
libs
prototype
libs_test COV=y
prototype_test COV=y
coverage
