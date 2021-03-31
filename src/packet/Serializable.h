#ifndef CONDUCTOR_SERIALIZABLE_H
#define CONDUCTOR_SERIALIZABLE_H

#include <ImpresarioUtils.h>
#include <ImpresarioSerialization.h>

namespace conductor {

class Serializable : virtual impresarioUtils::NonCopyable {
public:
    virtual ~Serializable() = default;

    virtual std::unique_ptr<flatbuffers::FlatBufferBuilder> serialize() const = 0;

    virtual ImpresarioSerialization::Identifier getIdentifier() const = 0;
};

}

#endif //CONDUCTOR_SERIALIZABLE_H
