#include "AudioProcessorSink.h"

namespace conductor {

AudioProcessorSink::AudioProcessorSink(std::unique_ptr<PacketReceiver<Serializable>> input,
                                       std::unique_ptr<impresarioUtils::NetworkSocket> output)
        : input{move(input)},
          output{move(output)},
          logger{spdlog::get(Config::getInstance().getLoggerName())} {

}

void AudioProcessorSink::process() {
    auto packet = input->getPacket();
    output->sendSerializedData(packet->getIdentifier(), *packet->serialize());
}

}
