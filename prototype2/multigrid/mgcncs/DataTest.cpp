/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

#include <algorithm>
#include <memory>
#include <multigrid/mgcncs/ChanConv.h>
#include <multigrid/mgcncs/Data.h>
#include <multigrid/mgcncs/MultigridGeometry.h>
#include <multigrid/mgcncs/TestData.h>
#include <test/TestBase.h>

class CspecDataTest : public TestBase {
protected:
  CSPECChanConv conv{};

  MultiGridGeometry *cspecgeometry;

  std::unique_ptr<CSPECData> dat;
  char buffer[9000];
  int size;

  virtual void SetUp() {
    cspecgeometry = new MultiGridGeometry(1, 80, 160, 4, 16);
    dat = std::unique_ptr<CSPECData>(new CSPECData(250, &conv, cspecgeometry));
  }
  virtual void TearDown() { delete cspecgeometry; }

  void assertdatanderr(int data, int error) {
    ASSERT_EQ(dat->elems, data);
    ASSERT_EQ(dat->error, error);
  }
};

/** Test cases below */
TEST_F(CspecDataTest, Constructor) {
  assertdatanderr(0, 0);
  ASSERT_LT(dat->wire_thresh, 16384);
  ASSERT_LT(dat->grid_thresh, 16384);
  ASSERT_EQ(dat->datasize, 40);
}

TEST_F(CspecDataTest, ValidData) {
  for (auto v : ok) {
    size = v.size() * 4;
    MESSAGE() << "Size of current data: " << size << std::endl;
    dat->receive((char *)&v[0], size);
    assertdatanderr(size / dat->datasize, 0);

    for (unsigned int i = 0; i < (dat->elems); i++) {
      ASSERT_EQ(dat->data[i].module, i);
    }
  }
}

TEST_F(CspecDataTest, InvalidData) {
  for (auto v : err_pkt) {
    size = v.size() * 4;
    dat->receive((char *)&v[0], size);
    assertdatanderr(0, 40);
  }
}

TEST_F(CspecDataTest, OverUndersizeData) {
  for (auto v : err_size) {
    size = v.size() * 4;
    dat->receive((char *)&v[0], size);
    assertdatanderr(size / dat->datasize, size - dat->elems * 40);
  }
}

TEST_F(CspecDataTest, ValidateGenerator) {
  for (int i = 1; i < 225; i++) {
    size = dat->generate(buffer, 9000, i, 0, 0);
    dat->receive(buffer, size);
    assertdatanderr(i, 0);
  }
}
/** Test for oversize specification */
TEST_F(CspecDataTest, GeneratorOversize) {
  size = dat->generate(buffer, 9000, 300, 0, 0);
  dat->receive(buffer, size);
  assertdatanderr(225, 0);
}

TEST_F(CspecDataTest, InputFilterBelowThresh) {
  size = err_below_thresh.size() * 4;
  dat->receive((char *)&err_below_thresh[0], size);
  assertdatanderr(size / dat->datasize, 0);
  int discard = dat->input_filter();
  ASSERT_EQ(discard, 3);
}

TEST_F(CspecDataTest, InputFilterMisordered) {
  size = err_misorder.size() * 4;
  dat->receive((char *)&err_misorder[0], size);
  assertdatanderr(0, 40);
}

TEST_F(CspecDataTest, InputFilterBadNwords) {
  size = err_nwords.size() * 4;
  dat->receive((char *)&err_nwords[0], size);
  assertdatanderr(0, 40);
}

TEST_F(CspecDataTest, InputFilterNoEvents) {
  char buffer[200];
  memset(buffer, 0, sizeof(buffer));
  const int databytes = 40;
  dat->receive(buffer, databytes);
  assertdatanderr(0, databytes);
  int discard = dat->input_filter();
  ASSERT_EQ(discard, 0);
}

TEST_F(CspecDataTest, CreateEventInvalidPixel) {
  char buffer[32];
  struct CSPECData::MultiGridData data;

  std::fill_n(buffer, sizeof(buffer), 0x00);
  data.module = 1;
  data.time = 0xaabbccdd;
  data.d[2] = 1;
  data.d[6] = 1;

  conv.makewirecal(0, 16383, 1);
  conv.makegridcal(0, 16383, 1);
  uint32_t time;
  uint32_t pixel;
  auto ret = dat->createevent(data, &time, &pixel);

  ASSERT_EQ(ret, -1);
}

TEST_F(CspecDataTest, CreateEventValidPixel) {
  struct CSPECData::MultiGridData data;
  data.module = 1;
  data.time = 0xaabbccdd;
  data.d[2] = 5106;
  data.d[6] = 1;

  uint32_t time;
  uint32_t pixel;
  auto ret = dat->createevent(data, &time, &pixel);
  ASSERT_EQ(ret, 0);
  ASSERT_EQ(time, 0xaabbccdd);
  EXPECT_EQ(pixel, 1);

  data.module = 1;
  data.time = 0xaabbaadd;
  data.d[2] = 5105;
  data.d[6] = 1;

  ret = dat->createevent(data, &time, &pixel);
  ASSERT_EQ(ret, 0);
  ASSERT_EQ(time, 0xaabbaadd);
  EXPECT_EQ(pixel, 2);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
