#include "CuriousOnsetDetector.h"

namespace conductor {

CuriousOnsetDetector::CuriousOnsetDetector(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                           std::unique_ptr<PacketDispatcher<SpectrogramPacket>> output,
                                           std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket)
        : input{move(input)},
          output{move(output)},
          parameterSocket{move(parameterSocket)},
          perceptrons{} {
}

void CuriousOnsetDetector::updateAlgorithmParameters() {
    auto parameters = parameterSocket->receiveSerializedData(zmq::recv_flags::dontwait);
    if (parameters != nullptr) {
        auto onsetParameters = ImpresarioSerialization::GetOnsetProcessorParameters(parameters->getBuffer());
    }
}

void CuriousOnsetDetector::process() {
    updateAlgorithmParameters();
    auto currentPacket = input->getPacket();
    auto &currentSTFT = *currentPacket;

    // fill perceptron list if this is the first invocation
    if (perceptrons.empty()) {
        for (int bin = 0; bin < currentSTFT.size(); bin++) {
            auto perceptron = std::make_unique<CuriousPerceptron>();
            perceptrons.push_back(move(perceptron));
        }
    }

    // send new data to perceptrons
    int index = 0;
    for (auto &perceptron: perceptrons) {
        perceptron->addSample(currentSTFT.getMagnitude(index));
        index++;
    }

    auto outputSpectrogramPacket = std::make_unique<SpectrogramPacket>(currentSTFT.getOriginTimestamp(),
                                                                       currentSTFT.getFrequencyBand());
    for (auto &perceptron: perceptrons) {
        outputSpectrogramPacket->addSample(perceptron->magnitudes.front(), perceptron->spectralFluxes.front(),
                                           perceptron->fluxyFluxes.front(), perceptron->getPeaks(),
                                           perceptron->fired());
    }
    output->sendPacket(move(outputSpectrogramPacket));

//    auto resultPacket = std::make_unique<OnsetPacket>(currentSTFT.getOriginTimestamp(),
//                                                      currentSTFT.getFrequencyBand(),
//                                                      currentSTFT.getOriginTimestamp(),
//                                                      ImpresarioSerialization::OnsetMethod::specflux,
//                                                      1);
//    output->sendPacket(move(resultPacket));
}

float CuriousOnsetDetector::halfWaveRectifier(float x) {
    return (x + std::abs(x)) / 2;
}

}
