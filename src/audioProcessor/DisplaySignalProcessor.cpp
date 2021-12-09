#include "DisplaySignalProcessor.h"

namespace conductor {

DisplaySignalProcessor::DisplaySignalProcessor(std::unique_ptr<PacketReceiver<MelSignalPacket>> input,
                                               std::unique_ptr<PacketDispatcher<Serializable>> output)
        : input{move(input)},
          output{move(output)} {

}

void DisplaySignalProcessor::process() {
    auto currentPacket = input->getPacket();
    auto outputPacket = std::make_unique<DisplaySignalPacket>(
            currentPacket->getOriginTimestamp(), currentPacket->size(), currentPacket, std::vector<float>{}
    );
    for (int index = 0; index < currentPacket->size(); index++) {
        outputPacket->addSample(static_cast<float>(currentPacket->getSample(index)));
    }
    output->sendPacket(move(outputPacket));
}

}
