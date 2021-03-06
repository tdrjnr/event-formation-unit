/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief Command line argument parser for nmxgen
 */

#pragma once
#include <string>

class NMXArgs {
public:
  /** @brief constructor for program arguments parsed via getopt_long()
   * @param argc Argument count - typically taken from main()
   * @param argv Argument array - typically taken from main()
   */
  NMXArgs(int argc, char *argv[]);

  std::string filename{}; /**< for single file streaming */

  unsigned int txGB{10}; /**< total transmit size (GB) */
  uint64_t txPkt{0xffffffffffffffff};
  int txEvt{100}; /**< 100 events per packet */

  std::string dest_ip{"127.0.0.1"}; /**< destination ip address */
  int port{9000};                   /**< destination udp port */
  int buflen{9000};                 /**< Tx buffer size */
  int sndbuf{1000000};              /**< kernel sndbuf size */

  int throttle{0};   /**< actually a sleep() counter */
  int loop{0};       /**< single shot or loop */
  int pcapoffset{0}; /**< for pcap: start after offset */

  unsigned int updint{1}; /**< update interval (seconds) */
};
