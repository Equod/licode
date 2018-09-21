#include "rtp/PacketPrintUtils.h"
#include "RtpHeaders.h"
#include "PacketPrintUtils.h"

#include <sstream>
#include <iostream>
#include <ios>
#include <bitset>

namespace erizo {
typedef int packet_length;

struct RtpHeaderOnly {
  uint32_t cc :4;
  uint32_t hasextension :1;
  uint32_t padding :1;
  uint32_t version :2;
  uint32_t payloadtype :7;
  uint32_t marker :1;
  uint32_t seqnum :16;
  uint32_t timestamp;
  uint32_t ssrc;
  uint8_t getCc() const {
	  return (uint8_t) cc;
  }
  bool HasExtension() const {
	  return hasextension != 0;
  }
  bool HasPadding() const {
	  return padding != 0;
  }
  uint8_t getVersion() const {
	  return (uint8_t) version;
  }
  uint8_t getPayloadtype() const {
	  return (uint8_t) payloadtype;
  }
  bool HasMarker() const {
	  return marker != 0;
  }
  uint16_t getSeqnum() const {
	  return ntohs(seqnum);
  }
  uint32_t getTimestamp() const {
	  return ntohl(timestamp);
  }
  uint32_t getSsrc() const {
	  return ntohl(ssrc);
  }
  friend std::ostream& operator<<(std::ostream& os, const RtpHeaderOnly& header) {
	  os << "Header: \n"
		 << "    version: " << (int) header.getVersion() << "\n"
		 << "    has padding: " << (header.HasPadding() ? "true" : "false") << "\n"
		 << "    has extention: " << (header.HasExtension() ? "true" : "false") << "\n"
		 << "    cc: " << (int) header.getCc() << "\n"
		 << "    marker: " << (header.HasMarker() ? "true" : "false") << "\n"
		 << "    type: " << (int) header.getPayloadtype() << "\n"
		 << "    sequence number: " << header.getSeqnum() << "\n"
		 << "    timestamp: " << header.getTimestamp() << "\n"
		 << "    synchronization source: " << header.getSsrc() << "\n";
	  return os;
  }
};

struct Ssrc {
  uint32_t sender_ssrc;
  uint32_t getSender_ssrc() const {
	  return htonl(sender_ssrc);
  }
  friend std::ostream& operator<<(std::ostream& os, const Ssrc& ssrc) {
	  os << "Ssrc:\n"
		 << "    ssrc: " << ssrc.getSender_ssrc() << "\n";
	  return os;
  }
};

struct SenderInfo {
  Ssrc sender_ssrc;
  uint32_t ntp_timestamp_msb;
  uint32_t ntp_timestamp_lsb;
  uint32_t rtp_timestamp;
  uint32_t packet_count;
  uint32_t octet_count;
  const Ssrc& getSender_ssrc() const {
	  return sender_ssrc;
  }
  uint32_t getNtp_timestamp_msb() const {
	  return ntohl(ntp_timestamp_msb);
  }
  uint32_t getNtp_timestamp_lsb() const {
	  return ntohl(ntp_timestamp_lsb);
  }
  uint32_t getRtp_timestamp() const {
	  return ntohl(rtp_timestamp);
  }
  uint32_t getPacket_count() const {
	  return ntohl(packet_count);
  }
  uint32_t getOctet_count() const {
	  return ntohl(octet_count);
  }
  uint64_t getNtp_timestamp() const {
	  return (uint64_t(getNtp_timestamp_msb()) << 32) + getNtp_timestamp_lsb();
  }
  friend std::ostream& operator<<(std::ostream& os, const SenderInfo& info) {
	  os << info.getSender_ssrc()
		 << "Sender info:\n"
		 << "    ntp_timestamp: " << info.getNtp_timestamp() << "\n"
		 << "    rtp timestamp: " << info.getRtp_timestamp() << "\n"
		 << "    packet count: " << info.getPacket_count() << "\n"
		 << "    octet count " << info.getOctet_count() << "\n";
	  return os;
  }
};

struct RtcpReport {
  uint32_t ssrcsource;
  /* RECEIVER REPORT DATA*/
  uint32_t fractionlost:8;
  uint32_t lost:24;
  uint32_t seqnumcycles:16;
  uint32_t highestseqnum:16;
  uint32_t jitter;
  uint32_t lastsr;
  uint32_t delaysincelast;
  uint32_t getSsrcsource() const {
	  return ntohl(ssrcsource);
  }
  uint8_t getFractionlost() const {
	  return fractionlost;
  }
  uint32_t getLost() const {
	  return ntohl(lost);
  }
  uint16_t getSeqnumcycles() const {
	  return ntohs(seqnumcycles);
  }
  uint16_t getHighestseqnum() const {
	  return ntohs(highestseqnum);
  }
  uint32_t getJitter() const {
	  return ntohl(jitter);
  }
  uint32_t getLastsr() const {
	  return ntohl(lastsr);
  }
  uint32_t getDelaysincelast() const {
	  return ntohl(delaysincelast);
  }

  friend std::ostream& operator<<(std::ostream& os, const RtcpReport& report) {
	  os << "SSRC: " << report.getSsrcsource() << "\n"
		 << "    fraction lost: " << (int) report.getFractionlost() << "\n"
		 << "    cumulative number of packets lost: " << (int) report.getLost() << "\n"
		 << "    seqnumcycles: " << report.getSeqnumcycles() << "\n"
		 << "    highestseqnum: " << report.getHighestseqnum() << "\n"
		 << "    interarrival jitter: " << report.getJitter() << "\n"
		 << "    last SR (LSR): " << report.getLastsr() << "\n"
		 << "    delay since last SR (DLSR): " << report.getDelaysincelast() << "\n";
	  return os;
  }
};

struct RembPayload {
  uint32_t uniqueid;
  uint32_t numssrc:8;
  uint32_t brLength :24;
  uint32_t ssrcfeedb[1];
  std::string getUniqueid() const {
	  char res[5] = "REMB";
	  memcpy(res, &uniqueid, 4);
	  return res;
  }
  uint8_t getNumssrc() const {
	  return (uint8_t) numssrc;
  }
  uint32_t getBrLength() const {
	  return ntohl(brLength);
  }
  uint32_t getSsrcfeedb(size_t index = 0) const {
	  return ntohl(ssrcfeedb[0]);
  }
  inline uint64_t getREMBBitRate() const {
	  return getBrMantis() << getBrExp();
  }

  inline uint32_t getBrExp() const {
	  // remove the 0s added by nothl (8) + the 18 bits of Mantissa
	  return (ntohl(brLength) >> 26);
  }
  inline uint32_t getBrMantis() const {
	  return (ntohl(brLength) >> 8 & 0x3ffff);
  }
  friend std::ostream& operator<<(std::ostream& os, const RembPayload& payload) {
	  os << "Remb data:\n"
		 << "    unique id: " << payload.getUniqueid() << "\n"
		 << "    Num SSRC: " << (int) payload.getNumssrc() << "\n"
		 << "    Estimation: " << (int) payload.getREMBBitRate() << "\n";
	  for (uint8_t i = 0; i < payload.getNumssrc(); ++i)
		  os << "    SSRC(" << (int) i << ") feedback: " << payload.getSsrcfeedb(i) << "\n";
	  return os;
  }
};

struct SliFeedback {
  SliFeedback(const void* net_buffer_data) {
	  uint32_t val;
	  memcpy(&val, net_buffer_data, 4);
	  val = ntohl(val);
	  memcpy(this, &val, 4);
  }
  uint32_t first:13;
  uint32_t number:13;
  uint32_t pictuire_id:6;
  uint32_t getFirst() const {
	  return first;
  }
  void setFirst(uint32_t first) {
	  SliFeedback::first = first;
  }
  uint32_t getNumber() const {
	  return number;
  }
  void setNumber(uint32_t number) {
	  SliFeedback::number = number;
  }
  uint32_t getPictuire_id() const {
	  return pictuire_id;
  }
  void setPictuire_id(uint32_t pictuire_id) {
	  SliFeedback::pictuire_id = pictuire_id;
  }

  friend std::ostream& operator<<(std::ostream& os, const SliFeedback& feedback) {
	  os << "SLI:\n"
		 << "    First: " << feedback.getFirst() << "\n"
		 << "    Number: " << feedback.getNumber() << "\n"
		 << "    PictureID" << feedback.getPictuire_id() << "\n";
	  return os;
  }
};

struct RtpExtentionHeader {
  uint32_t extension_code:16;
  uint32_t extension_length:16;
  uint16_t getExtension_code() const {
	  return ntohs(static_cast<uint16_t>(extension_code));
  }
  uint16_t getExtension_length() const {
	  return ntohs(static_cast<uint16_t>(extension_length));
  }
  friend std::ostream& operator<<(std::ostream& os, const RtpExtentionHeader& header) {
	  os << "Extention:" << "\n"
		 << "    code: " << header.getExtension_code() << "\n"
		 << "    length: " << header.getExtension_length() << "\n";
	  return os;
  }
};

struct RtcpNack {
  uint32_t pid:16;
  uint32_t blp:16;
  uint16_t getPid() const {
	  return ntohs(pid);
  }
  uint16_t getBlp() const {
	  return ntohs(blp);
  }
  friend std::ostream& operator<<(std::ostream& os, const RtcpNack& nack) {
	  os << "NACK block:\n"
		 << "    pid: " << nack.getPid() << "\n"
		 << "    blp: " << std::bitset<16>(nack.getBlp()) << "\n";
	  return os;
  }
};

std::string getTransportLayerFmtNameByCode(uint8_t code) {
	switch (code) {
		case 1:return "Generic NACK";
		case 31:return "reserved for future expansion of the identifier number space";
		default:return "unassigned";
	}
}

std::string getPayloadSpecificFmtNameByCode(uint8_t code) {
	switch (code) {
		case 1: return "Picture Loss Indication (PLI)";
		case 2: return "Slice Loss Indication (SLI)";
		case 3: return "Reference Picture Selection Indication (RPSI)";
		case 15: return "Application layer FB (AFB) message";
		case 31: return "reserved for future expansion of the sequence number space";
		default: return "unassigned";
	}
}

namespace {
//        packet_length rtcp_block_to_sstream(std::stringstream& str_stream,
//                const unsigned char* rtcp_block,
//                int block_index);
//
//        void sr_packet_to_sstream(std::ostream& str_stream,
//								  const std::string& padding,
//								  const RtcpHeader& data,
//								  size_t report_blocks);
//        void rr_packet_to_sstream(std::ostream& stream, const std::string& padding, const RtcpHeader& data);
//        void sdes_packet_to_sstream(std::stringstream& str_stream, const std::string& padding, const RtcpHeader& data);
//        void ps_feedback_to_sstream(std::stringstream& str_stream, const std::string& padding, const RtcpHeader& data);
//        void tl_feedback_to_sstream(std::stringstream& str_stream, const std::string& padding, const RtcpHeader& data);
}

namespace {
template<unsigned Pt>
std::ostream& rtcp_packet_data_to_stream(std::ostream& os,
										 const RtcpHeaderOnly* packet_header,
										 const void* packet_data,
										 int packet_data_length) {
	os << "Rtcp packet payload of type " << Pt << " is not yet implemented\n";
	return os;
}

template<>
std::ostream& rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Sr>(std::ostream& os,
																		 const RtcpHeaderOnly* packet_header,
																		 const void* packet_data,
																		 int packet_data_length) {
	if (packet_data_length
			>= (int) sizeof(SenderInfo) + (int) sizeof(RtcpReport) * (int) packet_header->getBlockcount()) {
		const char* data = (const char*) packet_data;
		os << *(reinterpret_cast<const SenderInfo*>(data));
		data += sizeof(SenderInfo);
		for (uint8_t i = 0; i < packet_header->getBlockcount(); ++i) {
			os << "Report block [ " << i + 1 << " / " << (int) packet_header->getBlockcount() << " ] "
			   << (*reinterpret_cast<const RtcpReport*>(data));
			data += sizeof(RtcpReport);
		}
	} else {
		os << "Parsing sr error: not enough len. Expected: " << sizeof(SenderInfo) << " + "
		   << (int) sizeof(RtcpReport) << " * " << (int) packet_header->getBlockcount() << " = "
		   << (int) sizeof(SenderInfo) + (int) sizeof(RtcpReport) * (int) packet_header->getBlockcount()
		   << ". Actually provided = " << packet_data_length << "\n";
	}
	return os;
}

template<>
std::ostream& rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Rr>(std::ostream& os,
																		 const RtcpHeaderOnly* packet_header,
																		 const void* packet_data,
																		 int packet_data_length) {
	if (packet_data_length >= (int) sizeof(Ssrc) + (int) sizeof(RtcpReport) * (int) packet_header->getBlockcount()) {
		const char* data = (const char*) packet_data;
		os << *(reinterpret_cast<const Ssrc*>(data));
		data += sizeof(Ssrc);
		for (uint8_t i = 0; i < packet_header->getBlockcount(); ++i) {
			os << "Report block [ " << i + 1 << " / " << (int) packet_header->getBlockcount() << " ] "
			   << (*reinterpret_cast<const RtcpReport*>(data));
			data += sizeof(RtcpReport);
		}
	} else {
		os << "Parsing rr error: not enough len. Expected: " << sizeof(Ssrc) << " + "
		   << (int) sizeof(RtcpReport) << " * " << (int) packet_header->getBlockcount() << " = "
		   << (int) sizeof(Ssrc) + (int) sizeof(RtcpReport) * (int) packet_header->getBlockcount()
		   << ". Actually provided = " << packet_data_length << "\n";
	}
	return os;
}

template<>
std::ostream& rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Psfb>(std::ostream& os,
																		   const RtcpHeaderOnly* packet_header,
																		   const void* packet_data,
																		   int packet_data_length) {
	os << "Payload-Specific Feedback Message:\n"
	   << "    fmt: " << getPayloadSpecificFmtNameByCode(packet_header->getBlockcount()) << " ["
	   << (int) packet_header->getBlockcount() << "]\n";
	auto data = reinterpret_cast<const char*>(packet_data);
	auto ssrc = reinterpret_cast<const Ssrc*>(data);
	os << *ssrc;
	data += sizeof(Ssrc);
	uint32_t media_ssrc;
	memcpy(&media_ssrc, data, sizeof(Ssrc));
	media_ssrc = ntohl(media_ssrc);
	os << "SSRC of media source:\n"
	   << "    ssrc:" << media_ssrc << "\n";
	data += 4;
	switch (packet_header->getBlockcount()) {  // FMT
		case RTCP_AFB: return os << *(reinterpret_cast<const RembPayload*>(data));
		case RTCP_PLI_FMT:return os;
		case RTCP_SLI_FMT: {
			for (int i = 2; i < packet_header->getLength(); ++i) {
				os << "[" << i - 1 << "/" << packet_header->getLength() - 2 << "] " << SliFeedback(data);
				data += 4;
			}
			return os;
		}
		case 3: return os << "    (implement me :) )\n";
		default:return os;
	}
}

template<>
std::ostream& rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Sdes>(std::ostream& os,
																		   const RtcpHeaderOnly* packet_header,
																		   const void* packet_data,
																		   int packet_data_length) {
	const auto* data = (const uint8_t*) packet_data;
	for (uint8_t i = 0; i < packet_header->getBlockcount(); ++i) {
		auto ssrc = reinterpret_cast<const Ssrc*>(data);
		os << *ssrc;
		data += sizeof(Ssrc);
		uint8_t type = *data;
		os << "    type: " << (int) type << " - ";
		switch (type) {
			case 1: os << "CNAME";
			break;
			case 2: os << "NAME";
			break;
			case 3: os << "EMAIL";
			break;
			case 4: os << "PHONE";
			break;
			case 5: os << "LOC";
			break;
			case 6: os << "TOOL";
			break;
			case 7: os << "NOTE";
			break;
			default: os << "UNKNOWN";
		}
		os << "\n";
		data++;
		uint8_t len = *data;
		os << "    len: " << (int) len << "\n";
		data++;
		char str[256] = {0};
		memcpy(str, data, len);
		os << "    text: " << str << "\n";
	}
	return os;
}

template<>
std::ostream& rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Rtpfb>(std::ostream& os,
																			const RtcpHeaderOnly* packet_header,
																			const void* packet_data,
																			int packet_data_length) {
	auto data = reinterpret_cast<const char*>(packet_data);
	auto ssrc = reinterpret_cast<const Ssrc*>(data);
	os << *ssrc;
	data += sizeof(Ssrc);
	uint32_t media_ssrc;
	memcpy(&media_ssrc, data, sizeof(Ssrc));
	media_ssrc = ntohl(media_ssrc);
	os << "Feedback message type:\n"
	   << "    fmt: " << getTransportLayerFmtNameByCode(packet_header->getBlockcount())
	   << " [" << (int) packet_header->getBlockcount() << "]\n";
	os << "SSRC of media source:\n"
	   << "    ssrc:" << media_ssrc << "\n";
	data += 4;
	if (packet_header->getBlockcount() == 1) {
		for (int data_len = 2; data_len < packet_header->getLength(); ++data_len) {
			auto nack = reinterpret_cast<const RtcpNack*>(data);
			os << *nack;
			data += sizeof(RtcpNack);
		}
	}
	return os;
}

std::ostream& rtcp_packet_data_to_stream(uint8_t packet_type,
										 std::ostream& os,
										 const RtcpHeaderOnly* packet_header,
										 const void* packet_data,
										 int packet_data_length) {
	switch (packet_type) {
		case RtcpHeaderOnly::PacketType::Fir:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Fir>(os,
																			   packet_header,
																			   packet_data,
																			   packet_data_length);
		case RtcpHeaderOnly::PacketType::Nack:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Nack>(os,
																				packet_header,
																				packet_data,
																				packet_data_length);
		case RtcpHeaderOnly::PacketType::Smptetc:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Smptetc>(os,
																				   packet_header,
																				   packet_data,
																				   packet_data_length);
		case RtcpHeaderOnly::PacketType::Ij:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Ij>(os,
																			  packet_header,
																			  packet_data,
																			  packet_data_length);
		case RtcpHeaderOnly::PacketType::Sr:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Sr>(os,
																			  packet_header,
																			  packet_data,
																			  packet_data_length);
		case RtcpHeaderOnly::PacketType::Rr:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Rr>(os,
																			  packet_header,
																			  packet_data,
																			  packet_data_length);
		case RtcpHeaderOnly::PacketType::Sdes:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Sdes>(os,
																				packet_header,
																				packet_data,
																				packet_data_length);
		case RtcpHeaderOnly::PacketType::Bye:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Bye>(os,
																			   packet_header,
																			   packet_data,
																			   packet_data_length);
		case RtcpHeaderOnly::PacketType::App:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::App>(os,
																			   packet_header,
																			   packet_data,
																			   packet_data_length);
		case RtcpHeaderOnly::PacketType::Rtpfb:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Rtpfb>(os,
																				 packet_header,
																				 packet_data,
																				 packet_data_length);
		case RtcpHeaderOnly::PacketType::Psfb:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Psfb>(os,
																				packet_header,
																				packet_data,
																				packet_data_length);
		case RtcpHeaderOnly::PacketType::Xr:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Xr>(os,
																			  packet_header,
																			  packet_data,
																			  packet_data_length);
		case RtcpHeaderOnly::PacketType::Avb:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Avb>(os,
																			   packet_header,
																			   packet_data,
																			   packet_data_length);
		case RtcpHeaderOnly::PacketType::Rsi:
			return rtcp_packet_data_to_stream<RtcpHeaderOnly::PacketType::Rsi>(os,
																			   packet_header,
																			   packet_data,
																			   packet_data_length);
		default:return os;
	}
}
}

std::string packet_to_str(const DataPacket& packet) {
	const char* data_ptr = packet.data;
	int len = packet.length;
	std::ostringstream oss;
	oss << "\n***************\n"
		<< "  LENGHT: " << len << "\n"
		<< "***************\n";

	static constexpr int min_packet_len = 4;

	while (len > min_packet_len) {
		const auto* rtcp = reinterpret_cast<const RtcpHeader*>(data_ptr);
		int read = rtcp->isRtcp() ? parse_as_rtcp(oss, data_ptr, len) :
				   parse_as_rtp(oss, data_ptr, len);
		data_ptr += read;
		len -= read;
	}
	return oss.str();
}

//    std::string rtcp_to_str(const RtcpHeader& rtcp)
//    {
//        std::stringstream str_stream;
//        const auto* rtcp_ptr = reinterpret_cast<const unsigned char*>(&rtcp);
//        int total_length = 0;
//        int current_block = 0;
//        do
//        {
//            if (total_length != 0)
//            {
//                str_stream << '\n';
//            }
//            const auto len = rtcp_block_to_sstream(str_stream, rtcp_ptr, current_block);
//            rtcp_ptr += len;
//            total_length += len;
//            ++current_block;
//        }
//        while (total_length < (rtcp.getLength() + 1) * 4);
//        return str_stream.str();
//    }

//    std::string rtp_to_str(const RtpHeader& rtp)
//    {
//        std::ostringstream str_stream;
//        str_stream << "RTP packet\n";
//        str_stream << "    marker: " << static_cast<int>(rtp.getMarker()) << "\n";
//        str_stream << "    payload type: " << static_cast<int>(rtp.getPayloadType()) << "\n";
//        str_stream << "    sequence number: " << rtp.getSeqNumber() << "\n";
//        str_stream << "    timestamp: " << rtp.getTimestamp() << "\n";
//        str_stream << "    ssrc: " << rtp.getSSRC();
//        return str_stream.str();
//    }

void parse_rtp_single_head_extentions(std::ostream& stream, const void* buffer, int max_size) {
	int len = max_size * 4;
	auto* data = (const uint8_t*) buffer;
	stream << "Extentions:\n";
	while (len > 0) {
		auto byte = *data;
		if (byte != 0) {
			uint8_t id = byte >> 4;
			uint8_t size = byte & (uint8_t) 0x0f;
			stream << "    id: " << (int) id;
			stream << "    data: ";
			for (int i = 0; i < size + 1; ++i) {
				stream << std::bitset<8>(data[1 + i]) << " ";
			}
			stream << "\n";
			data += 2 + size;
			len -= 2 + size;
		} else {
			data++;
			len--;
		}
	}
}

int parse_as_rtp(std::ostream& stream, const void* buffer, int max_size) {
	if (max_size >= (int) sizeof(RtpHeaderOnly)) {
		const char* data = (const char*) buffer;
		const auto* header = reinterpret_cast<const RtpHeaderOnly*>(data);
		stream << "\nRTP PACKET\n";
		stream << *header;
		data += sizeof(RtpHeaderOnly);
		for (uint8_t i = 0; i < header->getCc(); ++i) {
			stream << "    contributing source(" << i + 1 << "): " << ntohl(static_cast<uint32_t>(*data)) << "\n";
			data += 4;
		}
		if (header->HasExtension()) {
			auto* ext = reinterpret_cast<const RtpExtentionHeader*>(data);
			stream << *ext;
			data += sizeof(RtpExtentionHeader);
			if (ext->getExtension_code() == 0xbede) {
				parse_rtp_single_head_extentions(stream, data, ext->getExtension_length());
			}
			data += ext->getExtension_length() * 4;
		}
		uint16_t original_seqnum;
		memcpy(&original_seqnum, data, 2);
		original_seqnum = ntohs(original_seqnum);
		stream << "    original seq number: " << original_seqnum << std::endl;
		if (header->HasPadding()) {
			stream << "Padding:\n"
				   << "    bytes: " << (unsigned) (((const uint8_t*) buffer)[max_size - 1]) << "\n";
		}
	} else {
		// print len error
	}
	return max_size;
}

int parse_as_rtcp(std::ostream& stream, const void* buffer, int max_size) {
	if (max_size >= (int) sizeof(RtcpHeaderOnly)) {
		const auto* header = reinterpret_cast<const RtcpHeaderOnly*>(buffer);
		if (header->IsValid()) {
			if (header->getPacketLengthInBytes() <= max_size) {
				stream << "\nRTCP PACKET\n";
				stream << *header;
				if (header->getLength() > 0) {
					const char* dat_buf = (const char*) buffer;
					dat_buf += sizeof(RtcpHeaderOnly);
					rtcp_packet_data_to_stream(header->getPacketType(),
											   stream,
											   header,
											   dat_buf,
											   header->getLength() * 4);
					return header->getPacketLengthInBytes();
				}
			} else {
				stream << "RTCP PACKET PARSING ERROR: not enough len: { actual: " << max_size
					   << ", required: " << header->getPacketLengthInBytes() << " }\n";
			}
		} else {
			stream << "RTCP PACKET PARSING ERROR: header format error: { type: " << header->getPacketType()
				   << ", version " << header->getVersion() << " }\n";
		}
	}
	return max_size;
}

namespace {
//        packet_length rtcp_block_to_sstream(std::stringstream& str_stream,
//                const unsigned char* rtcp_block,
//                int block_index)
//        {
//            const auto* rtcp = reinterpret_cast<const RtcpHeader*>(rtcp_block);
//            std::string padding = "    ";
//            if (block_index == 0)
//            {
//                str_stream << "RTCP packet\n";
//            }
//            else
//            {
//                padding += padding;
//                str_stream << padding << "RTCP packet";
//                str_stream << " [compound " << block_index << "]\n";
//            }
//            str_stream << padding << "    packet type: "
//					   << RtcpPayloadNameMap::getNameByCode(rtcp->getPacketType())
//					   << "[" << static_cast<int>(rtcp->getPacketType()) << "]\n";
//            str_stream << padding << "    length: " << rtcp->getLength() << "\n";
//            str_stream << padding << "    ssrc: " << rtcp->getSSRC();
//            switch (rtcp->getPacketType())
//            {
//                case RTCP_Sender_PT:
//                    str_stream << '\n';
//					sr_packet_to_sstream(str_stream, padding + padding, *rtcp, rtcp->GetSrBlockCount());
//                    break;
//                case RTCP_Receiver_PT:
//                    str_stream << '\n';
//                    rr_packet_to_sstream(str_stream, padding + padding, *rtcp);
//                    break;
//                case RTCP_PS_Feedback_PT:
//                    str_stream << '\n';
//                    ps_feedback_to_sstream(str_stream, padding + padding, *rtcp);
//                    break;
//                case RTCP_RTP_Feedback_PT:
//                    str_stream << '\n';
//                    tl_feedback_to_sstream(str_stream, padding + padding, *rtcp);
//                    break;
//                case RTCP_SDES_PT:
//                    str_stream << '\n';
//                    sdes_packet_to_sstream(str_stream, padding + padding, *rtcp);
//                    break;
//                default:
//                    break;
//                }
//            return (rtcp->getLength() + 1) * 4;
//        }
//
//        void sr_packet_to_sstream(std::ostream& str_stream,
//								  const std::string& padding,
//								  const RtcpHeader& data,
//								  size_t report_blocks)
//        {
//            str_stream << padding << "SR\n";
//		  	str_stream << padding << "[sender info]\n";
//            str_stream << padding << "    NTP timestamp: " << data.getNtpTimestamp() << "\n";
//            str_stream << padding << "    RTP timestamp: " << data.getRtpTimestamp() << "\n";
//		  	str_stream << padding << "    sender's packet count: " << data.report.senderReport.getPacketsent() << "\n";
//		  	str_stream << padding << "    sender's octet count: " << data.report.senderReport.getOctetssent() << "\n";
//            for(size_t i = 0; i < report_blocks; ++i) {
//			  str_stream << padding << "[report block " << i + 1 << "/" << report_blocks << "]\n";
//			  data.report.senderReport.rrlist[i].print_report(str_stream, padding + padding);
//			}
//        }
//
//        void rr_packet_to_sstream(std::ostream& stream, const std::string& padding, const RtcpHeader& data)
//        {
//            stream << padding << "RR\n";
//            data.report.receiverReport.print_report(stream, padding + padding);
//        }
//
//        void sdes_packet_to_sstream(std::stringstream& str_stream, const std::string& padding, const RtcpHeader& data)
//        {
//            str_stream << padding << "SDES";
//        }
//
//        void ps_feedback_to_sstream(std::stringstream& str_stream, const std::string& padding, const RtcpHeader& data)
//        {
//            if (data.getBlockCount() == RTCP_AFB)
//            {
//                const char *uniqueId = reinterpret_cast<const char*>(&data.report.rembPacket.uniqueid);
//                if (!strncmp(uniqueId, "REMB", 4))
//                {
//                    const uint64_t bitrate = data.getBrMantis() << data.getBrExp();
//                    str_stream << padding << "REMB\n";
//                    str_stream << padding << "    bitrate: " << bitrate;
//                }
//                else
//                {
//                    str_stream << padding << "unsupported AFB packet";
//                }
//            }
//            else if (data.getBlockCount() == RTCP_PLI_FMT)
//            {
//                str_stream << padding << "PLI";
//            }
//        }
//
//        void tl_feedback_to_sstream(std::stringstream& str_stream, const std::string& padding, const RtcpHeader& data)
//        {
//            str_stream << padding << "NACK\n";
//            str_stream << padding << "    pid: " << data.getNackPid() << "\n";
//            str_stream << padding << "    blp: " << std::hex << data.getNackBlp() << std::dec;
//        }
}

std::string RtcpPayloadNameMap::getNameByCode(uint8_t code) {
	switch (code) {
		case 200:return "RTCP Sender Report";
		case 201:return "RTCP Receiver Report";
		case 202:return "RTCP SDES PT";
		case 203:return "RTCP BYE";
		case 204:return "RTCP APP";
		case 205:return "RTCP Transport Layer Feedback Packet";
		case 206:return "RTCP Payload Specific Feedback Packet";
		default:return "unknown";
	}
}

}