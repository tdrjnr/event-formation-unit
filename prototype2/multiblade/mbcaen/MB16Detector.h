/** Copyright (C) 2017 European Spallation Source ERIC */

/** @file
 *
 *  @brief Multiblade geometry
 */

#pragma once
#include <cstdio>

class MB16Detector {
public:
  inline int cassette(int digid) {
    switch (digid) {
    case 137:
      return 0;
      break;
    case 143:
      return 1;
      break;
    case 142:
      return 2;
      break;
    case 31:
      return 3;
      break;
    case 34:
      return 4;
      break;
    case 33:
      return 5;
      break;
    default:
      return -1;
    }
  }
};
