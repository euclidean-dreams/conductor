#ifndef CONDUCTOR_CLEANABLE_H
#define CONDUCTOR_CLEANABLE_H

#include <ImpresarioUtils.h>

namespace conductor {

class Cleanable : virtual impresarioUtils::NonCopyable {
public:
    virtual ~Cleanable() = default;

    virtual void clean() = 0;
};

}

#endif //CONDUCTOR_CLEANABLE_H
