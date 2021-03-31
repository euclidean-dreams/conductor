#ifndef CONDUCTOR_CONFIG_H
#define CONDUCTOR_CONFIG_H

#include <string_view>

namespace conductor {

constexpr std::string_view LOGGER_NAME = "conductor";

constexpr float SAMPLE_RATE = 44100.00;
constexpr int AUDIO_PACKET_SIZE = 128;

}

#endif //PERFORMER_CONFIG_H
