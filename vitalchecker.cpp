#include "include/vitalchecker.h"
#include <boost/locale.hpp>

VitalChecker::VitalChecker(const CriticalMessagePrinter& printer, const std::string& language)
    : m_printer(printer), m_language(language) {
    initializeMessages();
}

void VitalChecker::initializeMessages() {
    boost::locale::generator gen;
    gen.add_messages_path("locales");
    gen.add_messages_domain("vitalchecker");
    std::locale::global(gen(m_language + ".UTF-8"));
    std::cout.imbue(std::locale());

    m_criticalMessages = {
        {"Temperature", boost::locale::translate("Temperature critical!")},
        {"Pulse Rate", boost::locale::translate("Pulse Rate is out of range!")},
        {"Oxygen Saturation", boost::locale::translate("Oxygen Saturation out of range!")}
    };

    m_warningMessages = {
        {"Temperature", {boost::locale::translate("Approaching hypothermia"),
                         boost::locale::translate("Approaching hyperthermia")}},
        {"Pulse Rate", {boost::locale::translate("Approaching bradycardia"),
                        boost::locale::translate("Approaching tachycardia")}},
        {"Oxygen Saturation", {boost::locale::translate("Approaching hypoxemia"),
                               boost::locale::translate("Approaching hyperoxia")}}
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
        m_printer.printMessage(warnings.first, 12);
    } else if (vitalValue >= upperWarningLimit) {
        m_printer.printMessage(warnings.second, 12);
    }
}

bool VitalChecker::vitalInRange(const std::string& vitalName, float vitalValue,
                              float vitalLowerLimit, float vitalUpperLimit) const {
    float tolerance = 0.015 * vitalUpperLimit;
    float lowerWarningLimit = vitalLowerLimit + tolerance;
    float upperWarningLimit = vitalUpperLimit - tolerance;

    if (isCritical(vitalValue, vitalLowerLimit, vitalUpperLimit)) {
        m_printer.printMessage(m_criticalMessages.at(vitalName), 12);
        return false;
    }
    checkWarning(vitalValue, lowerWarningLimit, upperWarningLimit,
                 m_warningMessages.at(vitalName));
    return true;
}

bool VitalChecker::vitalsOk(float temperatureInF, float pulseRate, float spo2) const {
    bool temperatureOk = vitalInRange("Temperature", temperatureInF, 95, 102);
    bool pulseOk = vitalInRange("Pulse Rate", pulseRate, 60, 100);
    bool spo2Ok = vitalInRange("Oxygen Saturation", spo2, 90, 100);
    return temperatureOk && pulseOk && spo2Ok;
}
