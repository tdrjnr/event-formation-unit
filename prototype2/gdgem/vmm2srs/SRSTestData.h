/** Copyright (C) 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief Contains readout data copied from Wireshark traces
 * File: wireshark_VMM2_X_0_1_Y_14_15_FE55_11Gbq_col_MM_D300V_A544V.pcap
 */

#pragma once

// Contains 22 eventlets
unsigned char srsdata_22_eventlets[] = {
    0x02, 0xe4, 0x48, 0x25, 0x56, 0x4d, 0x32, 0x0f, 0xb8, 0xce, 0xc7, 0xe0,
    0x44, 0x38, 0x03, 0x08, 0x86, 0x00, 0x00, 0x00, 0x44, 0x38, 0xc7, 0x02,
    0xe6, 0x00, 0x00, 0x00, 0x44, 0x38, 0x1d, 0x08, 0x96, 0x00, 0x00, 0x00,
    0x44, 0x38, 0x17, 0x90, 0x9e, 0x00, 0x00, 0x00, 0x44, 0x38, 0x95, 0x00,
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x44, 0x38, 0x35, 0x50, 0x91, 0x00, 0x00, 0x00, 0x44, 0x28, 0x11, 0x96,
    0xf1, 0x00, 0x00, 0x00, 0x44, 0x08, 0x03, 0x04, 0x89, 0x00, 0x00, 0x00,
    0x44, 0x08, 0x0b, 0x32, 0xe9, 0x00, 0x00, 0x00, 0x44, 0x08, 0x0d, 0x04,
    0x99, 0x00, 0x00, 0x00, 0x44, 0x08, 0x29, 0x04, 0x82, 0x00, 0x00, 0x00,
    0x44, 0x28, 0x3d, 0x09, 0xe2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x04, 0x38, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x44, 0x38, 0x13, 0x04,
    0x9a, 0x00, 0x00, 0x00, 0x44, 0xb8, 0x40, 0x01, 0xfa, 0x00, 0x00, 0x00,
    0x84, 0x3e, 0x1b, 0x02, 0x88, 0x00, 0x00, 0x00, 0x84, 0x3e, 0x9f, 0x04,
    0xe8, 0x00, 0x00, 0x00, 0x84, 0x16, 0x01, 0x04, 0x98, 0x00, 0x00, 0x00,
    0x84, 0x3e, 0x03, 0x38, 0x84, 0x00, 0x00, 0x00};

// Only SRS header, no readout data
unsigned char srsdata_0_eventlets[] = {0x02, 0xe4, 0x48, 0x25,
                                       0x56, 0x4d, 0x32, 0x0f};

// Contains one valid eventlet, but invalid asic id
unsigned char srsdata_invalid_geometry[] = {
    //                                        asic 5
    0x02, 0xe4, 0x48, 0x25, 0x56, 0x4d, 0x32, 0x05, 0xb8, 0xce,
    0xc7, 0xe0, 0x44, 0x38, 0x03, 0x08, 0x00, 0x00, 0x00, 0x00};
