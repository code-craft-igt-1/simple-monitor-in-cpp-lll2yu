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
    VitalChecker vitalChecker{mockPrinter};
};

TEST_F(VitalCheckerTest, TemperatureBelowRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Temperature critical!", 12, _, _));
    EXPECT_FALSE(vitalChecker.checkVital("Temperature", 94.0, 95, 102));
}

TEST_F(VitalCheckerTest, TemperatureAboveRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Temperature critical!", 12, _, _));
    EXPECT_FALSE(vitalChecker.checkVital("Temperature", 103.0, 95, 102));
}

TEST_F(VitalCheckerTest, PulseRateBelowRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Pulse Rate is out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.checkVital("Pulse Rate", 50.0, 60, 100));
}

TEST_F(VitalCheckerTest, PulseRateAboveRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Pulse Rate is out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.checkVital("Pulse Rate", 110.0, 60, 100));
}

TEST_F(VitalCheckerTest, OxygenSaturationBelowRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Oxygen Saturation out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.checkVital("Oxygen Saturation", 85.0, 90, 100));
}

TEST_F(VitalCheckerTest, OxygenSaturationAboveRange) {
    EXPECT_CALL(mockPrinter, printCriticalMessage("Oxygen Saturation out of range!", 12, _, _));
    EXPECT_FALSE(vitalChecker.checkVital("Oxygen Saturation", 105.0, 90, 100));
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
