#ifndef CONDUCTOR_SPECTROGRAMPACKET_H
#define CONDUCTOR_SPECTROGRAMPACKET_H

#include <vector>
#include <ImpresarioUtils.h>
#include "packet/AudioPacket.h"
#include "packet/FileWritable.h"

namespace conductor {

class SpectrogramPacket : public AudioPacket, public FileWritable {
private:
    std::vector<std::vector<double>> layers;

public:
    SpectrogramPacket(uint64_t originTimestamp, int layerCount);

    void writeToFile(std::ofstream &fileStream) const override;

    void addData(std::vector<double> &data);
};

}

#endif //CONDUCTOR_SPECTROGRAMPACKET_H
