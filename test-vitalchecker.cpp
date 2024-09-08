#include "include/vitalchecker.h"
#include "include/printer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;

class MockPrinter : public CriticalMessagePrinter {
 public:
    MOCK_METHOD(void, printCriticalMessage, (const std::string&, int, std::function<void(int)>,
                std::ostream&), (const, override));
};

class MockSleep {
 public:
    MOCK_METHOD(void, sleep, (int), ());
};

class PrintCriticalMessageTest : public ::testing::Test {
 protected:
    MockSleep mockSleep;

    void mockSleepFunc(int seconds) {
        mockSleep.sleep(seconds);
    }
};

TEST_F(PrintCriticalMessageTest, CallsSleepAndOutputsCorrectly) {
  CriticalMessagePrinter printer;
  std::ostringstream stringBuffer;
  EXPECT_CALL(mockSleep, sleep(1)).Times(12);
  printer.printCriticalMessage("Test Message", 12, [this](int seconds) {
                                mockSleepFunc(seconds); }, stringBuffer);
  EXPECT_EQ(stringBuffer.str(),
            "Test Message\n\r* \r *\r* \r *\r* \r *\r* \r *\r* \r *\r* \r *");
}

class VitalCheckerTest : public ::testing::Test {
 protected:
    MockPrinter mockPrinter;
    VitalChecker vitalChecker{mockPrinter, "en"};
};

class VitalCheckerTestGerman : public ::testing::Test {
 protected:
    MockPrinter mockPrinter;
    VitalChecker vitalChecker{mockPrinter, "ger"};
};

TEST_F(VitalCheckerTest, TemperatureBelowRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Temperature critical!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Temperature", 94.0, 95, 102));
}

TEST_F(VitalCheckerTest, TemperatureAboveRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Temperature critical!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Temperature", 103.0, 95, 102));
}

TEST_F(VitalCheckerTest, PulseRateBelowRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Pulse Rate is out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Pulse Rate", 50.0, 60, 100));
}

TEST_F(VitalCheckerTest, PulseRateAboveRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Pulse Rate is out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Pulse Rate", 110.0, 60, 100));
}

TEST_F(VitalCheckerTest, OxygenSaturationBelowRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Oxygen Saturation out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Oxygen Saturation", 85.0, 90, 100));
}

TEST_F(VitalCheckerTest, OxygenSaturationAboveRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Oxygen Saturation out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Oxygen Saturation", 105.0, 90, 100));
}

TEST_F(VitalCheckerTest, VitalsOk) {
    EXPECT_CALL(mockPrinter, printCriticalMessage(_, _, _, _)).Times(0);
    EXPECT_TRUE(vitalChecker.vitalsOk(98.0, 80.0, 95.0));
}

TEST_F(VitalCheckerTest, VitalsNotOk) {
    EXPECT_CALL(mockPrinter, printCriticalMessage(_, _, _, _)).Times(3);
    EXPECT_FALSE(vitalChecker.vitalsOk(94.0, 50.0, 85.0));
}

TEST_F(VitalCheckerTest, VitalsNotOkTemperature) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Temperature critical!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalsOk(94.0, 80.0, 95.0));
}

TEST_F(VitalCheckerTest, VitalsNotOkPulseRateAndOxygenSaturation) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Pulse Rate is out of range!", 12, _, _));
    EXPECT_CALL(mockPrinter, printCriticalMessage("Oxygen Saturation out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalsOk(98.0, 50.0, 85.0));
}

TEST_F(VitalCheckerTest, TemperatureLowerWarningBoundary) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Approaching hypothermia", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Temperature", 96.53, 95, 102));
}

TEST_F(VitalCheckerTest, TemperatureUpperWarningBoundary) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Approaching hyperthermia", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Temperature", 100.47, 95, 102));
}

TEST_F(VitalCheckerTest, PulseRateLowerWarningBoundary) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Approaching bradycardia", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Pulse Rate", 60.0 + 1.5, 60, 100));
}

TEST_F(VitalCheckerTest, PulseRateUpperWarningBoundary) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Approaching tachycardia", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Pulse Rate", 100.0 - 1.5, 60, 100));
}

TEST_F(VitalCheckerTest, OxygenSaturationLowerWarningBoundary) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Approaching hypoxemia", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Oxygen Saturation", 90.0 + 1.5, 90, 100));
}

TEST_F(VitalCheckerTest, OxygenSaturationUpperWarningBoundary) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Approaching hyperoxia", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Oxygen Saturation", 100.0 - 1.5, 90, 100));
}

TEST_F(VitalCheckerTestGerman, TemperatureBelowRangeGerman) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Temperatur kritisch!", 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange("Temperature", 94.0, 95, 102));
}

TEST_F(VitalCheckerTestGerman, TemperatureLowerWarningBoundaryGerman) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Annäherung an Unterkühlung", 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange("Temperature", 95.0 + 1.53, 95, 102));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
