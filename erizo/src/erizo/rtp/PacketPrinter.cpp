#include "rtp/PacketPrinter.h"
#include <string>
#include "rtp/PacketPrintUtils.h"
#include "rtp/RtpHeaders.h"
#include "rtp/RtpUtils.h"

namespace erizo {
DEFINE_LOGGER(PacketPrinter, "rtp.PacketPrinter");

void PacketPrinter::read(Context *ctx, std::shared_ptr<DataPacket> packet) {
    // if (packet->type == VIDEO_PACKET) {
    //     auto *rtcp = reinterpret_cast<RtcpHeader *>(packet->data);
    //     if (!rtcp->isRtcp()) {
    //         auto *rtp = reinterpret_cast<RtpHeader *>(packet->data);
    //         if (rtp->hasPadding()) {
    //             // ELOG_DEBUG("INPUT <-------- %s", packet_to_str(*packet));
    //         }
    //      }
    // }
    auto *rtcp = reinterpret_cast<RtcpHeader *>(packet->data);
    if (rtcp->isRtcp()) {
        packet->parse_rtcp_packets();
        for (const auto &rtcp_header : packet->getRtcp_packets()) {
            if (RtpUtils::isHeaHeaderPLI(rtcp_header)) {
                ELOG_DEBUG("PLI --------> %s", packet_to_str(*packet), packet->length);
            }
            if (RtpUtils::isHeaHeaderFIR(rtcp_header)) {
                ELOG_DEBUG("FIR --------> %s", packet_to_str(*packet), packet->length);
            }
            if (RtpUtils::isHeaHeaderNACK(rtcp_header)) {
                ELOG_DEBUG("NACK <------------ IN %s", packet_to_str(*packet), packet->length);
            }
        }
    } else {        
        if (packet->type == VIDEO_PACKET) {
            auto *rtp = reinterpret_cast<RtpHeader *>(packet->data);
            // if (!rtp->hasPadding()) {
                ELOG_DEBUG("SEQNUM: %u %d", rtp->getSeqNumber(), rtp->hasPadding());
            // }
        }
    }
    ctx->fireRead(std::move(packet));
}

void PacketPrinter::write(Context *ctx, std::shared_ptr<DataPacket> packet) {
    // if (packet->type == VIDEO_PACKET) {
    auto *rtcp = reinterpret_cast<RtcpHeader *>(packet->data);
    // if (rtcp->isRtcp()) {
    //     ELOG_DEBUG("PASSA REMB");
    // }
    if (rtcp->isRtcp()) {
        packet->parse_rtcp_packets();
        for (const auto &rtcp_header : packet->getRtcp_packets()) {
            if (RtpUtils::isHeaHeaderPLI(rtcp_header)) {
                ELOG_DEBUG("PLI --------> %s", packet_to_str(*packet), packet->length);
            }
            if (RtpUtils::isHeaHeaderFIR(rtcp_header)) {
                ELOG_DEBUG("FIR --------> %s", packet_to_str(*packet), packet->length);
            }
            if (RtpUtils::isHeaHeaderNACK(rtcp_header)) {
                ELOG_DEBUG("ESCE NACK: %u %x", rtcp_header->getNackPid(), rtcp_header->getNackBlp());
                ELOG_DEBUG("NACK --------> %s", packet_to_str(*packet), packet->length);
            }
        }
    }
    // }
    ctx->fireWrite(std::move(packet));
}
}  // namespace erizo
