#ifndef CONDUCTOR_PACKETCONDUITCURATOR_H
#define CONDUCTOR_PACKETCONDUITCURATOR_H

#include <thread>
#include <vector>
#include <ImpresarioUtils.h>
#include "packet/Cleanable.h"

namespace conductor {

class PacketConduitCurator : impresarioUtils::NonCopyable {
private:
    inline static const int TICK_INTERVAL_MICROSECONDS = 1000;

    std::vector<std::unique_ptr<Cleanable>> packetConduits;

    void clean();

public:
    static void startCleaningLoop(std::unique_ptr<PacketConduitCurator> packetConduitCurator);

    PacketConduitCurator();

    bool finished();

    void addPacketConduit(std::unique_ptr<Cleanable> packetConduit);
};

}

#endif //CONDUCTOR_PACKETCONDUITCURATOR_H
