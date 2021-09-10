#ifndef CONDUCTOR_CLEANABLE_H
#define CONDUCTOR_CLEANABLE_H

namespace conductor {

class Cleanable {
public:
    virtual ~Cleanable() = default;

    virtual void clean() = 0;
};

}

#endif //CONDUCTOR_CLEANABLE_H
