//
// Created by bandyer on 10/09/18.
//

#include "RtcpPackets.h"
#include "PacketPrintUtils.h"

using erizo::RtcpPackets;
using erizo::RtcpHeader;
using erizo::RtcpHeaderOnly;

void RtcpPackets::parse(void* data, int len) {
  clear();
  auto buffer = reinterpret_cast<char*>(data);
  while (len > 0) {
    if(len > (int)sizeof(RtcpHeaderOnly)) {
      auto header_only = reinterpret_cast<RtcpHeaderOnly*>(buffer);
      if(header_only->IsValid() && header_only->getLength() > 0) {
        push_back(reinterpret_cast<RtcpHeader*>(header_only));
      }
      int read = (header_only->getLength() + 1) * 4;
      buffer += read;
      len -= read;
    } else {
      len = 0;
    }
  }
}
