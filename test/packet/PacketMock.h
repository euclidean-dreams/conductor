#ifndef CONDUCTOR_PACKETMOCK_H
#define CONDUCTOR_PACKETMOCK_H

#include <gmock/gmock.h>
#include "packet/Packet.h"

namespace conductor {

class PacketMock : public Packet {
public:
    MOCK_CONST_METHOD0(serialize, std::unique_ptr<zmq::multipart_t>());
};

}

#endif //CONDUCTOR_PACKETMOCK_H
