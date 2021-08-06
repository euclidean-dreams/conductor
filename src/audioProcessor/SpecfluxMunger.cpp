#include "SpecfluxMunger.h"

conductor::SpecfluxMunger::SpecfluxMunger(std::unique_ptr<PacketReceiver<HarmonicTransformPacket>> input,
                                          std::unique_ptr<PacketDispatcher<STFTPacket>> output)
        : input{move(input)},
          output{move(output)} {

}

void conductor::SpecfluxMunger::process() {
    auto inputPacket = input->getPacket();

    auto &harmonicTransform = *inputPacket;
    auto &stft = harmonicTransform.getSTFTPacket();

    std::vector<std::complex<float>> outputSignal{};
    outputSignal.reserve(harmonicTransform.size());
    for (int index = 0; index < harmonicTransform.size(); index++) {
        auto harmonicTransformSample = harmonicTransform.getSample(index);
        if (harmonicTransformSample < 0) {
            harmonicTransformSample = 0;
        }
        auto harmonicallyScaledSample = stft.getMagnitude(index) * harmonicTransformSample;
        if (harmonicallyScaledSample < 0) {
            harmonicallyScaledSample = 0;
        }
        auto sample = stft.getMagnitude(index) - harmonicallyScaledSample * 100;
        if (sample < 0) {
            sample = 0;
        }
        auto rooted_sample = std::complex<float>(std::sqrt(sample), std::sqrt(sample));
        outputSignal.push_back(rooted_sample);
    }

    auto outputPacket = std::make_unique<STFTPacket>(inputPacket->getOriginTimestamp(),
                                                     inputPacket->getFrequencyBand(), inputPacket->size(),
                                                     (inputPacket->size() - 1) * 2);
    for (int i = 0; i < harmonicTransform.size(); i++) {
        outputPacket->addSample(outputSignal[i]);
    }
    output->sendPacket(move(outputPacket));
}
