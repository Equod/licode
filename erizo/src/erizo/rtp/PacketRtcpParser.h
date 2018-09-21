#ifndef ERIZO_ALL_PACKETRTCPPARSER_H
#define ERIZO_ALL_PACKETRTCPPARSER_H

#include <pipeline/Handler.h>

namespace erizo {

class PacketRtcpParser : public InboundHandler {
 public:
  void read(Context* ctx, std::shared_ptr<DataPacket> packet) override;
  void enable() override;
  void disable() override;
  std::string getName() override;
  void notifyUpdate() override;
};

}  // namespace erizo

#endif //ERIZO_ALL_PACKETRTCPPARSER_H
