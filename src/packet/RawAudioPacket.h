#ifndef CONDUCTOR_RAWAUDIOPACKET_H
#define CONDUCTOR_RAWAUDIOPACKET_H

#include <vector>
#include "packet/AudioPacket.h"
#include "packet/FileWritable.h"

namespace conductor {

class RawAudioPacket : public AudioPacket, public FileWritable {
private:
    std::vector<float> data;
    int maxSize;
    int addIndex;
    bool finalized;

public:
    RawAudioPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size);

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;

    void writeToFile(std::ofstream &fileStream) const override;
};

}

#endif //CONDUCTOR_RAWAUDIOPACKET_H
