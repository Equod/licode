#ifndef ERIZO_SRC_ERIZO_RTP_PACKETPRINTUTILS_H_
#define ERIZO_SRC_ERIZO_RTP_PACKETPRINTUTILS_H_

#include <string>
#include "MediaDefinitions.h"
#include "rtp/RtpHeaders.h"

namespace erizo
{

class RtcpPayloadNameMap {
 public:
  static std::string getNameByCode(uint8_t code);
};

struct RtcpHeaderOnly {
  struct PacketType { enum Code {
	  Fir = 192,		// full INTRA-frame request
	  Nack = 193,		// negative acknowledgement
	  Smptetc = 194,	// SMPTE time-code mapping
	  Ij = 195,		// extended inter-arrival jitter report
	  Sr = 200,		// sender report
	  Rr = 201,		// receiver report
	  Sdes = 202, 	// source description
	  Bye = 203,		// goodbye
	  App = 204,		// application defined
	  Rtpfb = 205,	// Generic RTP Feedback
	  Psfb = 206,		// Payload-specific
	  Xr = 207,		// RTCP extension
	  Avb = 208,		// AVB RTCP packet
	  Rsi = 209		// Receiver Summary Information
	};};
  uint32_t blockcount :5;
  uint32_t padding :1;
  uint32_t version :2;
  uint32_t packettype :8;
  uint32_t length :16;
  uint8_t getBlockcount() const {
	  return (uint8_t)blockcount;
  }
  bool getPadding() const {
	  return padding != 0;
  }
  uint8_t getVersion() const {
	  return (uint8_t)version;
  }
  uint8_t getPacketType() const {
	  return (uint8_t)packettype;
  }
  uint16_t getLength() const {
	  return ntohs((uint16_t)length);
  }
  uint16_t getPacketLengthInBytes() const {
	  return (getLength() + uint16_t(1)) * uint16_t(4);
  }
  static bool IsValidPacketType(uint8_t packet_type) {
	  return (packet_type >= PacketType::Fir && packet_type <= PacketType::Ij) ||
			  (packet_type >= PacketType::Sr && packet_type <= PacketType::Rsi);
  }
  bool IsValidPacketType() const {
	  return IsValidPacketType(getPacketType());
  }
  bool IsValid() const {
	  return getVersion() == 2 && IsValidPacketType();
  }

  friend std::ostream& operator<<(std::ostream& os, const RtcpHeaderOnly& header) {
	  os 	<< "Header:\n"
			<< "    version: " << (int)header.getVersion() << "\n"
			<< "    padding: " << (header.getPadding()? "true" : "false") << "\n"
			<< "    count: " << (int)header.getBlockcount() << "\n"
			<< "    type: " << (int)header.getPacketType() << " - "
			<< RtcpPayloadNameMap::getNameByCode(header.getPacketType()) << "\n"
			<< "    length: " << (int)header.getLength() << "\n";
	  return os;
  }
};

    std::string packet_to_str(const DataPacket&);

    int parse_as_rtp(std::ostream& stream, const void* buffer, int max_size);
	int parse_as_rtcp(std::ostream& stream, const void* buffer, int max_size);
	int parse_rtp_extention(std::ostream& stream, const void* buffer, int max_size);

    std::string rtcp_to_str(const RtcpHeader&);

    std::string rtp_to_str(const RtpHeader&);
}

#endif
