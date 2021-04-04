#ifndef CONDUCTOR_FILEWRITABLE_H
#define CONDUCTOR_FILEWRITABLE_H

#include <fstream>

namespace conductor {

class FileWritable {
public:
    virtual ~FileWritable() = default;

    virtual void writeToFile(std::ofstream &fileStream) const = 0;
};

}

#endif //CONDUCTOR_FILEWRITABLE_H
