#ifndef CONDUCTOR_PACKETCOLLECTIONMANAGER_H
#define CONDUCTOR_PACKETCOLLECTIONMANAGER_H

#include <NonCopyable.h>

namespace conductor {

class PacketCollectionManager : impresarioUtils::NonCopyable {
public:
    virtual ~PacketCollectionManager() = default;

    virtual void concludePacketCollectionUse(int size) = 0;
};

}

#endif //CONDUCTOR_PACKETCOLLECTIONMANAGER_H
