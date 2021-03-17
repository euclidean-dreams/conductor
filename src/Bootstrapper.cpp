#include "Bootstrapper.h"

namespace conductor {

void Bootstrapper::bootstrap() {
    throwOnPortaudioError(Pa_Initialize());
    spdlog::stdout_color_mt(static_cast<std::string>(LOGGER_NAME));
    zmq::context_t context(1);
    auto audioInput = std::make_unique<AudioInput>(SAMPLE_RATE, AUDIO_PACKET_SIZE);
    auto audioProcessorSuite = std::make_unique<AudioProcessorSuite>(context, audioInput->getAudioStream());
    audioProcessorSuite->activate();
    context.close();
    throwOnPortaudioError(Pa_Terminate());
}

}
