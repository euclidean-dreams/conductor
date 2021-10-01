#ifndef CONDUCTOR_PACKETTYPECONVERTER_H
#define CONDUCTOR_PACKETTYPECONVERTER_H

#include "audioProcessor/AudioProcessor.h"
#include "packet/PacketReceiver.h"
#include "packet/PacketDispatcher.h"

namespace conductor {

template<class originalType, class desiredType>
class PacketTypeConverter : public AudioProcessor {
    std::unique_ptr<PacketReceiver<originalType>> input;
    std::unique_ptr<PacketDispatcher<desiredType>> output;

public:
    PacketTypeConverter(std::unique_ptr<PacketReceiver<originalType>> input,
                        std::unique_ptr<PacketDispatcher<desiredType>> output)
            : input{move(input)},
              output{move(output)} {

    }

    void process() override {
        auto inputPacket = input->getPacket();
        auto copiedPacket = std::make_unique<originalType>(*inputPacket);
        std::unique_ptr<desiredType> outputPacket(std::move(copiedPacket));
        output->sendPacket(move(outputPacket));
    }

};

}

#endif //CONDUCTOR_PACKETTYPECONVERTER_H
