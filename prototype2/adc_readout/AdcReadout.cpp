/** Copyright (C) 2018 European Spallation Source ERIC */

/** @file
 *
 *  @brief ADC readout detector module.
 */

#include <common/Detector.h>
#include "AdcReadoutBase.h"
#include "AdcSettings.h"

static AdcSettings LocalAdcSettings;

void CLIArguments(CLI::App &parser) {
  SetCLIArguments(parser, LocalAdcSettings);
}

PopulateCLIParser PopulateParser{CLIArguments};

class AdcReadout : public AdcReadoutBase {
public:
  AdcReadout(BaseSettings Settings)
      : AdcReadoutBase(std::move(Settings), LocalAdcSettings) {}
};

class ADC_Readout_Factory : public DetectorFactory {
public:
  std::shared_ptr<Detector> create(BaseSettings Settings) override {
    return std::shared_ptr<Detector>(new AdcReadout(Settings));
  }
};

ADC_Readout_Factory Factory;
