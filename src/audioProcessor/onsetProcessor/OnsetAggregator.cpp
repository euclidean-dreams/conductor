#include "OnsetAggregator.h"

std::unique_ptr<OnsetAggregator> OnsetAggregator::create(context_t &context, const string &inputEndpoint,
                                                         const string &outputEndpoint, int onsetProcessorCount) {
    auto onsetAggregatorInput = make_unique<NetworkSocket>(context, inputEndpoint, socket_type::router, true);
    auto onsetAggregatorOutput = make_unique<NetworkSocket>(context, outputEndpoint, socket_type::pub, true);
    return make_unique<OnsetAggregator>(move(onsetAggregatorInput), move(onsetAggregatorOutput), onsetProcessorCount);
}

OnsetAggregator::OnsetAggregator(std::unique_ptr<NetworkSocket> inputSocket,
                                 std::unique_ptr<NetworkSocket> outputSocket, int onsetProcessorCount)
        : inputSocket{move(inputSocket)},
          outputSocket{move(outputSocket)},
          onsetProcessorCount{onsetProcessorCount} {
}

void OnsetAggregator::setup() {
    collectIdentities();
    sendReadySignal();
}

void OnsetAggregator::collectIdentities() {
    for (int i = 0; i < onsetProcessorCount; i++) {
        auto envelope = inputSocket->receive();
        auto identity = envelope->popstr();
        identities.push_back(identity);
    }
    spdlog::get(LOGGER_NAME)->info("onset processor registration count: {}", identities.size());
}

void OnsetAggregator::sendReadySignal() {
    for (const auto &identity: identities) {
        auto readySignal = multipart_t{};
        readySignal.addstr(identity);
        readySignal.addmem(nullptr, 0);
        readySignal.addmem(nullptr, 0);
        inputSocket->send(readySignal);
    }
}

void OnsetAggregator::process() {
    vector<uint64_t> timestamps;
    vector<int8_t> methods;
    bool detectedAtLeastOneOnset = false;
    for (int i = 0; i < onsetProcessorCount; i++) {
        auto detectedOnset = receiveOnset(timestamps, methods);
        if (detectedOnset) {
            detectedAtLeastOneOnset = true;
        }
    }
    if (detectedAtLeastOneOnset) {
        sendOnsetAggregate(timestamps, methods);
    }
    sendReadySignal();
}

bool OnsetAggregator::receiveOnset(vector<uint64_t> &timestamps, vector<int8_t> &methods) {
    auto envelope = inputSocket->receive();
    envelope->pop();
    envelope->pop();
    auto message = envelope->pop();
    auto onset = GetOnset(message.data());
    methods.push_back(static_cast<int8_t>(onset->method()));
    timestamps.push_back(onset->timestamp());
    return onset->timestamp() > 0;
}

void OnsetAggregator::sendOnsetAggregate(const vector<uint64_t> &timestamps, const vector<int8_t> &methods) {
    FlatBufferBuilder builder{ONSET_AGGREGATE_SIZE};
    auto timestampsOffset = builder.CreateVector(timestamps);
    auto methodsOffset = builder.CreateVector(methods);
    auto onsetAggregate = CreateOnsetAggregate(builder, timestampsOffset, methodsOffset);
    builder.Finish(onsetAggregate);
    multipart_t message{builder.GetBufferPointer(), builder.GetSize()};
    outputSocket->send(message);
}

bool OnsetAggregator::shouldContinue() {
    return true;
}
