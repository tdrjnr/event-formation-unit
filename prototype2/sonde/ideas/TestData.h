/** Copyright (C) 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief Dataset for running unit tests - do not edit if unsure of what they
 * do!
 */

#include <vector>

using namespace std;

// clang-format off

//
// Invalid data
//
vector<uint8_t > err_short_header
{
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

vector<uint8_t> err_version
{// *
  0x21, 0xD6, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

vector<uint8_t> err_datatype
{//       **
  0x01, 0xD5, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

vector<uint8_t> err_length
{//                                                        *
  0x01, 0xD6, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

vector<uint8_t> err_nb_event_mismatch
{//                                                        *
  0x01, 0xD6, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0a,
  0x02, 0x00, 0x00, 0x00, 0x01, 0xaa, 0x00, 0x00, 0x00, 0x02
};

//
// Valid data
//
vector<uint8_t> ok_header_only  // # of events 0
{
  0x01, 0xD6, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
  0x00
};

//
vector<uint8_t> ok_events_1
{
  0x01, 0xD6, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x06,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x8a
};

vector<uint8_t> ok_events_3
{
  0x01, 0xD6, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10,
  0x03, 0x00, 0x00, 0x00, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x02,
  0x4a, 0x00, 0x00, 0x00, 0x03, 0x8a
};

// seph = single event pulse height
vector<uint8_t> type_0xd5_seph_ok_1
{
  0x01, 0xD5, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09,
  0x01, 0x02, 0x03, 0x00, 0x04, 0x00, 0x01, 0xab, 0xcd
};

vector<uint8_t> type_0xd5_seph_ok_3
{
  0x01, 0xD5, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0d,
  0x01, 0x02, 0x03, 0x00, 0x04, 0x00, 0x03, 0xaa, 0xaa, 0xbb,
  0xbb, 0xcc, 0xcc
};

// meph = multi event pulse height
vector<uint8_t> type_0xd4_meph_ok_1
{
  0x01, 0xD4, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0c,
  0x01, 0x00, 0x01,
  // time                 type  asic  chan  sample
  0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0xaa, 0xaa // 1'st readout
};

vector<uint8_t> type_0xd4_meph_ok_n3m1  //(n=3, m=1)
{
  0x01, 0xD4, 0x03, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x1e,
  0x03, 0x00, 0x01,
  // time                 type  asic  chan  sample
  0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0xaa, 0xaa, // 1'st readout
  0x00, 0x00, 0x00, 0x05, 0x02, 0x01, 0x01, 0xbb, 0xbb, // 1'st readout
  0x00, 0x00, 0x00, 0x09, 0x02, 0x02, 0x02, 0xcc, 0xcc // 1'st readout
};

// clang-format on

/** Raw packet data above, now collect into iterable containers */

vector<vector<uint8_t>> err_hdr{err_version, err_datatype, err_length,
   err_nb_event_mismatch};
