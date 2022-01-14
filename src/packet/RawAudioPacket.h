#ifndef CONDUCTOR_RAWAUDIOPACKET_H
#define CONDUCTOR_RAWAUDIOPACKET_H

#include "packet/AudioPacket.h"
#include "packet/FileWritable.h"
#include "packet/Signal.h"

namespace conductor {

class RawAudioPacket : public AudioPacket, public FileWritable {
private:
    Signal<float> signal;

public:
    RawAudioPacket(uint64_t originTimestamp, int size);

    void writeToFile(std::ofstream &fileStream) const override;

    void addSample(float sample);

    float getSample(int index) const;

    int size() const;
};

}

#endif //CONDUCTOR_RAWAUDIOPACKET_H
