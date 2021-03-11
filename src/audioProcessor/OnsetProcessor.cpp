#include "OnsetProcessor.h"

namespace conductor {

std::unique_ptr<OnsetProcessor> OnsetProcessor::create(zmq::context_t &context, const std::string &parameterEndpoint,
                                                       const std::string &inputEndpoint,
                                                       const std::string &outputEndpoint,
                                                       ImpresarioSerialization::OnsetMethod method) {
    auto input = std::make_unique<impresarioUtils::NetworkSocket>(context, inputEndpoint, zmq::socket_type::sub, false);
    input->setSubscriptionFilter("");
    auto output = std::make_unique<impresarioUtils::NetworkSocket>(context, outputEndpoint, zmq::socket_type::push,
                                                                   false);
    auto parameterSocket = std::make_unique<impresarioUtils::NetworkSocket>(context, parameterEndpoint,
                                                                            zmq::socket_type::sub, false);
    parameterSocket->setSubscriptionFilter("");
    return std::make_unique<OnsetProcessor>(move(input), move(output), move(parameterSocket), method);
}

OnsetProcessor::OnsetProcessor(std::unique_ptr<impresarioUtils::NetworkSocket> inputSocket,
                               std::unique_ptr<impresarioUtils::NetworkSocket> outputSocket,
                               std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                               ImpresarioSerialization::OnsetMethod method)
        : inputSocket{move(inputSocket)},
          outputSocket{move(outputSocket)},
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
    auto message = inputSocket->receive()->pop();
    auto audioPacket = ImpresarioSerialization::GetAudioPacket(message.data());
    auto onsetDelay = determineOnsetDelay(audioPacket->samples());
    if (onsetDelay > 0) {
        auto onsetTimestamp = determineOnsetTimestamp(onsetDelay, audioPacket->timestamp());
        sendOnset(onsetTimestamp);
    }
}

uint64_t OnsetProcessor::determineOnsetDelay(const flatbuffers::Vector<float> *samples) {
    for (int i = 0; i < samples->size(); i++) {
        onsetInput->data[i] = (*samples)[i];
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

void OnsetProcessor::sendOnset(uint64_t onsetTimestamp) {
    flatbuffers::FlatBufferBuilder builder{};
    auto onset = CreateOnset(builder, onsetTimestamp, method);
    builder.Finish(onset);
    zmq::multipart_t message{builder.GetBufferPointer(), builder.GetSize()};
    outputSocket->send(message);
}

bool OnsetProcessor::shouldContinue() {
    return true;
}

}
