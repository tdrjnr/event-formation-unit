/** Copyright (C) 2018 European Spallation Source ERIC */

/** @file
 *
 *  @brief ADC settings.
 */

#pragma once

#include <CLI11.hpp>

// TODO: Remove struct from name
struct AdcSettingsStruct {
  bool SerializeSamples{false};
  bool PeakDetection{false};
  bool SampleTimeStamp{false};
  // TODO: MeanSampleSize - needs a better name
  int TakeMeanOfNrOfSamples{1};
  bool IncludeTimeStamp{false};
  // TODO: check whether CLI11 would allow an enum
  std::string TimeStampLocation{"Middle"};
  std::string Name;
};

void SetCLIArguments(CLI::App &parser, AdcSettingsStruct &AdcSettings);
