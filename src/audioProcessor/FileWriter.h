#ifndef CONDUCTOR_FILEWRITER_H
#define CONDUCTOR_FILEWRITER_H

#include <sstream>
#include <fstream>
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketReceiver.h"
#include "packet/FileWritable.h"
#include "packet/RawAudioPacket.h"

namespace conductor {

template<class T>
class FileWriter : public AudioProcessor {
private:
    inline static const std::string_view FILE_PATH = "/root/output/";

protected:
    std::unique_ptr<PacketReceiver<T>> input;
    std::ofstream fileStream;

public:
    FileWriter(std::unique_ptr<PacketReceiver<T>> input, const std::string &relativePath)
    : input{move(input)},
        fileStream{} {
        std::ostringstream qualifiedFileName;
        qualifiedFileName << FILE_PATH << relativePath;
        auto path = qualifiedFileName.str();
        fileStream.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    void process() override {
        auto packet = input->getPacket();
        packet->writeToFile(fileStream);
    }
};

}

#endif //CONDUCTOR_FILEWRITER_H
