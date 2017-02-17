/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

#include <cstring>
#include <nmxgen/ReaderVMM.h>
#include <nmxgen/vmm_nugget.h>

ReaderVMM::ReaderVMM(std::string filename) {
  if (filename.empty())
    return;
  file_ = H5CC::File(filename, H5CC::Access::r_existing);
  if (!file_.has_group("RawVMM") ||
      !file_.open_group("RawVMM").has_dataset("points"))
    return;
  dataset_ = file_.open_group("RawVMM").open_dataset("points");

  auto shape = dataset_.shape();
  if ((shape.rank() != 2) || (shape.dim(1) != 4))
    return;

  total_ = shape.dim(0);
}

size_t ReaderVMM::read(char *buf) {

  vmm_nugget packet;

  size_t psize = sizeof(packet);

  size_t limit = std::min(current_ + (9000 / psize), total_);
  size_t byteidx = 0;
  for (; current_ < limit; ++current_) {
    index[0] = current_;
    auto data = dataset_.read<uint32_t>(slabsize, index);

    packet.time = (static_cast<uint64_t>(data.at(0)) << 32) |
                  static_cast<uint64_t>(data.at(1));
    packet.plane_id = (data.at(2) >> 16);
    packet.strip = static_cast<uint8_t>(data.at(2) & 0x0000FFFF);
    packet.adc = static_cast<uint16_t>(data.at(3));

    memcpy(buf, &packet, sizeof(packet));

    buf += psize;
    byteidx += psize;
  }
  return byteidx;
}
