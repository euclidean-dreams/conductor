#ifndef CONDUCTOR_PACKET_H
#define CONDUCTOR_PACKET_H

#include <NonCopyable.h>

namespace conductor {

class Packet : impresarioUtils::NonCopyable {
public:
    virtual ~Packet() = default;

};

}

#endif //CONDUCTOR_PACKET_H
