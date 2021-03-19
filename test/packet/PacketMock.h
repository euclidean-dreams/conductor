#ifndef CONDUCTOR_PACKETMOCK_H
#define CONDUCTOR_PACKETMOCK_H

#include <flatbuffers/flatbuffers.h>
#include <gmock/gmock.h>
#include "packet/Packet.h"

namespace conductor {

class PacketMock : public Packet {
public:
    MOCK_METHOD(std::unique_ptr<flatbuffers::FlatBufferBuilder>, serialize, (), (const));
    MOCK_METHOD(ImpresarioSerialization::Identifier, getIdentifier, (), (const));
};

}

#endif //CONDUCTOR_PACKETMOCK_H
