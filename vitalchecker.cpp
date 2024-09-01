#include "include/vitalchecker.h"

bool VitalChecker::checkVital(const std::string& vitalName, float vitalValue,
                              float vitalLowerLimit, float vitalUpperLimit) const {
    if (vitalValue < vitalLowerLimit || vitalValue > vitalUpperLimit) {
        std::unordered_map<std::string, std::string> messages = {
            {"Temperature", "Temperature critical!"},
            {"Pulse Rate", "Pulse Rate is out of range!"},
            {"Oxygen Saturation", "Oxygen Saturation out of range!"}
        };

        m_printer.printCriticalMessage(messages.at(vitalName), 12);
        return false;
    }
    return true;
}

bool VitalChecker::vitalsOk(float temperatureInF, float pulseRate, float spo2) const {
    bool temperatureOk = checkVital("Temperature", temperatureInF, 95, 102);
    bool pulseOk = checkVital("Pulse Rate", pulseRate, 60, 100);
    bool spo2Ok = checkVital("Oxygen Saturation", spo2, 90, 100);
    return temperatureOk && pulseOk && spo2Ok;
}
