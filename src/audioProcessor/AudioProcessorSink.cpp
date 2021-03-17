#include "AudioProcessorSink.h"

namespace conductor {

AudioProcessorSink::AudioProcessorSink(std::unique_ptr<PacketSpout> input,
                                       std::unique_ptr<impresarioUtils::NetworkSocket> output)
        : input{move(input)},
          output{move(output)},
          logger{spdlog::get(static_cast<std::string>(LOGGER_NAME))} {

}

void AudioProcessorSink::setup() {

}

void AudioProcessorSink::process() {
    auto packets = input->getPackets(1);
    auto &packet = packets->getPacket(0);
    auto message = packets->getPacket(0).serialize();
    output->sendSerializedData(packet.getIdentifier(), *packet.serialize());
    packets->concludeUse();
}

bool AudioProcessorSink::shouldContinue() {
    return true;
}

}
