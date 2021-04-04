#ifndef CONDUCTOR_SPECFLUXONSETPROCESSOR_H
#define CONDUCTOR_SPECFLUXONSETPROCESSOR_H

#include <ImpresarioUtils.h>
#include <ImpresarioSerialization.h>
#include "Config.h"
#include "audioProcessor/AudioProcessor.h"
#include "packet/STFTPacket.h"
#include "packet/OnsetPacket.h"
#include "packet/PacketReceiver.h"
#include "packet/PacketDispatcher.h"
#include "packet/AudioPacket.h"

namespace conductor {

class SpecFluxOnsetProcessor : public AudioProcessor {
private:
    inline static const float DEFAULT_THRESHOLD = 0.02;
    inline static const int DEFAULT_PEAK_PICKING_WINDOW_SIZE = 3;
    inline static const int DEFAULT_PEAK_PICKING_WINDOW_TAIL_MULTIPLIER = 3;

    std::unique_ptr<PacketReceiver<STFTPacket>> input;
    std::unique_ptr<PacketDispatcher<Serializable>> output;
    std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket;
    ImpresarioSerialization::FrequencyBand frequencyBand;
    std::shared_ptr<const STFTPacket> previousPacket;
    std::list<float> spectralFluxCatalog;
    std::_List_const_iterator<float> targetSpectralFluxIterator;
    std::list<uint64_t> spectralFluxTimestamps;
    float threshold;
    int peakPickingWindowSize;
    int peakPickingWindowTailMultiplier;

    int expectedSpectralFluxCatalogSize() const;

    int expectedSpectralFluxTimestampSize() const;

    void adjustTargetSpectralFluxIterator();

    void updateAlgorithmParameters();

    void catalogNextSpectralFlux(const STFTPacket &currentSTFT);

    static float calculateSpectralFlux(const STFTPacket &currentSTFT, const STFTPacket &previousSTFT);

    static float halfWaveRectifier(float x);

    bool onsetDetected();

public:
    SpecFluxOnsetProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                           std::unique_ptr<PacketDispatcher<Serializable>> output,
                           std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                           ImpresarioSerialization::FrequencyBand frequencyBand);

    void process() override;
};

}

#endif //PERFORMER_ONSETPROCESSOR_H
