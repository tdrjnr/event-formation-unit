/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief flatbuffer serialization
 */

#pragma once

#include "mo01_nmx_generated.h"

#include <gdgem/nmx/EventNMX.h>

class TrackSerializer {
public:
  /** @todo document */
  TrackSerializer(size_t maxarraylength, size_t minhits = 0);

  /** @todo document */
  ~TrackSerializer();

  /** @todo document */
  // int add_track(uint32_t plane, uint32_t strip, uint32_t time, uint32_t adc);
  int add_track(const EventNMX &event);

  /** @todo document */
  int serialize(char **buffer);

private:
  flatbuffers::FlatBufferBuilder builder;
  size_t maxlen{0};
  size_t minhits_{0};

  uint64_t time_offset{0};
  std::vector<flatbuffers::Offset<pos>> xtrack;
  std::vector<flatbuffers::Offset<pos>> ytrack;
  double xpos{-1};
  double ypos{-1};
};
