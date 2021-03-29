#ifndef CONDUCTOR_RAWAUDIOFILEWRITER_H
#define CONDUCTOR_RAWAUDIOFILEWRITER_H

#include "audioProcessor/fileWriter/FileWriter.h"
#include "packet/RawAudioPacket.h"

namespace conductor {

class RawAudioFileWriter : public FileWriter {
public:
    RawAudioFileWriter(std::unique_ptr<PacketSpout> input, const std::string &fileName);

    void process() override;
};

}

#endif //CONDUCTOR_RAWAUDIOFILEWRITER_H
