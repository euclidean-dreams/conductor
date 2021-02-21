#ifndef CONDUCTOR_CONFIG_H
#define CONDUCTOR_CONFIG_H

#include <string>
#include <aubio/aubio.h>
#include "OnsetMethod_generated.h"

using namespace std;
using namespace ImpresarioSerialization;

// logger
const static string LOGGER_NAME = "conductor";

// audio
const static int PROCESSOR_HOP_SIZE = 128;
const static int PROCESSOR_BUFFER_SIZE = PROCESSOR_HOP_SIZE * 8;
const static float SAMPLE_RATE = 44100.00;
const static int PACKET_SIZE = PROCESSOR_HOP_SIZE;
const static int RING_BUFFER_SIZE_MULTIPLIER = 16;

// audio source
const static int AUDIO_STREAM_QUERY_INTERVAL = 500;

// onset
const static smpl_t DEFAULT_ONSET_THRESHOLD = 0.5;
const static smpl_t DEFAULT_ONSET_SILENCE = -50.0;
const static smpl_t DEFAULT_ONSET_MINIOI_MS = 1;
const static bool DEFAULT_ONSET_ADAPTIVE_WHITENING = true;
const static vector<OnsetMethod> ONSET_PROCESSORS = {
        OnsetMethod::specflux,
        OnsetMethod::phase,
        OnsetMethod::complex,
        OnsetMethod::energy
};

// processors
const static string PARAMETER_RECEIVER_ENDPOINT = "tcp://10.0.0.132:44440";
const static string ONSET_AGGREGATOR_OUTPUT_ENDPOINT = "tcp://*:44441";

// flatBuffers
const static int ONSET_SIZE = 24;
const static int ONSET_AGGREGATE_SIZE = 72;
const static int AUDIO_PACKET_SIZE = 1064;

#endif //PERFORMER_CONFIG_H
