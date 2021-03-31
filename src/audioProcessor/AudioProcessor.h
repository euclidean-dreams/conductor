#ifndef CONDUCTOR_AUDIOPROCESSOR_H
#define CONDUCTOR_AUDIOPROCESSOR_H

#include <ImpresarioUtils.h>

namespace conductor {

class AudioProcessor : virtual impresarioUtils::NonCopyable {
public:
    virtual ~AudioProcessor() = default;

    virtual void setup() {};

    virtual void process() = 0;

    bool shouldContinue() { return true; };
};

}

#endif //PERFORMER_AUDIOPROCESSOR_H
