#ifndef CONDUCTOR_AUDIOPROCESSORSUITE_H
#define CONDUCTOR_AUDIOPROCESSORSUITE_H

#include <thread>
#include <memory>
#include <vector>
#include <zmq.hpp>
#include <NonCopyable.h>
#include "audioProcessor/AudioStreamSource.h"
#include "audioProcessor/AudioProcessorSink.h"
#include "audioProcessor/OnsetProcessor.h"
#include "audioProcessor/PitchProcessor.h"
#include "audioStream/AudioStream.h"
#include "packet/PacketConduit.h"

namespace conductor {

class AudioProcessorSuite : impresarioUtils::NonCopyable {
private:
    inline static const std::string_view PARAMETER_ENDPOINT = "tcp://10.0.0.132:44440";
    inline static const std::string_view CONDUCTOR_OUTPUT_ENDPOINT = "tcp://*:44441";
    inline static const std::array<ImpresarioSerialization::OnsetMethod, 1> ONSET_PROCESSORS = {
//        ImpresarioSerialization::OnsetMethod::energy,
//        ImpresarioSerialization::OnsetMethod::hfc,
//        ImpresarioSerialization::OnsetMethod::complex,
//        ImpresarioSerialization::OnsetMethod::phase,
//        ImpresarioSerialization::OnsetMethod::wphase,
//        ImpresarioSerialization::OnsetMethod::specdiff,
//        ImpresarioSerialization::OnsetMethod::kl,
//        ImpresarioSerialization::OnsetMethod::mkl,
            ImpresarioSerialization::OnsetMethod::specflux
    };
    inline static const std::array<ImpresarioSerialization::PitchMethod, 1> PITCH_PROCESSORS = {
//        ImpresarioSerialization::PitchMethod::schmitt,
//        ImpresarioSerialization::PitchMethod::fcomb,
//        ImpresarioSerialization::PitchMethod::mcomb,
//        ImpresarioSerialization::PitchMethod::yin,
//        ImpresarioSerialization::PitchMethod::yinfast,
            ImpresarioSerialization::PitchMethod::yinfft
    };

    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;

public:
    AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream);

    ~AudioProcessorSuite();

    void activate();

    static void activateAudioProcessor(AudioProcessor &audioProcessor);
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSUITE_H
