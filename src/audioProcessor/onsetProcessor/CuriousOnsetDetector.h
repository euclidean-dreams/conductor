#ifndef CONDUCTOR_CURIOUSONSETDETECTOR_H
#define CONDUCTOR_CURIOUSONSETDETECTOR_H

#include <iostream>
#include <ImpresarioUtils.h>
#include <ImpresarioSerialization.h>
#include "audioProcessor/AudioProcessor.h"
#include "packet/STFTPacket.h"
#include "packet/OnsetPacket.h"
#include "packet/SpectrogramPacket.h"
#include "packet/PacketReceiver.h"
#include "packet/PacketDispatcher.h"
#include "packet/AudioPacket.h"
#include "audioProcessor/onsetProcessor/CuriousPerceptron.h"

namespace conductor {

class CuriousOnsetDetector : public AudioProcessor {
private:
    std::unique_ptr<PacketReceiver<STFTPacket>> input;
    std::unique_ptr<PacketDispatcher<Serializable>> output;
    std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket;
    std::list<std::unique_ptr<CuriousPerceptron>> perceptrons;

    void updateAlgorithmParameters();

    static float halfWaveRectifier(float x);

public:
    CuriousOnsetDetector(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                         std::unique_ptr<PacketDispatcher<Serializable>> output,
                         std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket);

    void process() override;
};

}

#endif //CONDUCTOR_CURIOUSONSETDETECTOR_H
