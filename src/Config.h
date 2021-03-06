#ifndef CONDUCTOR_CONFIG_H
#define CONDUCTOR_CONFIG_H

#include <string_view>
#include <aubio/aubio.h>
#include "OnsetMethod_generated.h"

using namespace std;
using namespace ImpresarioSerialization;

// logger
constexpr string_view LOGGER_NAME = "conductor";

// audio
constexpr int PROCESSOR_HOP_SIZE = 256;
constexpr int PROCESSOR_BUFFER_SIZE = 1024;
constexpr float SAMPLE_RATE = 44100.00;
constexpr int PACKET_SIZE = PROCESSOR_HOP_SIZE;
constexpr int RING_BUFFER_SIZE_MULTIPLIER = 16;

// audio source
constexpr int AUDIO_STREAM_QUERY_INTERVAL = 500;

// onset
constexpr smpl_t DEFAULT_ONSET_THRESHOLD = 0.6;
constexpr smpl_t DEFAULT_ONSET_SILENCE = -50.0;
constexpr smpl_t DEFAULT_ONSET_MINIOI_MS = 1;
constexpr bool DEFAULT_ONSET_ADAPTIVE_WHITENING = true;
constexpr array<OnsetMethod, 9> ONSET_PROCESSORS = {
        OnsetMethod::energy,
        OnsetMethod::hfc,
        OnsetMethod::complex,
        OnsetMethod::phase,
        OnsetMethod::wphase,
        OnsetMethod::specdiff,
        OnsetMethod::kl,
        OnsetMethod::mkl,
        OnsetMethod::specflux
};

// processors
constexpr string_view PARAMETER_ENDPOINT = "tcp://10.0.0.132:44440";
constexpr string_view ONSET_AGGREGATOR_OUTPUT_ENDPOINT = "tcp://*:44441";

// flatBuffers
constexpr int ONSET_SIZE = 24;
constexpr int AUDIO_ATTRIBUTES_SIZE = 88;
constexpr int AUDIO_PACKET_SIZE = 1064;

#endif //PERFORMER_CONFIG_H
