#include "Bootstrapper.h"

namespace conductor {

void Bootstrapper::bootstrap() {
    Config::initialize();
    throwOnPortaudioError(Pa_Initialize());
    spdlog::stdout_color_mt(Config::getInstance().getLoggerName());
    zmq::context_t context(1);
    auto audioInput = std::make_unique<AudioInput>(Config::getInstance().getSampleRate(),
                                                   Config::getInstance().getAudioPacketSize());
//    auto audioInput = std::make_unique<AudioFileReader>(Config::getInstance().getAudioPacketSize());
    auto audioProcessorSuite = std::make_unique<AudioProcessorSuite>(context, audioInput->getAudioStream());
    audioProcessorSuite->activate();
    context.close();
    throwOnPortaudioError(Pa_Terminate());
}

}
