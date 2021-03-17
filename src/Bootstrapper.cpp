#include "Bootstrapper.h"

namespace conductor {

void Bootstrapper::bootstrap() {
    spdlog::stdout_color_mt(static_cast<std::string>(LOGGER_NAME));
    zmq::context_t context(1);
    auto audioStreamManager = std::make_unique<AudioStreamManager>(SAMPLE_RATE, AUDIO_PACKET_SIZE);
    auto audioProcessorSuite = std::make_unique<AudioProcessorSuite>(context, audioStreamManager->getAudioStream());
    audioProcessorSuite->activate();
    context.close();
}

}
