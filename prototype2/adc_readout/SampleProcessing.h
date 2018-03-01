/** Copyright (C) 2018 European Spallation Source ERIC */

/** @file
 *
 *  @brief Sample processing of the ADC data.
 */

#pragma once

#include <memory>
#include <deque>
#include <vector>
#include <map>
#include "AdcDataProcessor.h"

struct ProcessedSamples {
  int ChannelNr;
  std::uint64_t TimeStamp;
  double TimeDelta;
  std::vector<std::uint16_t> Samples;
  std::vector<std::uint64_t> TimeStamps;
};

class ChannelProcessing {
public:
  ChannelProcessing();
  ProcessedSamples operator()(const DataModule &Samples);
  void setMeanOfSamples(unsigned int NrOfSamples);
  void reset();
private:
  unsigned int MeanOfNrOfSamples{1};
  int SumOfSamples{0};
  int NrOfSamplesSummed{0};
  std::uint64_t TimeStampOfFirstSample;
//  std::deque<std::uint16_t> SampleBuffer;
};

class SampleProcessing : public AdcDataProcessor {
public:
  SampleProcessing(std::shared_ptr<Producer> Prod);
  virtual void operator()(const PacketData &Data) override;
  void setMeanOfSamples(unsigned int NrOfSamples);
protected:
  void serializeData(const ProcessedSamples &Data) const;
  void transmitData(const std::uint8_t &DataPtr, const size_t Size) const;
private:
  
  //std::map<int, > SampleBuffers;
};
