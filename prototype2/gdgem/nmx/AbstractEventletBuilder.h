/** Copyright (C) 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief Bastract class for creating NMX eventlets
 */

#pragma once

#include <gdgem/nmx/Clusterer.h>
#include <gdgem/nmx/Hists.h>

#ifdef DUMPTOFILE
#include <common/DataSave.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <h5cpp/hdf5.hpp>
#pragma GCC diagnostic pop
#endif

class AbstractBuilder {
public:
  struct ResultStats {
    ResultStats() {}
    ResultStats(uint32_t ev, uint32_t err, uint32_t geom_err)
        : valid_eventlets(ev), error_bytes(err), geom_errors(geom_err) {}

    uint32_t valid_eventlets{0};
    uint32_t error_bytes{0};
    uint32_t geom_errors{0};
  };

  AbstractBuilder(std::string dump_dir, bool dump_csv, bool dump_h5);

  virtual ~AbstractBuilder() {}

  /** @todo Martin document */
  virtual ResultStats process_buffer(char *buf, size_t size,
                                     Clusterer &clusterer, NMXHists &hists) = 0;

protected:
  bool dump_csv_{false};
  bool dump_h5_{false};

#ifdef DUMPTOFILE

  // CSV
  std::shared_ptr<DataSave> vmmsave;

  // H5
  hdf5::file::File file_;
  hdf5::node::Dataset srstime_;
  hdf5::node::Dataset bc_tdc_adc_;
  hdf5::node::Dataset fec_chip_chan_thresh_;
  hsize_t event_num_{0};

  static std::string time_str();
  void setup_h5(std::string dump_dir);
#endif
};
