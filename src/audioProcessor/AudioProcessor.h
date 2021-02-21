#ifndef CONDUCTOR_AUDIOPROCESSOR_H
#define CONDUCTOR_AUDIOPROCESSOR_H

#include "NonCopyable.h"

class AudioProcessor : NonCopyable {
public:
    virtual ~AudioProcessor() = default;

    virtual void setup() = 0;

    virtual void process() = 0;

    virtual bool shouldContinue() = 0;
};

#endif //PERFORMER_AUDIOPROCESSOR_H
