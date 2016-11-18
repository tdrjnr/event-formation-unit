/** Copyright (C) 2016 European Spallation Source ERIC */

#include <cassert>
#include <common/Trace.h>
#include <cspec/CSPECData.h>
#include <cstring>

using namespace std;

struct multi_grid {
  uint32_t header;
  uint32_t data[8];
  uint32_t footer;
} __attribute__((packed));

//#undef TRC_LEVEL
//#define TRC_LEVEL TRC_L_DEB

/** @todo no error checking, assumes valid data and valid buffer
 */
void CSPECData::createevent(const MultiGridData &data, char *buffer) {
  auto col = data.module;
  auto grid = chanconv->getGridId(data.d[6]);
  auto wire = chanconv->getWireId(data.d[2]);
  auto pixid = multigridgeom->getdetectorpixelid(col, grid, wire);

  static_assert(sizeof(data.time) == 4, "time should be 32 bit");
  static_assert(sizeof(pixid) == 4, "pixelid should be 32 bit");

  std::memcpy(buffer + 0, &data.time, sizeof(data.time));
  std::memcpy(buffer + 4, &pixid, sizeof(pixid));
}

int CSPECData::receive(const char *buffer, int size) {
  elems = 0;
  error = 0;
  enum State { hdr = 1, dat, ftr };
  int datctr = 0;
  uint32_t *datap = (uint32_t *)buffer;
  int state = State::hdr;
  int oldsize = size;

  while (size >= 4) {
    XTRACE(PROCESS, DEB, "elems: %d, size: %d, datap: %p\n", elems, size,
           datap);
    switch (state) {
    // Parse Header
    case State::hdr:
      if (((*datap & header_mask) != header_id) ||
          ((*datap & 0xfff) != nwords)) {
        XTRACE(PROCESS, INF, "State::hdr - header error\n");
        break;
      }
      XTRACE(PROCESS, DEB, "State::hdr valid data, next state State:dat\n");
      data[elems].module = (*datap >> 16) & 0xff;
      datctr = 0;
      state = State::dat;
      break;

    // Parse Data
    case State::dat:
      if ((*datap & header_mask) != data_id) {
        XTRACE(PROCESS, INF, "State::dat - header error\n");
        state = State::hdr;
        break;
      }
      XTRACE(PROCESS, DEB, "State::dat valid data (%d), next state State:dat\n",
             datctr);
      data[elems].d[datctr] = (*datap) & 0x3fff;
      datctr++;
      if (datctr == 8) {
        XTRACE(PROCESS, DEB, "State:dat all data, next state State:ftr\n");
        state = State::ftr;
      }
      break;

    // Parse Footer
    case State::ftr:
      if ((*datap & header_mask) != footer_id) {
        XTRACE(PROCESS, INF, "State::ftr - header error\n");
        state = State::hdr;
        break;
      }
      XTRACE(PROCESS, DEB, "State::ftr valid data, next state State:hdr\n");
      data[elems].time = (*datap) & 0x3fffffff;
      elems++;
      state = State::hdr;
      break;
    }
    size -= 4; // Parse 32 bit at a time
    datap++;
  }
  error = oldsize - (elems * datasize);

  return elems;
}

int CSPECData::input_filter() {
  int discarded = 0;
  if (elems == 0)
    return discarded;

  for (unsigned int i = 0; i < elems; i++) {
    data[i].valid = 0;
    if ((data[i].d[0] < wire_thresh) || (data[i].d[4] < grid_thresh)) {
      // printf("data 0 or 4 failed, thresholds: %d, %d\n", wire_thresh,
      // grid_thresh);
      // assert(1==9);
      discarded++;
      continue;
    }
    data[i].valid = 1;

    if ((data[i].d[1] >= wire_thresh) || (data[i].d[5] >= grid_thresh)) {
      // printf("data 1 or 5 failed, thresholds: %d, %d\n", wire_thresh,
      // grid_thresh);
      // assert(1==9);
      discarded++;       // double event
      data[i].valid = 0; // invalidate
      continue;
    }
  }
  return discarded;
}

/** First multi grid data generator - valid headers, custom
 * wire and grid adc values. Only used in google test - can
 * be excluded from coverage
 */
int CSPECData::generate(char *buffer, int size, int elems,
                        unsigned int wire_adc, unsigned int grid_adc) {
  int bytes = 0;
  int events = 0;
  auto mg = (struct multi_grid *)buffer;
  while ((size >= CSPECData::datasize) && elems) {
    events++;
    mg->header = header_id + nwords;
    for (int i = 0; i != 8; ++i) {
      mg->data[i] = data_id;
    }
    mg->data[0] += wire_adc;
    mg->data[4] += grid_adc;

    mg->footer = footer_id + events;
    mg++;
    elems--;
    size -= CSPECData::datasize;
    bytes += CSPECData::datasize;
  }
  return bytes;
}
