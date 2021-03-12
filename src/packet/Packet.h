#ifndef CONDUCTOR_PACKET_H
#define CONDUCTOR_PACKET_H

#include <zmq_addon.hpp>
#include <NonCopyable.h>

namespace conductor {

class Packet : impresarioUtils::NonCopyable {
public:
    virtual ~Packet() = default;

    virtual std::unique_ptr<zmq::multipart_t> serialize() const = 0;
};

}

#endif //CONDUCTOR_PACKET_H
