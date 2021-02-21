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
    spdlog::get(LOGGER_NAME)->info("onset processor registration count: {}", size(identities));
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
    for (int i = 0; i < onsetProcessorCount; i++) {
        auto envelope = inputSocket->receive();
        envelope->pop();
        envelope->pop();
        auto message = envelope->pop();
        auto onset = GetOnset(message.data());
        if (onset->timestamp() > 0) {
            spdlog::get(LOGGER_NAME)->info("{} {}", EnumNameOnsetMethod(onset->method()), onset->timestamp());
        }
    }
    sendReadySignal();
}

bool OnsetAggregator::shouldContinue() {
    return true;
}
