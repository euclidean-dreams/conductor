#include "STFTPacket.h"

namespace conductor {

STFTPacket::STFTPacket(uint64_t originTimestamp, ImpresarioSerialization::FrequencyBand frequencyBand, int size)
        : AudioPacket{originTimestamp, frequencyBand},
          real{},
          imaginary{},
          maxSize{size},
          addIndex{0},
          finalized{false} {
    real.reserve(maxSize);
    imaginary.reserve(maxSize);
}

STFTPacket::STFTPacket(const STFTPacket &original)
        : AudioPacket{original.originTimestamp, original.frequencyBand},
          real{},
          imaginary{},
          maxSize{original.maxSize},
          addIndex{original.addIndex},
          finalized{original.finalized} {
    real.reserve(maxSize);
    imaginary.reserve(maxSize);
    for (int i = 0; i < original.maxSize; i++) {
        real.push_back(original.real[i]);
        imaginary.push_back(original.imaginary[i]);
    }
}

void STFTPacket::writeToFile(std::ofstream &fileStream) const {
    auto outputBuffer = std::make_unique<float[]>(maxSize);
    for (int index = 0; index < maxSize; index++) {
        outputBuffer[index] = getMagnitude(index);
    }
    fileStream.write(reinterpret_cast<char *>(outputBuffer.get()), maxSize * sizeof(float));
}

std::unique_ptr<flatbuffers::FlatBufferBuilder> STFTPacket::serialize() const {
    auto builder = std::make_unique<flatbuffers::FlatBufferBuilder>();
    std::vector<float> rawMagnitudes;
    rawMagnitudes.reserve(maxSize);
    for (int index = 0; index < maxSize; index++) {
        rawMagnitudes.push_back(getMagnitude(index));
    }
    auto magnitudes = builder->CreateVector(rawMagnitudes);
    auto serializedSTFT = ImpresarioSerialization::CreateSTFT(*builder, originTimestamp, magnitudes);
    builder->Finish(serializedSTFT);
    return builder;
}

ImpresarioSerialization::Identifier STFTPacket::getIdentifier() const {
    return ImpresarioSerialization::Identifier::stft;
}

void STFTPacket::addSample(float realPart, float imaginaryPart) {
    if (finalized) {
        throw std::runtime_error("attempted to add a sample to a finalized packet");
    }
    real[addIndex] = realPart;
    imaginary[addIndex] = imaginaryPart;
    addIndex++;
    if (addIndex == maxSize) {
        finalized = true;
    }
}

void STFTPacket::validateRetrieve(int index) const {
    if (!finalized) {
        throw std::runtime_error("attempted to use a packet that wasn't finalized");
    }
    if (index < 0 || index >= maxSize) {
        throw std::out_of_range("attempted to access an out of range sample");
    }
}

float STFTPacket::getRealPart(int index) const {
    validateRetrieve(index);
    return real[index];
}

float STFTPacket::getImaginaryPart(int index) const {
    validateRetrieve(index);
    return imaginary[index];
}

float STFTPacket::getMagnitude(int index) const {
    validateRetrieve(index);
    return static_cast<float>(std::sqrt(std::pow(real[index], 2) + std::pow(imaginary[index], 2)));
}

int STFTPacket::size() const {
    return maxSize;
}

}
