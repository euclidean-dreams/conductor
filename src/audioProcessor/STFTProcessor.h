#ifndef CONDUCTOR_STFTPROCESSOR_H
#define CONDUCTOR_STFTPROCESSOR_H

#include <stdexcept>
#include <cmath>
#include <NE10.h>
#include <ImpresarioUtils.h>
#include "Config.h"
#include "packet/RawAudioPacket.h"
#include "packet/STFTPacket.h"
#include "packet/PacketDispatcher.h"
#include "packet/PacketReceiver.h"
#include "audioProcessor/AudioProcessor.h"

namespace conductor {

enum class WindowFunction {
    hamming,
    hann
};

class STFTProcessor : public AudioProcessor {
private:
    inline static const int HOP_SIZE = 1; // power of 2
    inline static const int WINDOW_SIZE = 4; // power of 2
    inline static const int FFT_SIZE = WINDOW_SIZE * AUDIO_PACKET_SIZE;
    std::unique_ptr<PacketReceiver<RawAudioPacket>> input;
    std::unique_ptr<PacketDispatcher<STFTPacket>> output;
    WindowFunction windowFunction;
    ne10_float32_t *fftInput;
    ne10_fft_cpx_float32_t *fftOutput;
    ne10_fft_r2c_cfg_float32_t fftPlan;
    std::list<std::unique_ptr<PacketCollection<RawAudioPacket>>> currentPackets;

    static float hammingWindow(int sampleNumber);

    static float hannWindow(int sampleNumber);

public:
    STFTProcessor(std::unique_ptr<PacketReceiver<RawAudioPacket>> input,
                  std::unique_ptr<PacketDispatcher<STFTPacket>> output,
                  WindowFunction windowFunction);

    ~STFTProcessor() override;

    void process() override;
};

}

#endif //CONDUCTOR_STFTPROCESSOR_H
