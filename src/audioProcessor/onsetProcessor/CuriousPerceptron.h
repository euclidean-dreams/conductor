#ifndef CONDUCTOR_CURIOUSPERCEPTRON_H
#define CONDUCTOR_CURIOUSPERCEPTRON_H

#include <list>
#include <cmath>
#include "Config.h"

namespace conductor {

class CuriousPerceptron {
private:
    inline static const float DEFAULT_THRESHOLD = 2;
    inline static const int DEFAULT_MEMORY_LENGTH = 50;

    int memoryLength;
    float threshold;
    std::_List_const_iterator<float> lastActivationSpectralPeak;
    std::_List_const_iterator<float> lastActivationMagnitudePeak;

    bool peakOccurring() const;

    std::_List_const_iterator<float> findLastActivationValley() const;

    std::_List_const_iterator<float> findOnset() const;

public:
    // todo make private!
    std::list<float> magnitudes;
    std::list<float> spectralFluxes;
    std::list<float> fluxyFluxes;

//public:
    explicit CuriousPerceptron();

    void addSample(float magnitude);

    float getPeaks();

    bool fired();
};

}

#endif //CONDUCTOR_CURIOUSPERCEPTRON_H
