/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

#pragma once

#include <NMX/EventNMX.h>
#include <map>

class ParserClusterer {
public:
  /** @todo Martin document */
  void parse(char *buf, size_t size);

  /** @todo Martin document */
  bool event_ready();

  /** @todo Martin document */
  EventNMX get();

private:
  std::multimap<uint64_t, vmm_nugget> backlog_; /**< @todo Martin document */
};
