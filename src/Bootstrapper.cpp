#include "Bootstrapper.h"

namespace conductor {

void Bootstrapper::bootstrap() {
    Config::initialize();
    throwOnPortaudioError(Pa_Initialize());
    spdlog::stdout_color_mt(Config::getInstance().getLoggerName());
    zmq::context_t context(1);

    std::unique_ptr<AudioProcessorSuite> audioProcessorSuite;
    if (Config::getInstance().getRealTimeInput()) {
        auto audioInput = std::make_unique<AudioInput>(
                Config::getInstance().getSampleRate(), Config::getInstance().getAudioPacketSize()
        );
        audioProcessorSuite = std::make_unique<AudioProcessorSuite>(context, audioInput->getAudioStream());
        audioProcessorSuite->activate();
    } else {
        auto audioInput = std::make_unique<AudioFileReader>(Config::getInstance().getAudioPacketSize());
        audioProcessorSuite = std::make_unique<AudioProcessorSuite>(context, audioInput->getAudioStream());
        audioProcessorSuite->activate();
    }

    context.close();
    throwOnPortaudioError(Pa_Terminate());
}

}
