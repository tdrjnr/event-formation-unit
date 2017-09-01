/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief flatbuffer serialization
 */

#pragma once

#ifdef FLATBUFFERS
#include <../streaming-data-types/build/schemas/mo01_nmx_generated.h>
#else
#pragma message("FLATBUFFERS not defined, using old schemas")
#include <common/mo01_nmx_generated.h>
#endif

#include <common/Producer.h>
#include <libs/include/gccintel.h>

class HistSerializer {
public:
  /** @todo document */
  HistSerializer(size_t maxarraylength);

  /** @todo document */
  ~HistSerializer();

  /** @todo document */
  int serialize(uint32_t *xhist, uint32_t *yhist, size_t entries,
                char **buffer);

private:
  flatbuffers::FlatBufferBuilder builder;
  size_t maxlen{0};
  uint8_t *xarrptr{nullptr};
  uint8_t *yarrptr{nullptr};
};
