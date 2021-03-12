#ifndef CONDUCTOR_NOTSERIALIZABLEEXCEPTION_H
#define CONDUCTOR_NOTSERIALIZABLEEXCEPTION_H

namespace conductor {

class NotSerializableException : public std::logic_error {
public:
    NotSerializableException() : logic_error("packet is not serializable") {}
};

}

#endif //CONDUCTOR_NOTSERIALIZABLEEXCEPTION_H
