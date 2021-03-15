#include "OnsetProcessor.h"

namespace conductor {

OnsetProcessor::OnsetProcessor(std::unique_ptr<PacketSpout> input,
                               std::shared_ptr<PacketConduit> output,
                               std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                               ImpresarioSerialization::OnsetMethod method)
        : input{move(input)},
          output{move(output)},
          parameterSocket{move(parameterSocket)},
          method{method},
          onsetAlgorithm{
                  new_aubio_onset(EnumNameOnsetMethod(method), PROCESSOR_BUFFER_SIZE, PROCESSOR_HOP_SIZE, SAMPLE_RATE)
          },
          onsetInput{new_fvec(AUDIO_PACKET_SIZE)},
          onsetResultWrapper{new_fvec(1)} {
    aubio_onset_set_threshold(onsetAlgorithm, DEFAULT_ONSET_THRESHOLD);
    aubio_onset_set_minioi_ms(onsetAlgorithm, DEFAULT_ONSET_MINIOI_MS);
    aubio_onset_set_silence(onsetAlgorithm, DEFAULT_ONSET_SILENCE);
    aubio_onset_set_awhitening(onsetAlgorithm, DEFAULT_ONSET_ADAPTIVE_WHITENING);
    aubio_onset_set_delay(onsetAlgorithm, 0);
}

OnsetProcessor::~OnsetProcessor() {
    del_aubio_onset(onsetAlgorithm);
    del_fvec(onsetInput);
    del_fvec(onsetResultWrapper);
}

void OnsetProcessor::updateAlgorithmParameters() {
    auto parameters = parameterSocket->receiveBuffer(zmq::recv_flags::dontwait);
    if (parameters != nullptr) {
        auto onsetParameters = ImpresarioSerialization::GetOnsetProcessorParameters(parameters.get());
        if (onsetParameters->method() == method) {
            aubio_onset_set_threshold(onsetAlgorithm, onsetParameters->threshold());
            aubio_onset_set_minioi_ms(onsetAlgorithm, onsetParameters->minioi_ms());
            aubio_onset_set_silence(onsetAlgorithm, onsetParameters->silence());
            aubio_onset_set_awhitening(onsetAlgorithm, onsetParameters->adaptive_whitening());
        }
    }
}

void OnsetProcessor::setup() {

}

void OnsetProcessor::process() {
    updateAlgorithmParameters();
    auto packets = input->getPackets(1);
    auto onsetDelay = determineOnsetDelay(*packets);
    if (onsetDelay > 0) {
        auto earliestTimestamp = AudioPacket::from(packets->getPacket(0)).getTimestamp();
        auto onsetTimestamp = determineOnsetTimestamp(onsetDelay, earliestTimestamp);
        auto packet = std::make_unique<OnsetPacket>(onsetTimestamp, method, earliestTimestamp);
        output->sendPacket(move(packet));
    }
    packets->concludeUse();
}

uint64_t OnsetProcessor::determineOnsetDelay(PacketCollection &packets) {
    auto indexOffset = 0;
    for (auto &packet: packets) {
        auto &audioPacket = AudioPacket::from(*packet);
        for (int index = indexOffset; index < audioPacket.size() + indexOffset; index++) {
            onsetInput->data[index] = audioPacket.getSample(index);
        }
    }
    aubio_onset_do(onsetAlgorithm, onsetInput, onsetResultWrapper);
    auto onsetDelay = static_cast<uint64_t>(aubio_onset_get_last_ms(onsetAlgorithm) * 1000);
    aubio_onset_reset(onsetAlgorithm);
    return onsetDelay;
}

uint64_t OnsetProcessor::determineOnsetTimestamp(uint64_t onsetDelay, uint64_t audioPacketTimestamp) {
    if (onsetDelay > 0) {
        auto currentTime = impresarioUtils::getCurrentTime();
        auto totalDelay = onsetDelay + (currentTime - audioPacketTimestamp);
        return currentTime - totalDelay;
    } else {
        return 0;
    }
}

bool OnsetProcessor::shouldContinue() {
    return true;
}

}
