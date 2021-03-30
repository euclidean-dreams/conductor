#ifndef CONDUCTOR_PACKETCOLLECTIONMANAGER_H
#define CONDUCTOR_PACKETCOLLECTIONMANAGER_H

#include <NonCopyable.h>

namespace conductor {

class PacketCollectionManager : impresarioUtils::NonCopyable {
public:
    virtual ~PacketCollectionManager() = default;
};

}

#endif //CONDUCTOR_PACKETCOLLECTIONMANAGER_H
