#ifndef CONDUCTOR_AUDIOFILEREADER_H
#define CONDUCTOR_AUDIOFILEREADER_H

#include <fstream>
#include <ImpresarioUtils.h>
#include "Config.h"
#include "audioStream/AudioFileStream.h"

namespace conductor {

class AudioFileReader : virtual impresarioUtils::NonCopyable {
private:
    AudioFileStream outputStream;

public:
    explicit AudioFileReader(int packetSize);

    AudioStream &getAudioStream();
};

}

#endif //CONDUCTOR_AUDIOFILEREADER_H
