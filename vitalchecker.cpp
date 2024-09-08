#include "include/vitalchecker.h"

VitalChecker::VitalChecker(const CriticalMessagePrinter& printer, const std::string& language)
    : m_printer(printer), m_language(language) {
    initializeMessages();
}

void VitalChecker::initializeMessages() {
    m_criticalMessages = {
        {"en", {
            {"Temperature", "Temperature critical!"},
            {"Pulse Rate", "Pulse Rate is out of range!"},
            {"Oxygen Saturation", "Oxygen Saturation out of range!"}
        }},
        {"ger", {
            {"Temperature", "Temperatur kritisch!"},
            {"Pulse Rate", "Pulsfrequenz außerhalb des Bereichs!"},
            {"Oxygen Saturation", "Sauerstoffsättigung außerhalb des Bereichs!"}
        }}
    };

    m_warningMessages = {
        {"en", {
            {"Temperature", {"Approaching hypothermia", "Approaching hyperthermia"}},
            {"Pulse Rate", {"Approaching bradycardia", "Approaching tachycardia"}},
            {"Oxygen Saturation", {"Approaching hypoxemia", "Approaching hyperoxia"}}
        }},
        {"ger", {
            {"Temperature", {"Annäherung an Unterkühlung", "Annäherung an Überhitzung"}},
            {"Pulse Rate", {"Annäherung an Bradykardie", "Annäherung an Tachykardie"}},
            {"Oxygen Saturation", {"Annäherung an Hypoxämie", "Annäherung an Hyperoxie"}}
        }}
    };
}

bool VitalChecker::isCritical(float vitalValue,
                              float vitalLowerLimit,
                              float vitalUpperLimit) const {
    return vitalValue < vitalLowerLimit || vitalValue > vitalUpperLimit;
}

void VitalChecker::checkWarning(float vitalValue,
                                float lowerWarningLimit,
                                float upperWarningLimit,
                                const std::pair<std::string, std::string>& warnings) const {
    if (vitalValue <= lowerWarningLimit) {
        m_printer.printCriticalMessage(warnings.first, 12);
    } else if (vitalValue >= upperWarningLimit) {
        m_printer.printCriticalMessage(warnings.second, 12);
    }
}

bool VitalChecker::vitalInRange(const std::string& vitalName, float vitalValue,
                              float vitalLowerLimit, float vitalUpperLimit) const {
    float tolerance = 0.015 * vitalUpperLimit;
    float lowerWarningLimit = vitalLowerLimit + tolerance;
    float upperWarningLimit = vitalUpperLimit - tolerance;

    if (isCritical(vitalValue, vitalLowerLimit, vitalUpperLimit)) {
        m_printer.printCriticalMessage(m_criticalMessages.at(m_language).at(vitalName), 12);
        return false;
    }
    checkWarning(vitalValue, lowerWarningLimit, upperWarningLimit,
                 m_warningMessages.at(m_language).at(vitalName));
    return true;
}

bool VitalChecker::vitalsOk(float temperatureInF, float pulseRate, float spo2) const {
    bool temperatureOk = vitalInRange("Temperature", temperatureInF, 95, 102);
    bool pulseOk = vitalInRange("Pulse Rate", pulseRate, 60, 100);
    bool spo2Ok = vitalInRange("Oxygen Saturation", spo2, 90, 100);
    return temperatureOk && pulseOk && spo2Ok;
}
