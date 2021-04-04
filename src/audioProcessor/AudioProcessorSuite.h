#ifndef CONDUCTOR_AUDIOPROCESSORSUITE_H
#define CONDUCTOR_AUDIOPROCESSORSUITE_H

#include <thread>
#include <memory>
#include <vector>
#include <zmq.hpp>
#include <ImpresarioUtils.h>
#include "audioProcessor/BandpassProcessor.h"
#include "audioProcessor/STFTProcessor.h"
#include "audioProcessor/AudioStreamSource.h"
#include "audioProcessor/AudioProcessorSink.h"
#include "audioProcessor/onsetProcessor/SpecFluxOnsetProcessor.h"
#include "audioProcessor/FileWriter.h"
#include "audioProcessor/PacketTypeConverter.h"
#include "audioStream/AudioStream.h"
#include "packet/PacketConduit.h"
#include "packet/PacketConduitCurator.h"
#include "packet/FileWritable.h"

namespace conductor {

class AudioProcessorSuite : virtual impresarioUtils::NonCopyable {
private:
    inline static const std::string_view PARAMETER_ENDPOINT = "tcp://10.0.0.132:44440";
    inline static const std::string_view PERFORMER_OUTPUT_ENDPOINT = "tcp://*:44441";
    inline static const std::string_view DATA_OUTPUT_ENDPOINT = "tcp://*:44442";
    inline static const bool RECORD_TO_FILES = false;
    inline static const bool SEND_DATA = true;
    inline static const std::map<
            ImpresarioSerialization::FrequencyBand,
            std::tuple<
                    std::vector<ImpresarioSerialization::OnsetMethod>,
                    std::vector<ImpresarioSerialization::PitchMethod>
            >
    > SUITE_CONFIGURATION = {
            {ImpresarioSerialization::FrequencyBand::subBass, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {}
            }},
            {ImpresarioSerialization::FrequencyBand::bass, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {}
            }},
            {ImpresarioSerialization::FrequencyBand::lowMidrange, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {}
            }},
            {ImpresarioSerialization::FrequencyBand::midrange, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {
                            ImpresarioSerialization::PitchMethod::yinfft
                    }
            }},
            {ImpresarioSerialization::FrequencyBand::upperMidrange, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {}
            }},
            {ImpresarioSerialization::FrequencyBand::presence, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {}
            }},
            {ImpresarioSerialization::FrequencyBand::brilliance, {
                    {
                            ImpresarioSerialization::OnsetMethod::specflux
                    }, {}
            }}
    };

    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;
    std::unique_ptr<PacketConduitCurator> packetConduitCurator;

public:
    AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream);

    void activate();

    static void activateAudioProcessor(AudioProcessor &audioProcessor);
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSUITE_H
