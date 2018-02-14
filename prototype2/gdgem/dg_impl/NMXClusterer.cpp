#include <algorithm>
#include <cmath>
#include <cassert>
#include <common/Trace.h>
#include <gdgem/dg_impl/NMXClusterer.h>

//#undef TRC_LEVEL
//#define TRC_LEVEL TRC_L_DEB

NMXClusterer::NMXClusterer(int bc, int tac, int acqWin, std::vector<int> xChips,
                           std::vector<int> yChips, int adcThreshold,
                           int minClusterSize, float deltaTimeHits,
                           int deltaStripHits, float deltaTimeSpan,
                           float deltaTimePlanes)
    : pBC(bc), pTAC(tac), pAcqWin(acqWin), pXChipIDs(xChips), pYChipIDs(yChips),
      pADCThreshold(adcThreshold), pMinClusterSize(minClusterSize),
      pDeltaTimeHits(deltaTimeHits), pDeltaStripHits(deltaStripHits),
      pDeltaTimeSpan(deltaTimeSpan), pDeltaTimePlanes(deltaTimePlanes),
      m_eventNr(0) {}

NMXClusterer::~NMXClusterer() {}

//====================================================================================================================
int NMXClusterer::AnalyzeHits(int triggerTimestamp, unsigned int frameCounter,
                              int fecID, int vmmID, int chNo, int bcid, int tdc,
                              int adc, int overThresholdFlag) {

  bool newEvent = false;
  double triggerTimestamp_ns = triggerTimestamp * 3.125;
  double deltaTriggerTimestamp_ns = 0;

  if (m_oldTriggerTimestamp_ns != triggerTimestamp_ns) {
    AnalyzeClusters();
    newEvent = true;
    m_subsequentTrigger = false;
    m_eventNr++;
  }

  if ((frameCounter < m_oldFrameCounter) && !(m_oldFrameCounter > frameCounter + 1000000000)) {
    DTRACE(DEB, "\n*********************************** SCRAMBLED eventNr  %d, "
                "old framecounter %d, new framecounter %u\n",
           m_eventNr, m_oldFrameCounter, frameCounter);
    stats_fc_error++;
  }

  if (m_oldTriggerTimestamp_ns > triggerTimestamp_ns &&
      (m_oldFrameCounter <= frameCounter ||
       m_oldFrameCounter > frameCounter + 1000000000)) {
    deltaTriggerTimestamp_ns = (13421772800 + triggerTimestamp_ns - m_oldTriggerTimestamp_ns);
    stats_triggertime_wraps++;
  } else {
    deltaTriggerTimestamp_ns = (triggerTimestamp_ns - m_oldTriggerTimestamp_ns);
  }

  if (newEvent &&
      (deltaTriggerTimestamp_ns <= 1000 * 4096 * (1 / (double)pBC))) {
    m_subsequentTrigger = true;
  }

  if (m_eventNr > 1) {
    m_timeStamp_ms = m_timeStamp_ms + deltaTriggerTimestamp_ns * 0.000001;
  }

  int planeID = GetPlaneID(vmmID);

  // Plane 0: x
  // plane 1: y
  int x = -1;
  int y = -1;
  if (planeID == 0) {
    // Fix for entries with all zeros
    if (bcid == 0 && tdc == 0 && overThresholdFlag) {
      bcid = m_oldBcidX;
      tdc = m_oldTdcX;
      stats_bcid_tdc_error++;
    }
    m_oldBcidX = bcid;
    m_oldTdcX = tdc;
    x = GetChannel(pXChipIDs, vmmID, chNo);
  } else if (planeID == 1) {
    // Fix for entries with all zeros
    if (bcid == 0 && tdc == 0 && overThresholdFlag) {
      bcid = m_oldBcidY;
      tdc = m_oldTdcY;
      stats_bcid_tdc_error++;
    }
    m_oldBcidY = bcid;
    m_oldTdcY = tdc;
    y = GetChannel(pYChipIDs, vmmID, chNo);
  }

  // Calculate bcTime [us]
  double bcTime = bcid * (1 / (double)pBC);

  // TDC time: pTAC * tdc value (8 bit)/ramp length
  // [ns]

  // TDC has reduced resolution due to most significant bit problem of current
  // sources (like ADC)
  int tdcRebinned = (int)tdc / 8;
  tdc = tdcRebinned * 8;
  double tdcTime = pTAC * (double)tdc / 255;

  // Chip time: bcid plus tdc value
  // Talk Vinnie: HIT time  = BCIDx25 + ADC*125/255 [ns]
  double chipTime = bcTime * 1000 + tdcTime;

  StoreHits(x, y, adc, bcid, chipTime, overThresholdFlag);

  if (newEvent) {
    DTRACE(DEB, "\neventNr  %d\n", m_eventNr);
    DTRACE(DEB, "fecID  %d\n", fecID);
  }

  if (deltaTriggerTimestamp_ns > 0) {
    DTRACE(DEB, "\tTimestamp %.2f [ms]\n", m_timeStamp_ms);
    DTRACE(DEB, "\tTime since last trigger %.4f us (%.4f kHz)\n",
           deltaTriggerTimestamp_ns * 0.001,
           1000000 / deltaTriggerTimestamp_ns);
    DTRACE(DEB, "\tTriggerTimestamp %.2f [ns]\n", triggerTimestamp_ns);
  }
  if (m_oldFrameCounter != frameCounter || newEvent) {
    DTRACE(DEB, "\n\tFrameCounter %u\n", frameCounter);
  }
  if (m_oldVmmID != vmmID || newEvent) {
    DTRACE(DEB, "\tvmmID  %d\n", vmmID);
  }
  if (planeID == 0) {
    DTRACE(DEB, "\t\tx-channel %d (chNo  %d) - overThresholdFlag %d\n", x, chNo,
           overThresholdFlag);
  } else if (planeID == 1) {
    DTRACE(DEB, "\t\ty-channel %d (chNo  %d) - overThresholdFlag %d\n", y, chNo,
           overThresholdFlag);
  } else {
    DTRACE(DEB, "\t\tPlane for vmmID %d not defined!\n", vmmID);
  }
  DTRACE(DEB, "\t\t\tbcid %d, tdc %d, adc %d\n", bcid, tdc, adc);
  DTRACE(DEB, "\t\t\tbcTime %.2f us, tdcTime %.2f ns, time %.2f us\n", bcTime,
         tdcTime, chipTime * 0.001);

  m_oldTriggerTimestamp_ns = triggerTimestamp_ns;
  m_oldFrameCounter = frameCounter;
  m_oldVmmID = vmmID;
  return 0;
}

//====================================================================================================================
void NMXClusterer::StoreHits(short x, short y, short adc, short bcid,
                             float chipTime, bool overThresholdFlag) {

  if ((adc >= pADCThreshold || overThresholdFlag)) {
    if (x > -1) {
      if (bcid < pAcqWin * pBC / 40) {
        m_hitsX.emplace_back(chipTime, x, adc);
      } else {
        m_hitsOldX.emplace_back(chipTime, x, adc);
      }
    } else if (y > -1) {
      if (bcid < pAcqWin * pBC / 40) {
        m_hitsY.emplace_back(chipTime, y, adc);

      } else {
        m_hitsOldY.emplace_back(chipTime, y, adc);
      }
    }
  }
}

//====================================================================================================================
int NMXClusterer::ClusterByTime(HitContainer &oldHits, float dTime, int dStrip,
                                float dSpan, string coordinate) {

  ClusterContainer cluster;

  int clusterCount = 0;
  int stripCount = 0;
  double time1 = 0, time2 = 0;
  int adc1 = 0;
  int strip1 = 0;

  for (auto &itOldHits : oldHits) {
    time2 = time1;

    time1 = std::get<0>(itOldHits);
    strip1 = std::get<1>(itOldHits);
    adc1 = std::get<2>(itOldHits);

    if (time1 - time2 > dTime && stripCount > 0) {
      clusterCount += ClusterByStrip(cluster, dStrip, dSpan, coordinate);
      cluster.clear();
    }
    cluster.emplace_back(strip1, time1, adc1);
    stripCount++;
  }

  if (stripCount > 0) {
    clusterCount += ClusterByStrip(cluster, dStrip, dSpan, coordinate);
  }
  return clusterCount;
}

//====================================================================================================================
int NMXClusterer::ClusterByStrip(ClusterContainer &cluster, int dStrip,
                                 float dSpan, string coordinate) {
  float startTime = 0;
  float largestTime = 0;
  float clusterPositionUTPC = -1;

  float centerOfGravity = -1;
  float centerOfTime = 0;
  int totalADC = 0;
  float time1 = 0;
  int adc1 = 0;
  int strip1 = 0, strip2 = 0;
  int stripCount = 0;
  int clusterCount = 0;

  std::sort(begin(cluster), end(cluster),
            [](const ClusterTuple &t1, const ClusterTuple &t2) {
              return std::get<0>(t1) < std::get<0>(t2);
            });

  for (auto &itCluster : cluster) {
    strip2 = strip1;
    strip1 = std::get<0>(itCluster);
    time1 = std::get<1>(itCluster);
    adc1 = std::get<2>(itCluster);

    // At beginning of cluster, set start time of cluster
    if (stripCount == 0) {
      startTime = time1;
      DTRACE(DEB, "\n%s cluster:\n", coordinate.c_str());
    }

    // Add members of a cluster, if it is either the beginning of a cluster,
    // or if strip gap and time span is correct
    if (stripCount == 0 ||
        (abs(strip1 - strip2) > 0 && abs(strip1 - strip2) <= (dStrip + 1) &&
         time1 - startTime <= dSpan)) {
      DTRACE(DEB, "\tstrip %d, time %f, adc %d:\n", strip1, time1, adc1);
      if (time1 > largestTime) {
        largestTime = time1;
        clusterPositionUTPC = strip1;
      }
      if (time1 < startTime) {
        startTime = time1;
      }
      centerOfGravity += strip1 * adc1;
      centerOfTime += time1 * adc1;
      totalADC += adc1;
      stripCount++;
    }
    // Stop clustering if gap between strips is too large or time span too long
    else if (abs(strip1 - strip2) > (dStrip + 1) ||
             largestTime - startTime > dSpan) {
      // Valid cluster
      if (stripCount >= pMinClusterSize) {
        centerOfGravity = (centerOfGravity / (float)totalADC);
        centerOfTime = (centerOfTime / (float)totalADC);
        StoreClusters(centerOfGravity, clusterPositionUTPC, stripCount,
                      totalADC, centerOfTime, largestTime, coordinate);
        clusterCount++;
        DTRACE(DEB, "******** VALID ********\n");
      }

      // Reset all parameters
      startTime = 0;
      largestTime = 0;
      clusterPositionUTPC = 0;
      stripCount = 0;
      centerOfGravity = 0;
      centerOfTime = 0;
      totalADC = 0;
      strip1 = 0;
    }
  }
  // At the end of the clustering, check again if there is a last valid cluster
  if (stripCount >= pMinClusterSize) {
    centerOfGravity = (centerOfGravity / (float)totalADC);
    centerOfTime = (centerOfTime / totalADC);
    StoreClusters(centerOfGravity, clusterPositionUTPC, stripCount, totalADC,
                  centerOfTime, largestTime, coordinate);
    clusterCount++;
    DTRACE(DEB, "******** VALID ********\n");
  }
  return clusterCount;
}

//====================================================================================================================
void NMXClusterer::StoreClusters(float clusterPosition,
                                 float clusterPositionUTPC, short clusterSize,
                                 int clusterADC, float clusterTime,
                                 float clusterTimeUTPC, string coordinate) {

  ClusterNMX theCluster;
  theCluster.size = clusterSize;
  theCluster.adc = clusterADC;
  theCluster.time = clusterTime;
  theCluster.time_uTPC = clusterTimeUTPC;
  theCluster.position = clusterPosition;
  theCluster.position_uTPC = clusterPositionUTPC;
  theCluster.clusterXAndY = false;
  theCluster.clusterXAndY_uTPC = false;

  if (coordinate == "x" && clusterPosition > -1.0) {
    m_tempClusterX.emplace_back(std::move(theCluster));
  }
  if (coordinate == "y" && clusterPosition > -1.0) {
    m_tempClusterY.emplace_back(std::move(theCluster));
  }
}

//====================================================================================================================
void NMXClusterer::MatchClustersXY(float dPlane) {

  for (auto &nx : m_tempClusterX) {
    float tx = nx.time;
    float posx = nx.position;
    float tx_uTPC = nx.time_uTPC;
    float posx_uTPC = nx.position_uTPC;

    float minDelta = 99999999;
    float deltaT = 0;
    ClusterVector::iterator it = end(m_tempClusterY);

    float ty = 0;
    float posy = 0;

    float minDelta_uTPC = 99999999;
    float deltaT_uTPC = 0;
    ClusterVector::iterator it_uTPC = end(m_tempClusterY);
    float ty_uTPC = 0;
    float posy_uTPC = 0;

    for (ClusterVector::iterator ny = begin(m_tempClusterY);
         ny != end(m_tempClusterY); ++ny) {
      if ((*ny).clusterXAndY == false) {
        ty = (*ny).time;
        deltaT = std::abs(ty - tx);
        if (deltaT < minDelta && deltaT <= dPlane) {
          minDelta = deltaT;
          it = ny;
        }
      }
      if ((*ny).clusterXAndY_uTPC == false) {
        ty_uTPC = (*ny).time_uTPC;
        deltaT_uTPC = std::abs(ty_uTPC - tx_uTPC);
        if (deltaT_uTPC < minDelta_uTPC && deltaT_uTPC <= dPlane) {
          minDelta_uTPC = deltaT_uTPC;
          it_uTPC = ny;
        }
      }
    }
    if (it != end(m_tempClusterY)) {
      nx.clusterXAndY = true;
      (*it).clusterXAndY = true;

      CommonClusterNMX theCommonCluster;
      theCommonCluster.sizeX = nx.size;
      theCommonCluster.sizeY = (*it).size;
      theCommonCluster.adcX = nx.adc;
      theCommonCluster.adcY = (*it).adc;
      theCommonCluster.positionX = nx.position;
      theCommonCluster.positionY = (*it).position;
      theCommonCluster.timeX = nx.time;
      theCommonCluster.timeY = (*it).time;

      DTRACE(DEB, "\ncommon cluster x/y (center of mass):");
      DTRACE(DEB, "\tpos x/pos y: %f/%f", posx, posy);
      DTRACE(DEB, "\ttime x/time y: : %f/%f", tx, ty);
      DTRACE(DEB, "\tadc x/adc y: %u/%u", theCommonCluster.adcX,
             theCommonCluster.adcY);
      DTRACE(DEB, "\tsize x/size y: %u/%u", theCommonCluster.sizeX,
             theCommonCluster.sizeY);
      m_clusterXY.emplace_back(std::move(theCommonCluster));
    }
    if (it_uTPC != end(m_tempClusterY)) {
      nx.clusterXAndY_uTPC = true;
      (*it_uTPC).clusterXAndY_uTPC = true;

      CommonClusterNMX theCommonCluster_uTPC;
      theCommonCluster_uTPC.sizeX = nx.size;
      theCommonCluster_uTPC.sizeY = (*it_uTPC).size;
      theCommonCluster_uTPC.adcX = nx.adc;
      theCommonCluster_uTPC.adcY = (*it_uTPC).adc;
      theCommonCluster_uTPC.positionX = nx.position_uTPC;
      theCommonCluster_uTPC.positionY = (*it_uTPC).position_uTPC;
      theCommonCluster_uTPC.timeX = nx.time_uTPC;
      theCommonCluster_uTPC.timeY = (*it_uTPC).time_uTPC;

      DTRACE(DEB, "\ncommon cluster x/y (uTPC):");
      DTRACE(DEB, "\tpos x/pos y: %f/%f", posx_uTPC, posy_uTPC);
      DTRACE(DEB, "\ttime x/time y: : %f/%f", tx_uTPC, ty_uTPC);
      DTRACE(DEB, "\tadc x/adc y: %u/%u", theCommonCluster_uTPC.adcX,
             theCommonCluster_uTPC.adcY);
      DTRACE(DEB, "\tsize x/size y: %u/%u", theCommonCluster_uTPC.sizeX,
             theCommonCluster_uTPC.sizeY);
      m_clusterXY_uTPC.emplace_back(std::move(theCommonCluster_uTPC));
    }
  }
}

//====================================================================================================================
void NMXClusterer::AnalyzeClusters() {
  std::sort(begin(m_hitsOldX), end(m_hitsOldX),
            [](const HitTuple &t1, const HitTuple &t2) {
              return std::get<0>(t1) < std::get<0>(t2);
            });

  std::sort(begin(m_hitsOldY), end(m_hitsOldY),
            [](const HitTuple &t1, const HitTuple &t2) {
              return std::get<0>(t1) < std::get<0>(t2);
            });
  std::sort(begin(m_hitsX), end(m_hitsX),
            [](const HitTuple &t1, const HitTuple &t2) {
              return std::get<0>(t1) < std::get<0>(t2);
            });

  std::sort(begin(m_hitsY), end(m_hitsY),
            [](const HitTuple &t1, const HitTuple &t2) {
              return std::get<0>(t1) < std::get<0>(t2);
            });

  CorrectTriggerData(m_hitsX, m_hitsOldX, pDeltaTimeHits);
  CorrectTriggerData(m_hitsY, m_hitsOldY, pDeltaTimeHits);
  int cntX = ClusterByTime(m_hitsOldX, pDeltaTimeHits, pDeltaStripHits,
                           pDeltaTimeSpan, "x");
  int cntY = ClusterByTime(m_hitsOldY, pDeltaTimeHits, pDeltaStripHits,
                           pDeltaTimeSpan, "y");

  DTRACE(DEB, "%d cluster in x\n", cntX);
  DTRACE(DEB, "%d cluster in y\n", cntY);

  MatchClustersXY(pDeltaTimePlanes);

  m_hitsOldX = std::move(m_hitsX);
  m_hitsOldY = std::move(m_hitsY);
  assert(m_hitsX.empty());
  assert(m_hitsY.empty());

  m_clusterX.insert(m_clusterX.end(),
                    std::make_move_iterator(m_tempClusterX.begin()),
                    std::make_move_iterator(m_tempClusterX.end()));
  m_clusterY.insert(m_clusterY.end(),
                    std::make_move_iterator(m_tempClusterY.begin()),
                    std::make_move_iterator(m_tempClusterY.end()));
  m_tempClusterX.clear();
  m_tempClusterY.clear();
}

//====================================================================================================================
void NMXClusterer::CorrectTriggerData(HitContainer &hits, HitContainer &oldHits,
                                      float correctionTime) {
  if (m_subsequentTrigger) {

    const auto &itHitsBegin = begin(hits);
    const auto &itHitsEnd = end(hits);
    const auto &itOldHitsBegin = oldHits.rend();
    const auto &itOldHitsEnd = oldHits.rbegin();
    if (itHitsBegin != itHitsEnd && itOldHitsBegin != itOldHitsEnd) {
      float bcPeriod = 1000 * 4096 * (1 / (float)pBC);
      float timePrevious = std::get<0>(*itOldHitsEnd);
      float timeNext = std::get<0>(*itHitsBegin) + bcPeriod;
      float deltaTime = timeNext - timePrevious;
      // Code only executed if the first hit in hits is close enough in time to
      // the last hit in oldHits
      if (deltaTime <= correctionTime) {
        HitContainer::iterator itFind;
        // Loop through all hits in hits
        for (itFind = itHitsBegin; itFind != itHitsEnd; ++itFind) {
          // At the first iteration, timePrevious is sett to the time of the
          // first hit in hits
          timePrevious = timeNext;
          // At the first iteration, timeNext is again set to the time of the
          // first hit in hits
          timeNext = std::get<0>(*itFind) + bcPeriod;

          // At the first iteration, delta time is 0
          deltaTime = timeNext - timePrevious;

          if (deltaTime > correctionTime) {
            break;
          } else {
            oldHits.emplace_back(timeNext, std::get<1>(*itFind),
                                 std::get<2>(*itFind));
          }
        }
        // Deleting all hits that have been inserted into oldHits (up to itFind,
        // but not including itFind)
        hits.erase(itHitsBegin, itFind);
      }
    }
  }
}

//====================================================================================================================
int NMXClusterer::GetPlaneID(int chipID) {
  auto chip = std::find(begin(pXChipIDs), end(pXChipIDs), chipID);
  if (chip != end(pXChipIDs)) {
    return 0;
  } else {
    auto chip = std::find(begin(pYChipIDs), end(pYChipIDs), chipID);
    if (chip != end(pYChipIDs)) {
      return 1;
    } else {
      return -1;
    }
  }
}

//====================================================================================================================
int NMXClusterer::GetChannel(std::vector<int> &chipIDs, int chipID,
                             int channelID) {
  auto chip = std::find(begin(chipIDs), end(chipIDs), chipID);
  if (chip != end(chipIDs)) {
    return channelID + (chip - begin(chipIDs)) * 64;
  } else {
    return -1;
  }
}