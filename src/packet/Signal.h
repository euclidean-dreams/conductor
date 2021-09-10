#ifndef CONDUCTOR_SIGNAL_H
#define CONDUCTOR_SIGNAL_H

#include "ImpresarioSerialization.h"

namespace conductor {

template<class T>
class Signal {
private:
    std::vector<T> samples;
    int maxSize;

public:
    explicit Signal(int size)
            : samples{},
              maxSize{size} {
        samples.reserve(maxSize);
    }

    void addSample(T sample) {
        if (samples.size() == maxSize) {
            throw std::runtime_error("attempted to add a sample to a finalized signal");
        }
        samples.push_back(sample);
    }

    T getSample(int index) const {
        if (samples.size() < maxSize) {
            throw std::runtime_error("attempted to use a signal that wasn't finalized");
        }
        if (index < 0 || index >= maxSize) {
            throw std::out_of_range("attempted to access an out of range sample on a signal");
        }
        return samples[index];
    }

    int size() const {
        return maxSize;
    }

    const std::vector<T> &getVectorReference() const {
        return samples;
    }
};

}

#endif //CONDUCTOR_SIGNAL_H
