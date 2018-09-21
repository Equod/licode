#include "rtp/RtpUtils.h"
#include "rtp/PacketPrintUtils.h"

#include <cmath>
#include <memory>

namespace erizo {


constexpr int kMaxPacketSize = 1500;

bool RtpUtils::sequenceNumberLessThan(uint16_t first, uint16_t last) {
  return RtpUtils::numberLessThan(first, last, 16);
}

bool RtpUtils::numberLessThan(uint16_t first, uint16_t last, int bits) {
  uint16_t result = first - last;
  uint16_t mark = std::pow(2, bits) - 1;
  result = result & mark;
  uint16_t threshold = (bits > 4) ? std::pow(2, bits - 4) - 1 : std::pow(2, bits) - 1;
  return result > threshold;
}

void RtpUtils::updateREMB(RtcpHeader *chead, uint bitrate) {
  if (chead->packettype == RTCP_PS_Feedback_PT && chead->getBlockCount() == RTCP_AFB) {
    char *uniqueId = reinterpret_cast<char*>(&chead->report.rembPacket.uniqueid);
    if (!strncmp(uniqueId, "REMB", 4)) {
      chead->setREMBBitRate(bitrate);
    }
  }
}

void RtpUtils::forEachNack(RtcpHeader *chead, std::function<void(uint16_t, uint16_t, RtcpHeader*)> f) {
  if (chead->packettype == RTCP_RTP_Feedback_PT) {
    int length = (chead->getLength() + 1)*4;
    int current_position = kNackCommonHeaderLengthBytes;
    uint8_t* aux_pointer = reinterpret_cast<uint8_t*>(chead);
    RtcpHeader* aux_chead;
    while (current_position < length) {
      aux_chead = reinterpret_cast<RtcpHeader*>(aux_pointer);
      uint16_t initial_seq_num = aux_chead->getNackPid();
      uint16_t plb = aux_chead->getNackBlp();
      f(initial_seq_num, plb, aux_chead);
      current_position += 4;
      aux_pointer += 4;
    }
  }
}

int RtpUtils::forEachRtcpPacketInBuffer(const void* buffer, int len, std::function<void(const RtcpHeader*)> f) {
  auto data = reinterpret_cast<const char*>(buffer);
  int remain_len = static_cast<int>(len);
  int count = 0;
  do {
    if(remain_len >= (int)sizeof(RtcpHeaderOnly)) {
      auto header = reinterpret_cast<const RtcpHeaderOnly*>(data);
      if(header->IsValid()) {
        f(reinterpret_cast<const RtcpHeader*>(header));
        ++count;
        auto packet_len = (header->getLength() + 1) * 4;
        data += packet_len;
        remain_len -= packet_len;
      } else {
        remain_len = 0;
      }
    } else {
      return count;
    }
  } while (remain_len > 0);
  return count;
}

int RtpUtils::forEachRtcpPacketInBuffer(void* buffer, int len, std::function<void(RtcpHeader*)> f) {
  auto data = reinterpret_cast<char*>(buffer);
  int remain_len = static_cast<int>(len);
  int count = 0;
  do {
    if(remain_len >= (int)sizeof(RtcpHeaderOnly)) {
      auto header = reinterpret_cast<RtcpHeaderOnly*>(data);
      if(header->IsValid()) {
        f(reinterpret_cast<RtcpHeader*>(header));
        ++count;
        auto packet_len = (header->getLength() + 1) * 4;
        data += packet_len;
        remain_len -= packet_len;
      } else {
        remain_len = 0;
      }
    } else {
      remain_len = 0;
    }
  } while (remain_len > 0);
  return count;
}

bool RtpUtils::HasPacketRtcpInBuffer(const void* buffer, size_t len, std::function<bool(const RtcpHeader*)> matcher) {
  bool result = false;
  forEachRtcpPacketInBuffer(buffer, static_cast<int>(len), [&result, matcher](const RtcpHeader* header){
    if(!result) {
      result = matcher(header);
    }
  });
  return result;
}

bool RtpUtils::isPLI(std::shared_ptr<DataPacket> packet) {
  bool is_pli = false;
  forEachRtcpBlock(packet, [&is_pli] (RtcpHeader *header) {
    if (header->getPacketType() == RTCP_PS_Feedback_PT &&
        header->getBlockCount() == RTCP_PLI_FMT) {
          is_pli = true;
        }
  });
  return is_pli;
}

bool RtpUtils::isFIR(std::shared_ptr<DataPacket> packet) {
  bool is_fir = false;
  forEachRtcpBlock(packet, [&is_fir] (RtcpHeader *header) {
    if (header->getPacketType() == RTCP_PS_Feedback_PT &&
        header->getBlockCount() == RTCP_FIR_FMT) {
          is_fir = true;
        }
  });
  return is_fir;
}

std::shared_ptr<DataPacket> RtpUtils::createPLI(uint32_t source_ssrc, uint32_t sink_ssrc) {
  RtcpHeader pli;
  pli.setPacketType(RTCP_PS_Feedback_PT);
  pli.setBlockCount(RTCP_PLI_FMT);
  pli.setSSRC(sink_ssrc);
  pli.setSourceSSRC(source_ssrc);
  pli.setLength(2);
  char *buf = reinterpret_cast<char*>(&pli);
  int len = (pli.getLength() + 1) * 4;
  return std::make_shared<DataPacket>(0, buf, len, VIDEO_PACKET);
}

std::shared_ptr<DataPacket> RtpUtils::createFIR(uint32_t source_ssrc, uint32_t sink_ssrc, uint8_t seq_number) {
  RtcpHeader fir;
  fir.setPacketType(RTCP_PS_Feedback_PT);
  fir.setBlockCount(RTCP_FIR_FMT);
  fir.setSSRC(sink_ssrc);
  fir.setSourceSSRC(source_ssrc);
  fir.setLength(4);
  fir.setFIRSourceSSRC(source_ssrc);
  fir.setFIRSequenceNumber(seq_number);
  char *buf = reinterpret_cast<char*>(&fir);
  int len = (fir.getLength() + 1) * 4;
  return std::make_shared<DataPacket>(0, buf, len, VIDEO_PACKET);
}

std::shared_ptr<DataPacket> RtpUtils::createREMB(uint32_t ssrc, std::vector<uint32_t> ssrc_list, uint32_t bitrate) {
  erizo::RtcpHeader remb;
  remb.setPacketType(RTCP_PS_Feedback_PT);
  remb.setBlockCount(RTCP_AFB);
  memcpy(&remb.report.rembPacket.uniqueid, "REMB", 4);

  remb.setSSRC(ssrc);
  remb.setSourceSSRC(0);
  remb.setLength(4 + ssrc_list.size());
  remb.setREMBBitRate(bitrate);
  remb.setREMBNumSSRC(ssrc_list.size());
  uint8_t index = 0;
  for (uint32_t feed_ssrc : ssrc_list) {
    remb.setREMBFeedSSRC(index++, feed_ssrc);
  }
  int len = (remb.getLength() + 1) * 4;
  char *buf = reinterpret_cast<char*>(&remb);
  return std::make_shared<erizo::DataPacket>(0, buf, len, erizo::OTHER_PACKET);
}


int RtpUtils::getPaddingLength(std::shared_ptr<DataPacket> packet) {
  RtpHeader *rtp_header = reinterpret_cast<RtpHeader*>(packet->data);
  if (rtp_header->hasPadding()) {
    return packet->data[packet->length - 1] & 0xFF;
  }
  return 0;
}

void RtpUtils::forEachRtcpBlock(std::shared_ptr<DataPacket> packet, std::function<void(RtcpHeader*)> f) {
  RtcpHeader *chead = reinterpret_cast<RtcpHeader*>(packet->data);
  int len = packet->length;
  if (chead->isRtcp()) {
    char* moving_buffer = packet->data;
    int rtcp_length = 0;
    int total_length = 0;
    int currentBlock = 0;

    do {
      moving_buffer += rtcp_length;
      chead = reinterpret_cast<RtcpHeader*>(moving_buffer);
      rtcp_length = (ntohs(chead->length) + 1) * 4;
      total_length += rtcp_length;
      f(chead);
      currentBlock++;
    } while (total_length < len);
  }
}

std::shared_ptr<DataPacket> RtpUtils::makePaddingPacket(std::shared_ptr<DataPacket> packet, uint8_t padding_size) {
  erizo::RtpHeader *header = reinterpret_cast<RtpHeader*>(packet->data);

  uint16_t packet_length = header->getHeaderLength() + padding_size;

  char packet_buffer[kMaxPacketSize];
  erizo::RtpHeader *new_header = reinterpret_cast<RtpHeader*>(packet_buffer);
  memset(packet_buffer, 0, packet_length);
  memcpy(packet_buffer, reinterpret_cast<char*>(header), header->getHeaderLength());

  new_header->setPadding(true);
  new_header->setMarker(false);
  packet_buffer[packet_length - 1] = padding_size;

  return std::make_shared<DataPacket>(packet->comp, packet_buffer, packet_length, packet->type);
}

bool RtpUtils::isHeaHeaderPLI(const RtcpHeader* header) {
  return header->getPacketType() == RTCP_PS_Feedback_PT &&
      header->getBlockCount() == RTCP_PLI_FMT;
}

bool RtpUtils::isHeaHeaderREMB(const RtcpHeader* header) {
  return header->getPacketType() == RTCP_PS_Feedback_PT &&
      header->getBlockCount() == RTCP_AFB;
}

bool RtpUtils::isHeaHeaderFIR(const RtcpHeader* header) {
  return header->getPacketType() == RTCP_PS_Feedback_PT &&
      header->getBlockCount() == RTCP_FIR_FMT;
}

bool RtpUtils::isHeaHeaderNACK(const RtcpHeader* header) {
  return header->getPacketType() == RTCP_RTP_Feedback_PT &&
      header->getBlockCount() == 1;
}

bool RtpUtils::HasPLI(const void* buffer, size_t len) {
  return HasPacketRtcpInBuffer(buffer, len, isHeaHeaderPLI);
}

bool RtpUtils::HasREMB(const void* buffer, size_t len) {
  return HasPacketRtcpInBuffer(buffer, len, isHeaHeaderREMB);
}

bool RtpUtils::HasFIR(const void* buffer, size_t len) {
  return HasPacketRtcpInBuffer(buffer, len, isHeaHeaderFIR);
}

bool RtpUtils::HasNACK(const void* buffer, size_t len) {
  return HasPacketRtcpInBuffer(buffer, len, isHeaHeaderNACK);
}



}  // namespace erizo
