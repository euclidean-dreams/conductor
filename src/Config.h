#ifndef CONDUCTOR_CONFIG_H
#define CONDUCTOR_CONFIG_H

#include <map>
#include <memory>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace conductor {

class Config {
private:
    static constexpr std::string_view CONFIG_FILE_PATH = "../config.yml";
    static std::unique_ptr<Config> instance;

    std::string loggerName;
    float sampleRate;
    int audioPacketSize;
    bool realTimeInput;

    // packetry
    int packetConduitCuratorTickInterval;

    // audio stream
    int ringBufferSizeMultiplier;

    std::string inputAudioFile;

    // pipeline
    std::string morselEndpoint;
    std::string matrixPerformerOutputEndpoint;
    std::string bannerPerformerOutputEndpoint;
    int matrixLedCount;
    int bannerLedCount;
    std::string dataOutputEndpoint;
    bool recordToFiles;
    bool sendData;

    int stftHopSize;
    int stftWindowSize;

    std::map<int, float> equalizerScalingFactors;

    std::string outputFilePath;

    float specfluxThreshold;
    int specfluxPeakPickingWindowSize;
    int specfluxPeakPickingWindowTailMultiplier;

    // mel filterbank
    int melFilterBankMaxFrequency;

    Config();

public:
    static Config &getInstance();

    static void initialize();

    std::string getLoggerName() const { return loggerName; }

    float getSampleRate() const { return sampleRate; }

    bool getRealTimeInput() const { return realTimeInput; }

    int getAudioPacketSize() const { return audioPacketSize; }

    int getPacketConduitCuratorTickInterval() const { return packetConduitCuratorTickInterval; }

    int getRingBufferSizeMultiplier() const { return ringBufferSizeMultiplier; }

    std::string getInputAudioFile() const { return inputAudioFile; }

    std::string getMorselEndpoint() const { return morselEndpoint; }

    std::string getMatrixPerformerOutputEndpoint() const { return matrixPerformerOutputEndpoint; }

    std::string getBannerPerformerOutputEndpoint() const { return bannerPerformerOutputEndpoint; }

    int getMatrixLedCount() const { return matrixLedCount; }

    int getBannerLedCount() const { return bannerLedCount; }

    std::string getDataOutputEndpoint() const { return dataOutputEndpoint; }

    bool getRecordToFiles() const { return recordToFiles; }

    bool getSendData() const { return sendData; }

    int getSTFTHopSize() const { return stftHopSize; }

    int getSTFTWindowSize() const { return stftWindowSize; }

    std::map<int, float> getEqualizerScalingValues() const { return equalizerScalingFactors; }

    std::string getOutputFilePath() const { return outputFilePath; }

    float getSpecfluxThreshold() const { return specfluxThreshold; }

    int getSpecfluxPeakPickingWindowSize() const { return specfluxPeakPickingWindowSize; }

    int getSpecfluxPeakPickingWindowTailMultiplier() const { return specfluxPeakPickingWindowTailMultiplier; }

    int getMelFilterBankMaxFrequency() const { return melFilterBankMaxFrequency; }
};

}

#endif //CONDUCTOR_CONFIG_H
