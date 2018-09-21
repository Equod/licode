#ifndef ERIZO_SRC_ERIZO_RTP_PACKETPRINTER_H_
#define ERIZO_SRC_ERIZO_RTP_PACKETPRINTER_H_

#include "pipeline/Handler.h"
#include "./logger.h"

namespace erizo {
class WebRtcConnection;

class PacketPrinter : public Handler {
  DECLARE_LOGGER();
 public:
  void enable() override {}
  void disable() override {}

  std::string getName() override {
      return "packet_printer";
  }

  void read(Context *ctx, std::shared_ptr<DataPacket> packet) override;
  void write(Context *ctx, std::shared_ptr<DataPacket> packet) override;
  void notifyUpdate() override {}
};

}

#endif
