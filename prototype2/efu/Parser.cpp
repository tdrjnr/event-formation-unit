/** Copyright (C) 2016 European Spallation Source ERIC */

#include <algorithm>
#include <cassert>
#include <cstring>
#include <common/Trace.h>
#include <efu/Parser.h>
#include <efu/Server.h>

//#undef TRC_LEVEL
//#define TRC_LEVEL TRC_L_DEB

int Parser::parse(char * input, unsigned int ibytes, char * output, unsigned int * obytes) {
  XTRACE(CMD, DEB, "parse() received %u bytes\n", ibytes);
  *obytes = 0;
  if (ibytes == 0) {
    return -EUSIZE;
  }
  if (ibytes > SERVER_BUFFER_SIZE) {
    return -EOSIZE;
  }

  if (input[ibytes - 1] != '\0') {
    XTRACE(CMD, DEB, "adding null termination\n");
    auto end = std::min(ibytes, SERVER_BUFFER_SIZE - 1);
    input[end] = '\0';
  }

  std::vector<std::string> tokens;
  char *chars_array = strtok((char *)input, "\n ");
  while (chars_array) {
    std::string token(chars_array);
    tokens.push_back(token);
    chars_array = strtok(NULL, "\n ");
  }

  if ((int)tokens.size() < 1) {
    return -ENOTOKENS;
  }

  XTRACE(CMD, DEB, "Tokens in command: %d\n", (int)tokens.size());
  for (auto token : tokens) {
    XTRACE(CMD, INF, "Token: %s\n", token.c_str());
  }


  /** @todo This is really ugly, consider using another approach later */
  if (tokens.at(0).compare(std::string("STAT_INPUT")) == 0) {
    XTRACE(CMD, INF, "STAT_INPUT\n");
    *obytes = snprintf(output, SERVER_BUFFER_SIZE,
        "STAT_INPUT %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64,
        opts.stat.i.rx_packets, opts.stat.i.rx_bytes,
        opts.stat.i.fifo_push_errors, opts.stat.i.fifo_free);

  } else if (tokens.at(0).compare(std::string("STAT_PROCESSING")) == 0) {
    XTRACE(CMD, INF, "STAT_PROCESSING\n");
    *obytes = snprintf((char *)output, SERVER_BUFFER_SIZE,
                 "STAT_PROCESSING %" PRIu64 ", %" PRIu64 ", %" PRIu64
                 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64,
                 opts.stat.p.rx_events, opts.stat.p.rx_error_bytes,
                 opts.stat.p.rx_discards, opts.stat.p.rx_idle,
                 opts.stat.p.fifo_push_errors, opts.stat.p.fifo_free);

  } else if (tokens.at(0).compare(std::string("STAT_OUTPUT")) == 0) {
    XTRACE(CMD, INF, "STAT_OUTPUT\n");
    *obytes = snprintf((char *)output, SERVER_BUFFER_SIZE,
        "STAT_OUTPUT %" PRIu64 ", %" PRIu64 ", %" PRIu64,
        opts.stat.o.rx_events, opts.stat.o.rx_idle, opts.stat.o.tx_bytes);

  } else if (tokens.at(0).compare(std::string("STAT_RESET")) == 0) {
    XTRACE(CMD, INF, "STAT_RESET\n");
    opts.stat.clear();
    *obytes = snprintf((char *)output, SERVER_BUFFER_SIZE, "<OK>");

  } else if (tokens.at(0).compare(std::string("STAT_MASK")) == 0) {
    if ((int)tokens.size() != 2) {
      XTRACE(CMD, INF, "STAT_MASK wrong number of argument\n");
      return -EBADARGS;
    }
    unsigned int mask = (unsigned int)std::stoul(tokens.at(1), nullptr, 0);
    XTRACE(CMD, WAR, "STAT_MASK 0x%08x\n", mask);
    opts.stat.set_mask(mask);
    *obytes = snprintf((char *)output, SERVER_BUFFER_SIZE, "<OK>");

  } else {
    return -EBADCMD;
  }
  return 0;
}
