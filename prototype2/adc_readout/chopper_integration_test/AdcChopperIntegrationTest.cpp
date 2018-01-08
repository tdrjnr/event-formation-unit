#include <iostream>
#include <fstream>
#include "UDPServer.h"
#include "PacketGenerator.h"
#include "TDCGenerator.h"
#include <chrono>
#include <thread>
#include <csignal>
#include <iomanip>
#include "Producer.h"
#include <random>
#include <algorithm>

bool RunLoop = true;

void signalHandler(int signal) {
  std::cout << "Got exit signal:" << signal << std::endl;
  RunLoop = false;
}

struct MonitorSimParams {
  std::uint16_t ChannelNr;
  int MeanNrOfPulses;
  double TimeOffset;
  double TimeWidth;
};

struct MonitorPulseInfo {
  std::uint16_t ChannelNr;
  double TimeStamp;
  double Amplitude;
};

bool PulseSortingFunction(const MonitorPulseInfo &Pulse1, const MonitorPulseInfo &Pulse2) {
  return Pulse1.TimeStamp < Pulse2.TimeStamp;
}

int main() {
  const long double ChopperFrequency = 14;
  std::chrono::duration<long double> ChopperPeriod(1 / ChopperFrequency);
  auto PulseTime = std::chrono::system_clock::now() + ChopperPeriod;
  std::signal(SIGINT, signalHandler);
  Producer KafkaProducer("localhost", "tdc_data");
  
  std::random_device rd;
  std::mt19937 gen(rd());
  
  std::vector<MonitorSimParams> Monitors;
  Monitors.emplace_back(MonitorSimParams{0, 10, -50e-6, 10e-6}); //Pulses, offset, width
  Monitors.emplace_back(MonitorSimParams{1, 10, 150e-6, 10e-6}); //Pulses, offset, width
  
  ADC::PacketGenerator AdcGenerator;
  TDC::TDCGenerator TdcGenerator("mini_chopper");
  
  UDPServer server(2048, 65535);
  while (not server.IsOk()) {
  }
  
  while (RunLoop) {
    PulseTime += ChopperPeriod;
    std::this_thread::sleep_until(PulseTime);
    long double FloatTime = std::chrono::duration_cast<std::chrono::duration<long double>>(PulseTime.time_since_epoch()).count();
    
    auto TdcPacket = TdcGenerator.GetTDCPacket(double(FloatTime));
    KafkaProducer.produce((char*)TdcPacket.Pointer, TdcPacket.Size);
    
    std::vector<MonitorPulseInfo> AllPulses;
    for (auto &Monitor: Monitors) {
      std::poisson_distribution<> NrOfPulsesGen(Monitor.MeanNrOfPulses);
      int NrOfPulses = NrOfPulsesGen(gen);
      std::normal_distribution<> TimeOffsetGen{Monitor.TimeOffset,Monitor.TimeWidth};
      for (int i = 0; i < NrOfPulses; i++) {
        double CurrentTimeOffset = TimeOffsetGen(gen);
        double CurrentTimeStamp = FloatTime + CurrentTimeOffset;
        AllPulses.emplace_back(MonitorPulseInfo{Monitor.ChannelNr, CurrentTimeStamp, 1.0});
      }
    }
    std::sort(AllPulses.begin(), AllPulses.end(), PulseSortingFunction);
    for (auto &Pulse : AllPulses) {
      std::uint32_t TS_Seconds(Pulse.TimeStamp);
      auto AdcPacket = AdcGenerator.GeneratePacket(TS_Seconds, Pulse.TimeStamp - TS_Seconds, Pulse.ChannelNr);
      
      server.TransmitPacket(AdcPacket.Pointer, AdcPacket.Size);
    }
  }
  std::cout << "Waiting for transmit thread to exit!" << std::endl;
  return 0;
}
