/** Copyright (C) 2018 European Spallation Source ERIC */

/** @file
 *
 *  @brief Parsing code for ADC readout.
 */

#pragma once

#include <vector>
#include <netinet/in.h>
#include "AdcBufferElements.h"
#include <exception>
#include <string>
#include <stdexcept>

class ParserException : public std::runtime_error {
public:
  enum class Type {
    UNKNOWN,
    TRAILER_FEEDF00D,
    TRAILER_0x55,
    DATA_BEEFCAFE,
    DATA_LENGTH,
    DATA_ABCD,
    STREAM_HEADER,
    STREAM_ABCD,
    STREAM_TYPE,
    STREAM_OVERSAMPLING,
    STREAM_CHANNELS_MASK,
    STREAM_SIZE,
    STREAM_BEEFCAFE,
    HEADER_LENGTH,
    HEADER_TYPE,
    IDLE_LENGTH,
  };
  ParserException(std::string ErrorStr);
  ParserException(Type ErrorType);
  virtual const char* what() const noexcept override;
  Type getErrorType() const;
private:
  Type ParserErrorType;
  std::string Error;
};

struct DataModule {
  std::uint32_t TimeStampSeconds;
  std::uint32_t TimeStampSecondsFrac;
  std::uint16_t Channel;
  std::uint16_t OversamplingFactor{1};
  std::vector<std::uint16_t> Data;
};

struct AdcData {
  std::vector<DataModule> Modules;
  std::int32_t FillerStart = 0;
};

enum class PacketType {Idle, Data, Stream, Unknown};

struct PacketData {
  std::uint16_t GlobalCount;
  std::uint16_t ReadoutCount;
  PacketType Type = PacketType::Unknown;
  std::vector<DataModule> Modules;
  std::uint32_t IdleTimeStampSeconds = 0;
  std::uint32_t IdleTimeStampSecondsFrac = 0;
};

struct HeaderInfo {
  PacketType Type = PacketType::Unknown;
  std::uint16_t GlobalCount;
  std::uint16_t ReadoutCount;
  std::int32_t DataStart = 0;
  std::uint16_t TypeValue; //Used only by streaming data
};

struct TrailerInfo {
  std::uint32_t FillerBytes = 0;
};

struct IdleInfo {
  std::uint32_t TimeStampSeconds = 0;
  std::uint32_t TimeStampSecondsFrac = 0;
  std::int32_t FillerStart = 0;
};

struct StreamSetting {
  std::vector<int> ChannelsActive;
  int OversamplingFactor{1};
};

#pragma pack(push, 2)
struct PacketHeader {
  std::uint16_t GlobalCount;
  std::uint16_t PacketType;
  std::uint16_t ReadoutLength;
  std::uint16_t ReadoutCount;
  std::uint16_t Reserved;
  void fixEndian() {
    GlobalCount = ntohs(GlobalCount);
    PacketType = ntohs(PacketType);
    ReadoutLength = ntohs(ReadoutLength);
    ReadoutCount = ntohs(ReadoutCount);
  }
};

struct DataHeader {
  std::uint16_t MagicValue;
  std::uint16_t Length;
  std::uint16_t Channel;
  std::uint16_t Fragment;
  std::uint32_t TimeStampSeconds;
  std::uint32_t TimeStampSecondsFrac;
  void fixEndian() {
    MagicValue = ntohs(MagicValue);
    Length = ntohs(Length);
    Channel = ntohs(Channel);
    Fragment = ntohs(Fragment);
    TimeStampSeconds = ntohl(TimeStampSeconds);
    TimeStampSecondsFrac = ntohl(TimeStampSecondsFrac);
  }
};

struct StreamHeader {
  std::uint16_t MagicValue;
  std::uint16_t Length;
  std::uint32_t TimeStampSeconds;
  std::uint32_t TimeStampSecondsFrac;
  void fixEndian() {
    MagicValue = ntohs(MagicValue);
    Length = ntohs(Length);
    TimeStampSeconds = ntohl(TimeStampSeconds);
    TimeStampSecondsFrac = ntohl(TimeStampSecondsFrac);
  }
};

struct IdleHeader {
  std::uint32_t TimeStampSeconds;
  std::uint32_t TimeStampSecondsFrac;
  void fixEndian() {
    TimeStampSeconds = ntohl(TimeStampSeconds);
    TimeStampSecondsFrac = ntohl(TimeStampSecondsFrac);
  }
};
#pragma pack(pop)

PacketData parsePacket(const InData &Packet);
AdcData parseData(const InData &Packet, std::uint32_t StartByte);
AdcData parseStreamData(const InData &Packet, std::uint32_t StartByte, std::uint16_t TypeValue);
HeaderInfo parseHeader(const InData &Packet);
TrailerInfo parseTrailer(const InData &Packet, std::uint32_t StartByte);
IdleInfo parseIdle(const InData &Packet, std::uint32_t StartByte);
StreamSetting parseStreamSettings(const std::uint16_t &SettingsRaw);

