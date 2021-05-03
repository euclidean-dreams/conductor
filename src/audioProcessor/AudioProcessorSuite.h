#ifndef CONDUCTOR_AUDIOPROCESSORSUITE_H
#define CONDUCTOR_AUDIOPROCESSORSUITE_H

#include <thread>
#include <memory>
#include <vector>
#include <zmq.hpp>
#include <ImpresarioUtils.h>
#include "Config.h"
#include "audioProcessor/BandpassProcessor.h"
#include "audioProcessor/STFTProcessor.h"
#include "audioProcessor/AudioStreamSource.h"
#include "audioProcessor/AudioProcessorSink.h"
#include "audioProcessor/onsetProcessor/SpecFluxOnsetProcessor.h"
#include "audioProcessor/HarmonicTransformProcessor.h"
#include "audioProcessor/SpectrogramProcessor.h"
#include "audioProcessor/FileWriter.h"
#include "audioProcessor/PacketTypeConverter.h"
#include "audioStream/AudioStream.h"
#include "packet/PacketConduit.h"
#include "packet/PacketConduitCurator.h"
#include "packet/FileWritable.h"
#include "packet/HarmonicTransformPacket.h"
#include "packet/SpectrogramPacket.h"

namespace conductor {

class AudioProcessorSuite : virtual impresarioUtils::NonCopyable {
private:
    std::vector<std::unique_ptr<AudioProcessor>> audioProcessors;
    std::unique_ptr<PacketConduitCurator> packetConduitCurator;

public:
    AudioProcessorSuite(zmq::context_t &context, AudioStream &audioStream);

    void activate();

    static void activateAudioProcessor(AudioProcessor &audioProcessor);
};

}

#endif //CONDUCTOR_AUDIOPROCESSORSUITE_H
