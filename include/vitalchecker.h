#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include "./printer.h"

class VitalChecker {
 public:
    explicit VitalChecker(const CriticalMessagePrinter& printer,
                          const std::string& language = "en");

    bool vitalInRange(const std::string& vitalName, float vitalValue,
                    float vitalLowerLimit, float vitalUpperLimit) const;

    bool vitalsOk(float temperatureInF, float pulseRate, float spo2) const;

 private:
    const CriticalMessagePrinter& m_printer;
    std::string m_language;
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::string>> m_criticalMessages;
    std::unordered_map<std::string,
                       std::unordered_map<std::string,
                                          std::pair<std::string, std::string>>> m_warningMessages;

 private:
    void initializeMessages();
    bool isCritical(float vitalValue, float vitalLowerLimit, float vitalUpperLimit) const;
    void checkWarning(float vitalValue, float lowerWarningLimit, float upperWarningLimit,
                      const std::pair<std::string, std::string>& warnings) const;
};
