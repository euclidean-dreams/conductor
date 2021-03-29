#ifndef CONDUCTOR_FILEWRITER_H
#define CONDUCTOR_FILEWRITER_H

#include <sstream>
#include <fstream>
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketSpout.h"

namespace conductor {

class FileWriter : public AudioProcessor {
private:
    inline static const std::string_view FILE_PATH = "/root/output/";

protected:
    std::unique_ptr<PacketSpout> input;
    std::ofstream fileStream;

public:
    FileWriter(std::unique_ptr<PacketSpout> input, const std::string &relativePath);

    ~FileWriter() override;

    void setup() override {};

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_FILEWRITER_H
