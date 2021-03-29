#include "FileWriter.h"

namespace conductor {

FileWriter::FileWriter(std::unique_ptr<PacketSpout> input, const std::string &relativePath)
        : input{move(input)},
          fileStream{} {
    std::ostringstream qualifiedFileName;
    qualifiedFileName << FILE_PATH << relativePath;
    auto path = qualifiedFileName.str();
    fileStream.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
}

FileWriter::~FileWriter() {
    fileStream.close();
}

bool FileWriter::shouldContinue() {
    return true;
}

}
