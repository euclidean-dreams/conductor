#ifndef CONDUCTOR_PACKET_H
#define CONDUCTOR_PACKET_H

#include <IdentifierWrapper_generated.h>
#include <NonCopyable.h>

namespace conductor {

class Packet : impresarioUtils::NonCopyable {
public:
    virtual ~Packet() = default;

    virtual std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const = 0;

    virtual ImpresarioSerialization::Identifier getIdentifier() const = 0;
};

}

#endif //CONDUCTOR_PACKET_H
