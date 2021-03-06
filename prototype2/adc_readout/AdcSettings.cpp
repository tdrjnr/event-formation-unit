/** Copyright (C) 2018 European Spallation Source ERIC */

/** @file
 *
 *  @brief Populate the command line parser with possible arguments.
 */

#include "AdcSettings.h"
#include "AdcReadoutConstants.h"

void SetCLIArguments(CLI::App &parser, AdcSettings &Settings) {
  parser
      .add_flag("--serialize_samples", Settings.SerializeSamples,
                "Serialize sample data and send to Kafka broker.")
      ->group("ADC Readout Options");
  parser
      .add_flag("--peak_detection", Settings.PeakDetection,
                "Find the maximum value in a range of samples and send that "
                "value along with its time-stamp to he Kafka broker.")
      ->group("ADC Readout Options");
  parser
      .add_option("--name", Settings.Name,
                  "Name of the source of the data as made available on the "
                  "Kafka broker.")
      ->group("ADC Readout Options")
      ->set_default_val("AdcDemonstrator");
  parser
      .add_flag("--sample_timestamp", Settings.SampleTimeStamp,
                "Provide a timestamp with every single ADC sample. Note: this "
                "drastically increases the bandwidth requirements.")
      ->group("ADC Readout Options");
  auto IsPositiveInt = [&Settings](std::vector<std::string> Input) -> bool {
    int InputVal;
    try {
      InputVal = std::stoi(Input[0]);
      if (InputVal < 1 or InputVal > AdcTimerCounterMax) {
        return false;
      }
    } catch (std::invalid_argument &E) {
      return false;
    } catch (std::out_of_range &E) {
      return false;
    }
    Settings.TakeMeanOfNrOfSamples = InputVal;
    return true;
  };
  CLI::callback_t CBFunc(IsPositiveInt);
  parser
      .add_option("--mean_of_samples", CBFunc,
                  "Only used when serializing data. Take the mean of # of "
                  "samples (oversample) and serialize that mean.")
      ->group("ADC Readout Options")
      ->set_default_val("1");
  parser
      .add_set("--time_stamp_loc", Settings.TimeStampLocation,
               {"Start", "Middle", "End"},
               "Only used when serializing oversampled data. The time stamp "
               "corresponds to one of the following: 'Start', 'Middle', 'End'.")
      ->group("ADC Readout Options")
      ->set_default_val("Middle");
}
