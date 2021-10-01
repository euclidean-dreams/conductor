#include "PythonInterpreterManager.h"

namespace conductor {

PythonInterpreterManager::PythonInterpreterManager()
        : mutex{},
          pythonInterpreterGuard{} {

}

std::unique_ptr<std::lock_guard<std::mutex>> PythonInterpreterManager::getLock() {
    return std::make_unique<std::lock_guard<std::mutex>>(mutex);
}

}
