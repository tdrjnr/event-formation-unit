include_directories(.)

#=============================================================================
# Detector Pipeline Plugins
#=============================================================================
set(mbcaen_SRC
  mbcaen.cpp
  mbcaen/MBData.cpp
  mbcommon/multiBladeEventBuilder.cpp
  mbcommon/dumpEventBuilderInfo.cpp
  )
set(mbcaen_INC
  mbcaen/MBData.h
  mbcommon/multiBladeEventBuilder.h
  mbcommon/dumpEventBuilderInfo.h
  )
create_module(mbcaen)

#=============================================================================
# Build MB text 2 UDP
#=============================================================================
set(mbtext2udp_SRC
  mbtext2udp.cpp
  mbgen/MBArgs.cpp
  mbcommon/TextFile.cpp
  )
set(mbtext2udp_INC
  mbgen/MBArgs.h
  )
create_executable(mbtext2udp)

# Excecutables to build if TCLAP is installed
find_package(TCLAP)
if(TCLAP_FOUND)
  # Create an excecutable for the event-builder to process recorded data from a text-file.
  set(mbprocesstextfile_SRC
    mbprocesstextfile.cpp
    mbcommon/multiBladeEventBuilder.cpp
    mbcommon/TextFile.cpp
    mbcommon/dumpEventBuilderInfo.cpp
    )
  set(mbprocesstextfile_INC
    mbcommon/multiBladeEventBuilder.h
    mbcommon/TextFile.h
    mbcommon/dumpEventBuilderInfo.h
    )
  create_executable(mbprocesstextfile)

  # Create an excecutable to pass data several times for profiling
  # Not really usefull unless built with -gp
  set(mbprofiling_SRC mbprofiling.cpp
    mbcommon/multiBladeEventBuilder.cpp
    mbcommon/TextFile.cpp
    mbcommon/dumpEventBuilderInfo.cpp
    )
  set(mbprofiling_INC
    mbcommon/multiBladeEventBuilder.h
    mbcommon/TextFile.h
    mbcommon/dumpEventBuilderInfo.h
    )
  create_executable(mbprofiling)
else()
  message(STATUS "*** Unable to compile mbprocesstextfile as TCLAP was not found.")
  message(STATUS "*** Unable to compile mbprofiling as TCLAP was not found.")
endif()

# GOOGLE BENCHMARK
set(MBDataBenchmarkTest_INC
  ${mbtest_INC}
  )
set(MBDataBenchmarkTest_SRC
  mbcaen/MBData.cpp
  mbcaen/MBDataBenchmarkTest.cpp
  )
create_benchmark_executable(MBDataBenchmarkTest)

# GOOGLE BENCHMARK
set(multiBladeEventBuilderBenchmarkTest_INC
  ${mbtest_INC}
  )
set(multiBladeEventBuilderBenchmarkTest_SRC
  mbcommon/multiBladeEventBuilder.cpp
  mbcommon/multiBladeEventBuilderBenchmarkTest.cpp
  )
create_benchmark_executable(multiBladeEventBuilderBenchmarkTest)


# Common include path for both tests
set(mbtest_INC
  mbcommon/multiBladeEventBuilder.h
  mbtest/multiBladeTestData.h
  )

# Create excecutable for the test of counters
set(mbtestcounter_INC
  ${mbtest_INC}
  )
set(mbcountertest_SRC
  mbcommon/multiBladeEventBuilder.cpp
  mbtest/multiBladeEventBuilderCounterTest.cpp
  )
create_test_executable(mbcountertest)

# Create excecutable for the test of clustering
set(mbclustertest_SRC
  mbcommon/multiBladeEventBuilder.cpp
  mbtest/multiBladeEventBuilderClusteringTest.cpp
  )
set(mbclustertest_INC
  ${mbtest_INC}
)
create_test_executable(mbclustertest)

# Create excecutable for the test of geometry
set(MB16DetectorTest_SRC
  mbcaen/MB16DetectorTest.cpp
  )
set(MB16DetectorTest_INC
  mbcaen/MB16Detector.h
  )
create_test_executable(MB16DetectorTest)
