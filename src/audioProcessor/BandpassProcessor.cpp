#include "BandpassProcessor.h"

namespace conductor {

BandpassProcessor::BandpassProcessor(std::unique_ptr<PacketSpout> input, std::unique_ptr<PacketConduit> output,
                                     ImpresarioSerialization::FrequencyBand frequencyBand)
        : input{move(input)},
          output{move(output)},
          frequencyBand{frequencyBand},
          filter{} {
    auto frequencyBandValues = FREQUENCY_BAND_VALUES.at(frequencyBand);
    auto minFrequency = frequencyBandValues.first;
    auto maxFrequency = frequencyBandValues.second;
    auto width = maxFrequency - minFrequency;
    auto centerFrequency = width / 2 + minFrequency;
    filter.setup(SAMPLE_RATE, centerFrequency, width);
}

void BandpassProcessor::setup() {

}

void BandpassProcessor::process() {
    auto inputPackets = input->getPackets(1);
    auto &inputPacket = RawAudioPacket::from(inputPackets->getPacket(0));
    auto outputPacket = std::make_unique<RawAudioPacket>(inputPacket.getSampleTimestamp(), frequencyBand,
                                                         inputPacket.size());
    for (int i = 0; i < inputPacket.size(); i++) {
        outputPacket->addSample(filter.filter(inputPacket.getSample(i)));
    }
    output->sendPacket(move(outputPacket));
}

bool BandpassProcessor::shouldContinue() {
    return true;
}

}
