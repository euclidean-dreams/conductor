#include "Bootstrapper.h"

namespace conductor {

void Bootstrapper::bootstrap() {
    spdlog::stdout_color_mt(static_cast<std::string>(LOGGER_NAME));
    zmq::context_t context(1);
    auto audioStreamManager = std::make_unique<AudioStreamManager>(SAMPLE_RATE, PACKET_SIZE,
                                                                   RING_BUFFER_SIZE_MULTIPLIER);
    auto audioProcessorSuite = std::make_unique<AudioProcessorSuite>(context, audioStreamManager->getAudioStream());
    audioProcessorSuite->activate();
    context.close();
}

}
