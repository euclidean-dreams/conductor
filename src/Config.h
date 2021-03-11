#ifndef CONDUCTOR_CONFIG_H
#define CONDUCTOR_CONFIG_H

#include <string_view>
#include <aubio/aubio.h>
#include "OnsetMethod_generated.h"

namespace conductor {

// logger
constexpr std::string_view LOGGER_NAME = "conductor";

// audio
constexpr int PROCESSOR_HOP_SIZE = 128;
constexpr int PROCESSOR_BUFFER_SIZE = 512;
constexpr float SAMPLE_RATE = 44100.00;
constexpr int AUDIO_PACKET_SIZE = PROCESSOR_HOP_SIZE;
constexpr int RING_BUFFER_SIZE_MULTIPLIER = 16;

// audio source
constexpr int AUDIO_STREAM_QUERY_INTERVAL = 250;

// onset
constexpr smpl_t DEFAULT_ONSET_THRESHOLD = 0.3;
constexpr smpl_t DEFAULT_ONSET_SILENCE = -60.0;
constexpr smpl_t DEFAULT_ONSET_MINIOI_MS = 1;
constexpr bool DEFAULT_ONSET_ADAPTIVE_WHITENING = true;
constexpr std::array<ImpresarioSerialization::OnsetMethod, 9> ONSET_PROCESSORS = {
        ImpresarioSerialization::OnsetMethod::energy,
        ImpresarioSerialization::OnsetMethod::hfc,
        ImpresarioSerialization::OnsetMethod::complex,
        ImpresarioSerialization::OnsetMethod::phase,
        ImpresarioSerialization::OnsetMethod::wphase,
        ImpresarioSerialization::OnsetMethod::specdiff,
        ImpresarioSerialization::OnsetMethod::kl,
        ImpresarioSerialization::OnsetMethod::mkl,
        ImpresarioSerialization::OnsetMethod::specflux
};

// processors
constexpr std::string_view PARAMETER_ENDPOINT = "tcp://10.0.0.132:44440";
constexpr std::string_view CONDUCTOR_OUTPUT_ENDPOINT = "tcp://*:44441";

}

#endif //PERFORMER_CONFIG_H
