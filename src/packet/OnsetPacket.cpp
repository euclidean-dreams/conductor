#include "OnsetPacket.h"

namespace conductor {

const OnsetPacket &OnsetPacket::from(const Packet &packet) {
    return dynamic_cast<const OnsetPacket &>(packet);
}

OnsetPacket::OnsetPacket(uint64_t timestamp, ImpresarioSerialization::OnsetMethod method, uint64_t sampleTimestamp)
        : timestamp{timestamp},
          method{method},
          sampleTimestamp{sampleTimestamp} {

}

std::unique_ptr<zmq::multipart_t> OnsetPacket::serialize() const {
    flatbuffers::FlatBufferBuilder builder{};
    auto onset = CreateOnset(builder, timestamp, method, sampleTimestamp);
    builder.Finish(onset);
    return std::make_unique<zmq::multipart_t>(builder.GetBufferPointer(), builder.GetSize());
}

}
