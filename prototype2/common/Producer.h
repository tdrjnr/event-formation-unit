/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief Wrapper class for sending data to Kafka broker
 */

#pragma once

#include <librdkafka/rdkafkacpp.h>

class ProducerBase {
public:
  ProducerBase() = default;
  virtual ~ProducerBase() = default;
  virtual int produce(char *buffer, int length) = 0;
};

class Producer : public ProducerBase {
public:
  /** @brief Construct a producer object.
   * @param broker 'URL' specifying host and port, example "127.0.0.1:9009"
   * @param topicstr Name of Kafka topic according to agreement, example
   * "T-REX_detectors"
   */
  Producer(std::string broker, std::string topicstr);

  ~Producer();

  /** @brief Function called to send data to a broker
   *  @param buffer Pointer to char buffer containing data to be tx'ed
   *  @param length Size of buffer data in bytes
   */
  int produce(char *buffer, int length) override;

private:
  std::string errstr;
  RdKafka::Conf *conf{0};
  RdKafka::Conf *tconf{0};
  RdKafka::Topic *topic{0};
  RdKafka::Producer *producer{0};
};
