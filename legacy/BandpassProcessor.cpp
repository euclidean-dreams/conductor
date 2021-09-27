#include "BandpassProcessor.h"

namespace conductor {

BandpassProcessor::BandpassProcessor(std::unique_ptr<PacketReceiver<RawAudioPacket>> input,
                                     std::unique_ptr<PacketDispatcher<RawAudioPacket>> output,
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
    filter.setup(Config::getInstance().getSampleRate(), centerFrequency, width);
}

void BandpassProcessor::process() {
    auto inputPacket = input->getPacket();
    auto outputPacket = std::make_unique<RawAudioPacket>(inputPacket->getOriginTimestamp(), frequencyBand,
                                                         inputPacket->size());
    for (int i = 0; i < inputPacket->size(); i++) {
        outputPacket->addSample(filter.filter(inputPacket->getSample(i)));
    }
    output->sendPacket(move(outputPacket));
}

}
