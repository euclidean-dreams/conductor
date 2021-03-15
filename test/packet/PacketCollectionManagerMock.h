#ifndef CONDUCTOR_PACKETCOLLECTIONMANAGERMOCK_H
#define CONDUCTOR_PACKETCOLLECTIONMANAGERMOCK_H

#include <gmock/gmock.h>
#include "packet/PacketCollectionManager.h"

namespace conductor {

class PacketCollectionManagerMock : public PacketCollectionManager {
public:
    MOCK_METHOD(void, concludePacketCollectionUse, (int size));

};

}

#endif //CONDUCTOR_PACKETCOLLECTIONMANAGERMOCK_H
