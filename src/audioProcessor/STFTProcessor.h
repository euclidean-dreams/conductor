#ifndef CONDUCTOR_STFTPROCESSOR_H
#define CONDUCTOR_STFTPROCESSOR_H

#include <fft.h>
#include <fft_internal.h>
#include <Time.h>
#include "Config.h"
#include "packet/RawAudioPacket.h"
#include "packet/STFTPacket.h"
#include "packet/PacketConduit.h"
#include "packet/PacketSpout.h"
#include "audioProcessor/AudioProcessor.h"

namespace conductor {

class STFTProcessor : public AudioProcessor {
private:
    inline static const int HOP_SIZE = 2; // power of 2
    inline static const int WINDOW_SIZE = 8; // power of 2
    inline static const int FFT_SIZE = WINDOW_SIZE * AUDIO_PACKET_SIZE;
    std::unique_ptr<PacketSpout> input;
    std::unique_ptr<PacketConduit> output;
    float *fftInput;
    cfloat *fftOutput;
    mufft_plan_1d *fftPlan;
    std::list<std::unique_ptr<PacketCollection>> currentPackets;

    static float hannWindow(int sampleNumber);

public:
    STFTProcessor(std::unique_ptr<PacketSpout> input, std::unique_ptr<PacketConduit> output);

    ~STFTProcessor() override;

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_STFTPROCESSOR_H
