#ifndef CONDUCTOR_AUDIOFILEWRITER_H
#define CONDUCTOR_AUDIOFILEWRITER_H

#include <sstream>
#include <string_view>
#include <iostream>
#include <fstream>
#include <Time.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketSpout.h"
#include "packet/RawAudioPacket.h"

namespace conductor {

class AudioFileWriter : public AudioProcessor {
private:
    inline static const std::string_view FILE_PATH = "/root/output/";

    std::unique_ptr<PacketSpout> input;
    ImpresarioSerialization::FrequencyBand frequencyBand;
    std::ofstream fileStream;

public:
    AudioFileWriter(std::unique_ptr<PacketSpout> input, ImpresarioSerialization::FrequencyBand frequencyBand);

    ~AudioFileWriter() override;

    void setup() override {};

    void process() override;

    bool shouldContinue() override;

};

}

#endif //CONDUCTOR_AUDIOFILEWRITER_H
