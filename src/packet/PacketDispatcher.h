#ifndef CONDUCTOR_PACKETDISPATCHER_H
#define CONDUCTOR_PACKETDISPATCHER_H

#include <ImpresarioUtils.h>
#include "packet/PacketConduit.h"

namespace conductor {

template<class T>
class PacketDispatcher : virtual impresarioUtils::NonCopyable {
private:
    PacketConduit<T> &packetConduit;

public:
    explicit PacketDispatcher(PacketConduit<T> &packetConduit)
            : packetConduit{packetConduit} {

    }

    void sendPacket(std::unique_ptr<const T> packet) {
        packetConduit.sendPacket(move(packet));
    }

};

}

#endif //CONDUCTOR_PACKETDISPATCHER_H
