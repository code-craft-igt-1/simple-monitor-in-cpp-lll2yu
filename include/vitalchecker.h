#pragma once

#include <string>
#include <unordered_map>
#include "./printer.h"

class VitalChecker {
 public:
    explicit VitalChecker(const CriticalMessagePrinter& printer) : m_printer(printer) {}

    bool checkVital(const std::string& vitalName, float vitalValue,
                    float vitalLowerLimit, float vitalUpperLimit) const;

    bool vitalsOk(float temperatureInF, float pulseRate, float spo2) const;

 private:
    const CriticalMessagePrinter& m_printer;
};
