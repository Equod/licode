#ifndef ERIZO_SRC_ERIZO_RTP_RTPUTILS_H_
#define ERIZO_SRC_ERIZO_RTP_RTPUTILS_H_

#include "rtp/RtpHeaders.h"

#include "./MediaDefinitions.h"

#include <stdint.h>

#include <memory>

namespace erizo {

class RtpUtils {
 public:
  static bool sequenceNumberLessThan(uint16_t first, uint16_t second);
  static int forEachRtcpPacketInBuffer(const void* buffer, int len, std::function<void(const RtcpHeader*)> f);
  static int forEachRtcpPacketInBuffer(void* buffer, int len, std::function<void(RtcpHeader*)> f);
  static bool HasPacketRtcpInBuffer(const void* buffer, size_t len, std::function<bool(const RtcpHeader*)> matcher);

  static bool numberLessThan(uint16_t first, uint16_t last, int bits);

  static void forEachRtcpBlock(std::shared_ptr<DataPacket> packet, std::function<void(RtcpHeader*)> f);

  static void updateREMB(RtcpHeader *chead, uint bitrate);

  static bool isPLI(std::shared_ptr<DataPacket> packet);

  static bool isFIR(std::shared_ptr<DataPacket> packet);

  static void forEachNack(RtcpHeader *chead, std::function<void(uint16_t, uint16_t, RtcpHeader*)> f);

  static std::shared_ptr<DataPacket> createPLI(uint32_t source_ssrc, uint32_t sink_ssrc);

  static std::shared_ptr<DataPacket> createFIR(uint32_t source_ssrc, uint32_t sink_ssrc, uint8_t seq_number);
  static std::shared_ptr<DataPacket> createREMB(uint32_t ssrc, std::vector<uint32_t> ssrc_list, uint32_t bitrate);

  static int getPaddingLength(std::shared_ptr<DataPacket> packet);

  static std::shared_ptr<DataPacket> makePaddingPacket(std::shared_ptr<DataPacket> packet, uint8_t padding_size);

  static bool isHeaHeaderPLI(const RtcpHeader* header);
  static bool isHeaHeaderREMB(const RtcpHeader* header);
  static bool isHeaHeaderFIR(const RtcpHeader* header);
  static bool isHeaHeaderNACK(const RtcpHeader* header);

  static bool HasPLI(const DataPacket& packet) { return HasPLI(packet.data, static_cast<size_t>(packet.length)); }

  static bool HasREMB(const DataPacket& packet) { return HasREMB(packet.data, static_cast<size_t>(packet.length)); }

  static bool HasFIR(const DataPacket& packet) { return HasFIR(packet.data, static_cast<size_t>(packet.length)); }

  static bool HasNACK(const DataPacket& packet) { return HasNACK(packet.data, static_cast<size_t>(packet.length)); }

 protected:
  static bool HasPLI(const void* buffer, size_t len);
  static bool HasREMB(const void* buffer, size_t len);
  static bool HasFIR(const void* buffer, size_t len);
  static bool HasNACK(const void* buffer, size_t len);
};

}  // namespace erizo

#endif  // ERIZO_SRC_ERIZO_RTP_RTPUTILS_H_
