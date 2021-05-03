#ifndef CONDUCTOR_STFTPROCESSOR_H
#define CONDUCTOR_STFTPROCESSOR_H

#ifdef USE_MUFFT

#include <fft.h>
#include <fft_internal.h>

#else
#include <NE10.h>
#endif // USE_MUFFT

#include <stdexcept>
#include <cmath>
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
    std::unique_ptr<PacketReceiver<RawAudioPacket>> input;
    std::unique_ptr<PacketDispatcher<STFTPacket>> output;
    WindowFunction windowFunction;
    int hopSize;
    int windowSize;
    int fftSize;
#ifdef USE_MUFFT
    float *fftInput;
    cfloat *fftOutput;
    mufft_plan_1d *fftPlan;
#else
    ne10_float32_t *fftInput;
    ne10_fft_cpx_float32_t *fftOutput;
    ne10_fft_r2c_cfg_float32_t fftPlan;
#endif // USE_MUFFT
    std::list<std::unique_ptr<PacketCollection<RawAudioPacket>>> currentPackets;

    float hammingWindow(int sampleNumber);

    float hannWindow(int sampleNumber);

public:
    STFTProcessor(std::unique_ptr<PacketReceiver<RawAudioPacket>> input,
                  std::unique_ptr<PacketDispatcher<STFTPacket>> output,
                  WindowFunction windowFunction);

    ~STFTProcessor() override;

    void process() override;
};

}

#endif //CONDUCTOR_STFTPROCESSOR_H
