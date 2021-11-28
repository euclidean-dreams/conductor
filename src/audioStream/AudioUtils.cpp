#include "audioStream/AudioUtils.h"

namespace conductor {

void throwOnPortaudioError(PaError portaudioReturnCode) {
    if (portaudioReturnCode != paNoError) {
        spdlog::get(Config::getInstance().getLoggerName())->error(Pa_GetErrorText(portaudioReturnCode));
        throw std::runtime_error(Pa_GetErrorText(portaudioReturnCode));
    }
}

}
