/** Copyright (C) 2016 European Spallation Source ERIC */

/** @file
 *
 *  @brief structure/class containing global detector pixel id and timestamp for cspec
 */

#pragma once
#include <cstdint>
#include <cstdio>

class CSPECEvent {
public:

   /** @brief construct an event
    *  @param timestamp Global time (in whatever units they come)
    *  @param detectorid Global detector pixel ID
    */
   CSPECEvent(uint64_t timestamp, uint32_t globalpixid)
      :  time_(timestamp)
      ,  pixelid_(globalpixid) {}

   ~CSPECEvent() {
     time_ = 0;
     pixelid_ = 0x0;
   }

   /** @brief return pixelid */
   inline uint32_t getpixelid() {return pixelid_; }

   /** @brief return time */
   inline uint64_t gettimestamp() {return time_; }

   /** @brief lessthan operator needed for sorting
    *  @param other Event to compare against
    */
   bool operator<(CSPECEvent other) const {
     return pixelid_ < other.getpixelid();
   }

   /** @brief greaterthan operator needed for sorting
    *  @param other Event to compare against
    */
   bool operator>(CSPECEvent other) const {
     return pixelid_ > other.getpixelid();
   }

private:
   uint64_t time_;
   uint32_t pixelid_;
};
