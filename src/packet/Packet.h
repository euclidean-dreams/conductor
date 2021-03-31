#ifndef CONDUCTOR_PACKET_H
#define CONDUCTOR_PACKET_H

#include <ImpresarioUtils.h>

namespace conductor {

class Packet : virtual impresarioUtils::NonCopyable {
public:
    virtual ~Packet() = default;
};

}

#endif //CONDUCTOR_PACKET_H
