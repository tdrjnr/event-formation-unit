
# Where does this really need to go ???
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/calib_data
  DESTINATION ${CMAKE_BINARY_DIR}/prototype2)

#=============================================================================
# Detector Pipeline Plugins
#=============================================================================

# Multi Grid demonstrator detector - obsolete
#set(mgcncs_SRC mgcncs.cpp mgcncs/Data.cpp mgcncs/ChanConv.cpp)
#set(mgcncs_INC mgcncs/Data.h mgcncs/ChanConv.h mgcncs/MultigridGeometry.h)
#create_module(mgcncs)

# Two-thread implementation of Multi Grid demonstrator detector
# For the old readout system
set(mgcncs2_SRC
  mgcncs2.cpp
  mgcncs/Data.cpp
  mgcncs/ChanConv.cpp
  mgcncs/CalibrationFile.cpp
  )
set(mgcncs2_INC
  mgcncs/Data.h
  mgcncs/ChanConv.h
  mgcncs/MultigridGeometry.h
  mgcncs/CalibrationFile.h
  )
create_module(mgcncs2)

# Two-thread implementation of Multi Grid demonstrator detector
# for Mesytec Readout
set(mgmesytec_SRC
  ../gdgem/nmx/Hists.cpp
  ../gdgem/nmx/HistSerializer.cpp
  ../common/ReadoutSerializer.cpp
  mgmesytec.cpp
  mgmesytec/Data.cpp
  )
set(mgmesytec_INC
  ../gdgem/nmx/Hists.h
  ../gdgem/nmx/HistSerializer.h
  ../common/ReadoutSerializer.h
  mgmesytec/Data.h
  )
create_module(mgmesytec)


#=============================================================================
# Data generators
#=============================================================================

# Generates dummy 'CSPEC' data matching the Multi Grid demonstrator
set(mgcncsgen_SRC
  mgcncsgen/mgcncsgen.cpp
  mgcncsgen/DGArgs.cpp
  mgcncs/Data.cpp
  )
set(mgcncsgen_INC
  mgcncsgen/DGArgs.h
  mgcncs/Data.h
  )
create_executable(mgcncsgen)

#
set(mgcncsgenfile_SRC
  mgcncsgen/mgcncsgenfile.cpp
  mgcncsgen/DGArgs.cpp
  )
set(mgcncsgenfile_INC
  mgcncsgen/DGArgs.h
  )
create_executable(mgcncsgenfile)

# Data generator for raw cspec data
set(mgcncsgenjson_SRC
  mgcncsgen/mgcncsgenjson.cpp
  mgcncsgen/JsonArgs.cpp
  ../../dataformats/multigrid/src/RunSpecParse.cpp
  ../../dataformats/multigrid/src/jsoncpp.cpp
  )
set(mgcncsgenjson_INC mgcncsgen/JsonArgs.h
  )
create_executable(mgcncsgenjson)

#=============================================================================
# Tests
#=============================================================================

set(MGCNCSChanConvTest_SRC
  mgcncs/ChanConvTest.cpp
  mgcncs/ChanConv.cpp
  )
create_test_executable(MGCNCSChanConvTest)

set(MGCNCSDataTest_SRC
  mgcncs/DataTest.cpp
  mgcncs/Data.cpp
  mgcncs/ChanConv.cpp
  )
set(MGCNCSDataTest_INC
  mgcncs/Data.h
  mgcncs/ChanConv.h
  mgcncs/TestData.h
  mgcncs/MultigridGeometry.h
  )
create_test_executable(MGCNCSDataTest)


set(MGMesytecDataTest_SRC
  ../gdgem/nmx/Hists.cpp
  ../common/ReadoutSerializer.cpp
  ../common/Producer.cpp
  mgmesytec/DataTest.cpp
  mgmesytec/Data.cpp
  )
set(MGMesytecDataTest_INC
  ../gdgem/nmx/Hists.h
  ../common/ReadoutSerializer.h
  ../common/Producer.h
  mgmesytec/Data.h
  mgmesytec/TestData.h
  )
create_test_executable(MGMesytecDataTest)

set(MG24DetectorTest_SRC
  mgmesytec/MG24DetectorTest.cpp)
set(MG24DetectorTest_INC
  mgmesytec/MG24Detector.h)
create_test_executable(MG24DetectorTest "")

#set(CSPECEventTest_SRC cspec/CSPECEventTest.cpp)
#set(CSPECEventTest_INC cspec/CSPECEvent.h)
#create_test_executable(CSPECEventTest)


# Is this ever used ???
set(PYTHON_EXECUTABLE /usr/bin/python)


if(${CMAKE_COMPILER_IS_GNUCXX})
  add_library(calibfile OBJECT mgcncs/CalibrationFile.cpp)
  enable_coverage_flags(calibfile)

  set(CalibrationFileTestLibrary_SRC
    mgcncs/CalibrationFileTest.cpp
    ../test/SyscallWrappers.cpp)
  add_library(CalibrationFileTestLibrary
    STATIC ${CalibrationFileTestLibrary_SRC})
  add_compile_flags(CalibrationFileTestLibrary
    "-Wl,--wrap=fstat,--wrap=read,--wrap=write,--wrap=open")
  enable_coverage_flags(CalibrationFileTestLibrary)

  set(CalibrationFileTest_SRC $<TARGET_OBJECTS:calibfile>)
  set(CalibrationFileTest_INC mgcncs/CalibrationFile.h)
  set(CalibrationFileTest_LIB CalibrationFileTestLibrary ${GTEST_BOTH_LIBRARIES})
  create_test_executable(CalibrationFileTest)
  add_compile_flags(CalibrationFileTest
    "-Wl,--wrap=fstat,--wrap=read,--wrap=write,--wrap=open")
  add_linker_flags(CalibrationFileTest
    "-Wl,--wrap=fstat,--wrap=read,--wrap=write,--wrap=open")
else()
  message(STATUS "*** Skipping CalibrationFileTest - requires gcc/linux")
endif()

set(MultiGridGeometryTest_SRC
  mgcncs/MultigridGeometryTest.cpp)
set(MultiGridGeometryTest_INC
  mgcncs/MultigridGeometry.h
  ../../libs/include/TSCTimer.h)
create_test_executable(MultiGridGeometryTest)
