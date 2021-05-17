#include "Config.h"

namespace conductor {

std::unique_ptr<Config> Config::instance = nullptr;

void Config::initialize() {
    if (instance == nullptr) {
        instance = std::unique_ptr<Config>(new Config());
    } else {
        throw std::logic_error("Attempted to initialize Config singleton more than once");
    }
}

Config &Config::getInstance() {
    return *instance;
}

Config::Config() {
    auto configFile = YAML::LoadFile(std::string{CONFIG_FILE_PATH});
    loggerName = configFile["general"]["logger_name"].as<std::string>();
    sampleRate = configFile["general"]["sample_rate"].as<float>();
    audioPacketSize = configFile["general"]["audio_packet_size"].as<int>();
    realTimeInput = configFile["general"]["real_time_input"].as<bool>();

    // packetry
    packetConduitCuratorTickInterval = configFile["packet_conduit_curator"]["tick_interval"].as<int>();

    // audio stream
    ringBufferSizeMultiplier = configFile["ring_buffer"]["size_multiplier"].as<int>();

    inputAudioFile = configFile["audio_file_reader"]["audio_input_file"].as<std::string>();

    // pipeline
    parameterEndpoint = configFile["suite"]["parameter_endpoint"].as<std::string>();
    performerOutputEndpoint = configFile["suite"]["performer_output_endpoint"].as<std::string>();
    dataOutputEndpoint = configFile["suite"]["data_output_endpoint"].as<std::string>();
    recordToFiles = configFile["suite"]["record_to_files"].as<bool>();
    sendData = configFile["suite"]["send_data"].as<bool>();

    stftHopSize = configFile["stft"]["hop_size"].as<int>();
    stftWindowSize = configFile["stft"]["window_size"].as<int>();

    outputFilePath = configFile["file_writer"]["output_file_path"].as<std::string>();

    specfluxThreshold = configFile["specflux_onset_processor"]["threshold"].as<float>();
    specfluxPeakPickingWindowSize = configFile["specflux_onset_processor"]["peak_picking_window_size"].as<int>();
    specfluxPeakPickingWindowTailMultiplier = configFile["specflux_onset_processor"]["peak_picking_window_tail_multiplier"].as<int>();
}

}