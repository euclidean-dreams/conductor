#include <memory>
#include <zmq.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "Config.h"
#include "audioStream/AudioStreamManager.h"
#include "audioProcessor/AudioProcessorSuite.h"

using namespace zmq;

int main() {
    spdlog::stdout_color_mt("conductor");
    context_t context(1);
    auto audioStreamManager = make_unique<AudioStreamManager>(SAMPLE_RATE, PACKET_SIZE, RING_BUFFER_SIZE_MULTIPLIER);
    auto audioProcessorSuite = make_unique<AudioProcessorSuite>(context, audioStreamManager->getAudioStream());
    audioProcessorSuite->activate();
    context.close();
    return 0;
}
