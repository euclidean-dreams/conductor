#include "AudioProcessorSink.h"

namespace conductor {

AudioProcessorSink::AudioProcessorSink(std::unique_ptr<PacketSpout> input,
                                       std::unique_ptr<impresarioUtils::NetworkSocket> output)
        : input{move(input)},
          output{move(output)} {

}

void AudioProcessorSink::setup() {

}

void AudioProcessorSink::process() {
    auto packets = input->getAllAvailablePackets();
    if (packets != nullptr) {
        for (auto &packet: *packets) {
            auto message = packet->serialize();
            output->send(*message);
        }
        input->concludePacketUse(move(packets));
    }
    std::this_thread::sleep_for(std::chrono::microseconds(AUDIO_SINK_WAKE_INTERVAL));
}

bool AudioProcessorSink::shouldContinue() {
    return true;
}

}
