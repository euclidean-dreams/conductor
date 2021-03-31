#ifndef CONDUCTOR_PACKETCOLLECTION_H
#define CONDUCTOR_PACKETCOLLECTION_H

#include <memory>
#include <vector>
#include <ImpresarioUtils.h>
#include "packet/Packet.h"

namespace conductor {

template<class T>
class PacketCollection : virtual impresarioUtils::NonCopyable {
private:
    std::vector<std::shared_ptr<const T>> packets;

public:
    explicit PacketCollection()
            : packets{} {

    }

    void addPacket(std::shared_ptr<const T> packet) {
        packets.push_back(move(packet));
    }

    const T &getPacket(int index) {
        return *packets[index];
    }

    typename std::vector<std::shared_ptr<const T>>::const_iterator begin() const {
        return packets.cbegin();
    }

    typename std::vector<std::shared_ptr<const T>>::const_iterator end() const {
        return packets.cend();
    }
};

}

#endif //CONDUCTOR_PACKETCOLLECTION_H
