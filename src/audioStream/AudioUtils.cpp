#include "audioStream/AudioUtils.h"

namespace conductor {

void throwOnPortaudioError(PaError portaudioReturnCode) {
    if (portaudioReturnCode != paNoError) {
        throw std::runtime_error(Pa_GetErrorText(portaudioReturnCode));
    }
}

}
