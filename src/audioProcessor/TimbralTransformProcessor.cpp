#include "TimbralTransformProcessor.h"

namespace conductor {

TimbralTransformProcessor::TimbralTransformProcessor(std::unique_ptr<PacketReceiver<STFTPacket>> input,
                                                     std::unique_ptr<PacketDispatcher<HarmonicTransformPacket>> output)
        : input{move(input)},
          output{move(output)},
          psi{6} {

}

void TimbralTransformProcessor::process() {
    auto currentPacket = input->getPacket();
    auto &currentSTFT = *currentPacket;

    // create output packet
    auto outputPacket = std::make_unique<HarmonicTransformPacket>(
            currentSTFT.getOriginTimestamp(), currentSTFT.getFrequencyBand(), currentSTFT.size() * psi, currentPacket
    );
    for (int index = 0; index < currentSTFT.size(); index++) {
        for (int psindex = 1; psindex <= psi; psindex++) {
            auto timbralIndex = index * psindex;
            if (timbralIndex < currentSTFT.size() - 1) {
                outputPacket->addSample(currentSTFT.getMagnitude(timbralIndex));
            } else {
                outputPacket->addSample(0);
            }
        }
    }
    output->sendPacket(move(outputPacket));
}

}
