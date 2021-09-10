#ifndef CONDUCTOR_AUDIOPROCESSORSINK_H
#define CONDUCTOR_AUDIOPROCESSORSINK_H

#include <thread>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <ImpresarioUtils.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketReceiver.h"
#include "packet/Serializable.h"

namespace conductor {

class AudioProcessorSink : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<Serializable>> input;
    std::unique_ptr<impresarioUtils::NetworkSocket> output;
    std::shared_ptr<spdlog::logger> logger;

public:
    explicit AudioProcessorSink(std::unique_ptr<PacketReceiver<Serializable>> input,
                                std::unique_ptr<impresarioUtils::NetworkSocket> output);

    void process() override;
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSINK_H
