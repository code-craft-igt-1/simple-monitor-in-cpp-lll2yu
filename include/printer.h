#pragma once

#include <string>
#include <iostream>
#include <functional>
#include <thread>

class CriticalMessagePrinter {
 public:
    virtual ~CriticalMessagePrinter() = default;
    virtual void printMessage(
        const std::string& message,
        int secondsToShowMessage,
        std::function<void(int)> sleepFunc = [](int secondsToSleep) {
             std::this_thread::sleep_for(std::chrono::seconds(secondsToSleep));
             },
        std::ostream& outputBuffer = std::cout) const;
};
