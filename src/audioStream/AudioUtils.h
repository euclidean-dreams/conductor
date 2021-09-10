#ifndef CONDUCTOR_AUDIOUTILS_H
#define CONDUCTOR_AUDIOUTILS_H

#include <stdexcept>
#include <portaudio.h>

namespace conductor {

void throwOnPortaudioError(PaError portaudioReturnCode);

}

#endif //CONDUCTOR_AUDIOUTILS_H
