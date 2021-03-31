#ifndef CONDUCTOR_FILEWRITABLE_H
#define CONDUCTOR_FILEWRITABLE_H

#include <fstream>
#include <ImpresarioUtils.h>

namespace conductor {

class FileWritable : virtual impresarioUtils::NonCopyable {
public:
    virtual ~FileWritable() = default;

    virtual void writeToFile(std::ofstream &fileStream) const = 0;
};

}

#endif //CONDUCTOR_FILEWRITABLE_H
