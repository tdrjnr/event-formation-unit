/** Copyright (C) 2016, 2017 European Spallation Source ERIC */

#include <gdgem/vmm2srs/SRSMappings.h>
#include <sstream>

// This is done only once, when starting EFU
void SRSMappings::define_plane(
    uint8_t planeID,
    std::initializer_list<std::pair<uint16_t, uint16_t>> chips) {
  int offset = 0;
  for (auto c : chips) {
    set_mapping(c.first, c.second, planeID, offset);
    offset += NMX_CHIP_CHANNELS;
  }
}

// This is done only once, when starting EFU
void SRSMappings::set_mapping(uint16_t fecID, uint16_t vmmID, uint8_t planeID,
                              uint16_t strip_offset) {
  if (vmmID >= NMX_MAX_CHIPS)
    return;

  if (offsets_.size() <= fecID) {
    for (int i = offsets_.size(); i <= fecID; ++i) {
      offsets_.resize(i + 1);
      offsets_[i] = std::vector<uint16_t>(NMX_MAX_CHIPS, NMX_INVALID_GEOM_ID);
      planes_.resize(i + 1);
      planes_[i] = std::vector<uint8_t>(NMX_MAX_CHIPS, NMX_INVALID_PLANE_ID);
    }
  }
  offsets_[fecID][vmmID] = strip_offset;
  planes_[fecID][vmmID] = planeID;
}

uint16_t SRSMappings::get_strip(uint16_t fecID, uint16_t vmmID,
                                uint32_t channelID) const {
  if (fecID >= offsets_.size())
    return NMX_INVALID_GEOM_ID;
  const auto &fec = offsets_[fecID];
  if (vmmID >= fec.size())
    return NMX_INVALID_GEOM_ID;
  const auto &chip = fec[vmmID];
  if (chip != NMX_INVALID_GEOM_ID)
    return chip + channelID;
  return NMX_INVALID_GEOM_ID;
}

uint8_t SRSMappings::get_plane(uint16_t fecID, uint16_t vmmID) const {
  if (fecID >= planes_.size())
    return NMX_INVALID_PLANE_ID;
  const auto &fec = planes_[fecID];
  if (vmmID >= fec.size())
    return NMX_INVALID_PLANE_ID;
  return fec[vmmID];
}

std::string SRSMappings::debug() const {
  std::stringstream ss;
  for (size_t i = 0; i < planes_.size(); ++i) {
    for (size_t j = 0; j < planes_[i].size(); ++j) {
      if (planes_[i][j] == NMX_INVALID_PLANE_ID)
        continue;
      ss << "    (FEC=" << i << ",VMM=" << j << ") --> "
         << "(plane=" << planes_[i][j] << ","
         << " strips=[" << offsets_[i][j] << "-"
         << offsets_[i][j] + NMX_CHIP_CHANNELS - 1 << "])\n";
    }
  }
  return ss.str();
}
