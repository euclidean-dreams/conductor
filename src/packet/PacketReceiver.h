#ifndef CONDUCTOR_PACKETRECEIVER_H
#define CONDUCTOR_PACKETRECEIVER_H

#include <ImpresarioUtils.h>
#include "packet/PacketConduit.h"

namespace conductor {

template<class T>
class PacketReceiver : virtual impresarioUtils::NonCopyable {
private:
    PacketConduit<T> &packetConduit;
    int id;

public:
    explicit PacketReceiver(PacketConduit<T> &packetConduit)
            : packetConduit{packetConduit},
              id{packetConduit.registerReceiver()} {

    }

    std::shared_ptr<const T> getPacket() {
        packetConduit.waitForAvailablePackets(id, 1);
        return packetConduit.getPacket(id);
    }

    std::unique_ptr<PacketCollection<T>> getPackets(int packetCount) {
        packetConduit.waitForAvailablePackets(id, packetCount);
        return packetConduit.getPackets(id, packetCount);
    }

};

}

#endif //CONDUCTOR_PACKETRECEIVER_H
