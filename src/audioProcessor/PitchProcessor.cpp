#include "PitchProcessor.h"

namespace conductor {

PitchProcessor::PitchProcessor(std::unique_ptr<PacketSpout> input, std::shared_ptr<PacketConduit> output,
                               std::unique_ptr<impresarioUtils::NetworkSocket> parameterSocket,
                               ImpresarioSerialization::PitchMethod method)
        : input{move(input)},
          output{move(output)},
          parameterSocket{move(parameterSocket)},
          method{method},
          pitchAlgorithm{
                  new_aubio_pitch(ImpresarioSerialization::EnumNamePitchMethod(method),
                                  PROCESSOR_BUFFER_SIZE, PROCESSOR_HOP_SIZE, SAMPLE_RATE)
          },
          pitchInput{new_fvec(AUDIO_PACKET_SIZE)},
          pitchResultWrapper{new_fvec(1)},
          pitchThreshold{DEFAULT_THRESHOLD} {
    aubio_pitch_set_silence(pitchAlgorithm, DEFAULT_SILENCE);
    aubio_pitch_set_unit(pitchAlgorithm, static_cast<std::string>(PITCH_UNIT).c_str());
}

PitchProcessor::~PitchProcessor() {
    del_aubio_pitch(pitchAlgorithm);
    del_fvec(pitchInput);
    del_fvec(pitchResultWrapper);
}

void PitchProcessor::updateAlgorithmParameters() {
    auto parameters = parameterSocket->receiveSerializedData(zmq::recv_flags::dontwait);
    if (parameters != nullptr) {
        auto pitchParameters = ImpresarioSerialization::GetPitchProcessorParameters(parameters->getBuffer());
        if (pitchParameters->method() == method) {
            aubio_pitch_set_silence(pitchAlgorithm, pitchParameters->silence());
            pitchThreshold = pitchParameters->threshold();
        }
    }
}

void PitchProcessor::setup() {

}

void PitchProcessor::process() {
    updateAlgorithmParameters();
    auto packets = input->getPackets(1);
    auto indexOffset = 0;
    for (auto &packet: *packets) {
        auto &rawAudioPacket = RawAudioPacket::from(*packet);
        for (int index = indexOffset; index < rawAudioPacket.size() + indexOffset; index++) {
            pitchInput->data[index] = rawAudioPacket.getSample(index);
        }
    }
    aubio_pitch_do(pitchAlgorithm, pitchInput, pitchResultWrapper);
    auto confidence = aubio_pitch_get_confidence(pitchAlgorithm);
    float pitch = fvec_get_sample(pitchResultWrapper, 0);
    if (pitch > 0) {
        auto &firstPacket = RawAudioPacket::from(packets->getPacket(0));
        auto earliestTimestamp = firstPacket.getSampleTimestamp();
        auto frequencyBand = firstPacket.getFrequencyBand();
        auto resultPacket = std::make_unique<PitchPacket>(earliestTimestamp, frequencyBand, method, pitch, confidence);
        output->sendPacket(move(resultPacket));
    }
    packets->concludeUse();
}

bool PitchProcessor::shouldContinue() {
    return true;
}

}
