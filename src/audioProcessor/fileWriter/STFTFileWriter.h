#ifndef CONDUCTOR_STFTFILEWRITER_H
#define CONDUCTOR_STFTFILEWRITER_H

#include "audioProcessor/fileWriter/FileWriter.h"
#include "packet/STFTPacket.h"

namespace conductor {

class STFTFileWriter : public FileWriter {
public:
    STFTFileWriter(std::unique_ptr<PacketSpout> input, const std::string &fileName);

    void process() override;
};

}

#endif //CONDUCTOR_STFTFILEWRITER_H
