#include "include/printer.h"
#include <thread>
#include <chrono>

using std::flush;

void CriticalMessagePrinter::printCriticalMessage(
                                const std::string& message,
                                int secondsToShowMessage,
                                std::function<void(int)> sleepFunc,
                                std::ostream& outputBuffer) const {
    outputBuffer << message << "\n";
    auto fullCycle = secondsToShowMessage / 2;
    auto remainingSeconds = secondsToShowMessage % 2;
    for (int i = 0; i < fullCycle; i++) {
        outputBuffer << "\r* " << flush;
        sleepFunc(1);
        outputBuffer << "\r *" << flush;
        sleepFunc(1);
    }
    if (remainingSeconds) {
        outputBuffer << "\r* " << flush;
        sleepFunc(1);
    }
}
