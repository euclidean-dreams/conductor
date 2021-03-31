#include "PacketConduitCurator.h"

namespace conductor {

void PacketConduitCurator::startCleaningLoop(std::unique_ptr<PacketConduitCurator> packetConduitCurator) {
    while (!packetConduitCurator->finished()) {
        auto cycleStartTime = impresarioUtils::getCurrentTime();
        packetConduitCurator->clean();
        auto totalCycleTime = impresarioUtils::getElapsedTime(cycleStartTime);
        if (TICK_INTERVAL_MICROSECONDS > totalCycleTime) {
            std::this_thread::sleep_for(
                    std::chrono::microseconds(TICK_INTERVAL_MICROSECONDS - totalCycleTime)
            );
        }
    }

}

PacketConduitCurator::PacketConduitCurator()
        : packetConduits{} {

}

void PacketConduitCurator::clean() {
    for (auto &packetConduit: packetConduits) {
        packetConduit->clean();
    }
}

bool PacketConduitCurator::finished() {
    return false;
}

void PacketConduitCurator::addPacketConduit(std::unique_ptr<Cleanable> packetConduit) {
    packetConduits.push_back(move(packetConduit));
}

}
