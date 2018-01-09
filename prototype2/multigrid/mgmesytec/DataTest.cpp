/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

//#include <algorithm>
//#include <memory>
#include <gdgem/nmx/Hists.h> // @fixme
#include <multigrid/mgmesytec/Data.h>
#include <multigrid/mgmesytec/TestData.h>
#include <test/TestBase.h>

class MesytecDataTest : public TestBase {
protected:
  NMXHists hists;
  MesytecData mesytec;
  virtual void SetUp() {}
  virtual void TearDown() {}
};

/** Test cases below */

TEST_F(MesytecDataTest, ErrUnsupportedCommand) {
  auto res = mesytec.parse((char *)&err_unsupported_cmd[0], err_unsupported_cmd.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::EUNSUPP);

  res = mesytec.parse((char *)&err_unsupported_cmd_II[0], err_unsupported_cmd_II.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::EUNSUPP);
}

TEST_F(MesytecDataTest, ErrNoSisReadoutHeader) {
  auto res = mesytec.parse((char *)&err_no_sis_readout_header[0], err_no_sis_readout_header.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::EHEADER);
}

TEST_F(MesytecDataTest, ErrNoSisReadoutTrailer) {
  auto res = mesytec.parse((char *)&err_no_sis_readout_trailer[0], err_no_sis_readout_trailer.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::EHEADER);
}

TEST_F(MesytecDataTest, ErrNoTimeStamp) {
  auto res = mesytec.parse((char *)&err_no_timestamp[0], err_no_timestamp.size(), hists);
  ASSERT_EQ(res, MesytecData::error::OK);
  ASSERT_EQ(mesytec.readouts, 0);
}

TEST_F(MesytecDataTest, ErrNoEndDataCookie) {
  auto res = mesytec.parse((char *)&err_no_end_data_cookie[0], err_no_end_data_cookie.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::EHEADER);
}

TEST_F(MesytecDataTest, ErrPktShort) {
  auto res = mesytec.parse((char *)&err_pkt_too_short[0], err_pkt_too_short.size(), hists);

  ASSERT_EQ(res, -MesytecData::error::ESIZE);
}

TEST_F(MesytecDataTest, ParseRecordedWSData) {
  auto res = mesytec.parse((char *)&ws1[0], ws1.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::OK);
  ASSERT_EQ(mesytec.readouts, 128);
  ASSERT_EQ(mesytec.discards, 0);
}

TEST_F(MesytecDataTest, ParseRecordedWSDataDiscardAll) {
  mesytec.setAdcThreshold(60000); // Above all readouts
  auto res = mesytec.parse((char *)&ws1[0], ws1.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::OK);
  ASSERT_EQ(mesytec.readouts, 128);
  ASSERT_EQ(mesytec.discards, 128);
}

TEST_F(MesytecDataTest, ParseRecordedWSDataII) {
  auto res = mesytec.parse((char *)&ws2[0], ws2.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::OK);
  ASSERT_EQ(mesytec.readouts, 256);
}

TEST_F(MesytecDataTest, ParseRecordedWSDataIII) {
  auto res = mesytec.parse((char *)&ws3[0], ws3.size(), hists);
  ASSERT_EQ(res, -MesytecData::error::OK);
  ASSERT_EQ(mesytec.readouts, 256); // Readout provides more than 92 channels
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}