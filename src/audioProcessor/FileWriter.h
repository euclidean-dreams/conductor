#ifndef CONDUCTOR_FILEWRITER_H
#define CONDUCTOR_FILEWRITER_H

#include <sstream>
#include <fstream>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketReceiver.h"
#include "packet/FileWritable.h"
#include "packet/RawAudioPacket.h"

namespace conductor {

template<class T>
class FileWriter : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<T>> input;
    std::ofstream fileStream;

public:
    FileWriter(std::unique_ptr<PacketReceiver<T>> input, const std::string &relativePath)
            : input{move(input)},
              fileStream{} {
        std::ostringstream qualifiedFileName;
        qualifiedFileName << Config::getInstance().getOutputFilePath() << relativePath;
        auto path = qualifiedFileName.str();
        fileStream.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    ~FileWriter() {
        fileStream.close();
    }

    void process() override {
        auto packet = input->getPacket();
        packet->writeToFile(fileStream);
    }
};

}

#endif //CONDUCTOR_FILEWRITER_H
