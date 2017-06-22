/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief flatbuffer serialization
 */

#pragma once

#include <../monitors/schemas/mon_efu_generated.h>
#include <gdgem/nmx/EventNMX.h>

class TrackSerializer {
public:

  /** @todo document */
  TrackSerializer(size_t maxarraylength);

  /** @todo document */
  ~TrackSerializer();

  /** @todo document */
  //int add_track(uint32_t plane, uint32_t strip, uint32_t time, uint32_t adc);
  int add_track(const EventNMX & event, size_t minhits);

  /** @todo document */
  int serialize(char ** buffer);

private:
  flatbuffers::FlatBufferBuilder builder;
  size_t maxlen{0};

  std::vector<flatbuffers::Offset<pos>> xpos;
  std::vector<flatbuffers::Offset<pos>> ypos;
};