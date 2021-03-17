#ifndef CONDUCTOR_CONFIG_H
#define CONDUCTOR_CONFIG_H

#include <string_view>
#include <aubio/aubio.h>
#include "OnsetMethod_generated.h"
#include "PitchMethod_generated.h"

namespace conductor {

constexpr std::string_view LOGGER_NAME = "conductor";

constexpr int PROCESSOR_HOP_SIZE = 128;
constexpr int PROCESSOR_BUFFER_SIZE = 512;
constexpr float SAMPLE_RATE = 44100.00;
constexpr int AUDIO_PACKET_SIZE = PROCESSOR_HOP_SIZE;

}

#endif //PERFORMER_CONFIG_H
