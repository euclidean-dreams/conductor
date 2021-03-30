#ifndef CONDUCTOR_STFTPROCESSOR_H
#define CONDUCTOR_STFTPROCESSOR_H

#include <cmath>
#include <NE10.h>
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
    inline static const int HOP_SIZE = 1; // power of 2
    inline static const int WINDOW_SIZE = 4; // power of 2
    inline static const int FFT_SIZE = WINDOW_SIZE * AUDIO_PACKET_SIZE;
    std::unique_ptr<PacketSpout> input;
    std::unique_ptr<PacketConduit> output;
    ne10_float32_t *fftInput;
    ne10_fft_cpx_float32_t *fftOutput;
    ne10_fft_r2c_cfg_float32_t fftPlan;
    std::list<std::unique_ptr<PacketCollection>> currentPackets;

    static float hammingWindow(int sampleNumber);

public:
    STFTProcessor(std::unique_ptr<PacketSpout> input, std::unique_ptr<PacketConduit> output);

    ~STFTProcessor() override;

    void setup() override;

    void process() override;

    bool shouldContinue() override;
};

}

#endif //CONDUCTOR_STFTPROCESSOR_H
