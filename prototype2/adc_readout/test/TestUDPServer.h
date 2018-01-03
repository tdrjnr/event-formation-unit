//
// Created by Jonas Nilsson on 2017-11-08.
//

#pragma once

#define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/steady_timer.hpp>
#include <thread>
#include <atomic>

typedef std::shared_ptr<asio::ip::udp::socket> SocketPtr;

class TestUDPServer {
public:
  TestUDPServer(std::uint16_t SrcPort, std::uint16_t DstPort, std::uint32_t Packets, std::uint32_t WaitTimeNS);
  ~TestUDPServer();

private:
  asio::io_service Service;
  void threadFunction();
  std::thread AsioThread;
  asio::ip::udp::socket Socket;
  asio::ip::udp::resolver Resolver;

  static const int BufferSize = 100;
  char SendBuffer[BufferSize];
  void handleWrite(const asio::error_code &err, std::size_t BytesSent);
  void handleResolve(const asio::error_code &Err, asio::ip::udp::resolver::iterator EndpointIter);
  void handleConnect(const asio::error_code &Err, asio::ip::udp::resolver::iterator EndpointIter);
  void handleNewPacket(const asio::error_code &Err);

  std::uint32_t NrOfPackets;
  std::uint32_t WaitTimeNSec;

  bool GotError = false;

  asio::steady_timer PacketTimer;
//  asio::deadline_timer waitTimer;
};
