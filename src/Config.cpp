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
    audioDevice = configFile["general"]["audio_device"].as<int>();
    sampleRate = configFile["general"]["sample_rate"].as<float>();
    audioPacketSize = configFile["general"]["audio_packet_size"].as<int>();
    realTimeInput = configFile["general"]["real_time_input"].as<bool>();

    // packetry
    packetConduitCuratorTickInterval = configFile["packet_conduit_curator"]["tick_interval"].as<int>();

    // audio stream
    ringBufferSizeMultiplier = configFile["ring_buffer"]["size_multiplier"].as<int>();

    inputAudioFile = configFile["audio_file_reader"]["audio_input_file"].as<std::string>();

    // pipeline
    morselEndpoint = configFile["suite"]["morsel_endpoint"].as<std::string>();
    matrixPerformerOutputEndpoint = configFile["suite"]["matrix_performer_output_endpoint"].as<std::string>();
    bannerPerformerOutputEndpoint = configFile["suite"]["banner_performer_output_endpoint"].as<std::string>();
    matrixLedCount = configFile["suite"]["matrix_led_count"].as<int>();
    bannerLedCount = configFile["suite"]["banner_led_count"].as<int>();
    dataOutputEndpoint = configFile["suite"]["data_output_endpoint"].as<std::string>();
    recordToFiles = configFile["suite"]["record_to_files"].as<bool>();
    sendData = configFile["suite"]["send_data"].as<bool>();

    stftHopSize = configFile["stft"]["hop_size"].as<int>();
    stftWindowSize = configFile["stft"]["window_size"].as<int>();

    for (auto scalingFactor: configFile["equalizer"]["default_scaling_factors"]) {
        auto frequency = scalingFactor.first.as<int>();
        auto scale = scalingFactor.second.as<float>();
        equalizerScalingFactors.emplace(frequency, scale);
    }

    outputFilePath = configFile["file_writer"]["output_file_path"].as<std::string>();

    specfluxThreshold = configFile["specflux_onset_processor"]["threshold"].as<float>();
    specfluxPeakPickingWindowSize = configFile["specflux_onset_processor"]["peak_picking_window_size"].as<int>();
    specfluxPeakPickingWindowTailMultiplier = configFile["specflux_onset_processor"]["peak_picking_window_tail_multiplier"].as<int>();

    // mel filter bank
    melFilterBankMaxFrequency = configFile["mel_filterbank"]["max_frequency"].as<int>();
}

}
