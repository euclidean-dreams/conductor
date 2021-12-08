

#ifndef CONDUCTOR_BOOTSTRAPPER_H
#define CONDUCTOR_BOOTSTRAPPER_H

#include <memory>
#include <zmq.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "Config.h"
#include "audioStream/AudioInput.h"
#include "audioStream/AudioFileReader.h"
#include "audioProcessor/AudioProcessorSuite.h"
#include "audioStream/AudioUtils.h"

namespace conductor {

class Bootstrapper {
public:
    static void bootstrap();
};

}

#endif //CONDUCTOR_BOOTSTRAPPER_H
