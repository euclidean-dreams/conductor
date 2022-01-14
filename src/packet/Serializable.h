#ifndef CONDUCTOR_SERIALIZABLE_H
#define CONDUCTOR_SERIALIZABLE_H

#include <ImpresarioSerialization.h>

namespace conductor {

class Serializable {
public:
    virtual ~Serializable() = default;

    virtual std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const = 0;

    virtual ImpresarioSerialization::Identifier getIdentifier() const = 0;
};

}

#endif //CONDUCTOR_SERIALIZABLE_H
