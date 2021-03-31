#ifndef CONDUCTOR_PYTHONINTERPRETERMANAGER_H
#define CONDUCTOR_PYTHONINTERPRETERMANAGER_H

#include <mutex>
#include <pybind11/embed.h>
#include <ImpresarioUtils.h>

namespace conductor {

class PythonInterpreterManager : virtual impresarioUtils::NonCopyable {
private:
    std::mutex mutex;
    pybind11::scoped_interpreter pythonInterpreterGuard;

public:
    PythonInterpreterManager();

    std::unique_ptr<std::lock_guard<std::mutex>> getLock();
};

}

#endif //CONDUCTOR_PYTHONINTERPRETERMANAGER_H
