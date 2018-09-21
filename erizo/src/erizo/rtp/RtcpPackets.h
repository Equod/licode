//
// Created by bandyer on 10/09/18.
//

#ifndef ERIZO_ALL_RTCPPACKETS_H
#define ERIZO_ALL_RTCPPACKETS_H

#include <vector>
#include "RtpHeaders.h"

namespace erizo {

class RtcpPackets : public std::vector<erizo::RtcpHeader*> {
 public:
  void parse(void* data, int len);
};

}

#endif //ERIZO_ALL_RTCPPACKETS_H
