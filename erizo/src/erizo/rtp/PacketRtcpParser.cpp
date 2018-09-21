//
// Created by bandyer on 07/09/18.
//

#include "PacketRtcpParser.h"

using erizo::PacketRtcpParser;

void PacketRtcpParser::read(Context* ctx, std::shared_ptr<DataPacket> packet) {
  packet->parse_rtcp_packets();
  ctx->fireRead(std::move(packet));
}

void erizo::PacketRtcpParser::enable() {
}

void erizo::PacketRtcpParser::disable() {
}

std::string erizo::PacketRtcpParser::getName() {
  return "PacketRtcpParser";
}

void erizo::PacketRtcpParser::notifyUpdate() {
}
