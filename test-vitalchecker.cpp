#include <tuple>
#include "include/vitalchecker.h"
#include "include/printer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::_;

class MockPrinter : public CriticalMessagePrinter {
 public:
    MOCK_METHOD(void, printMessage, (const std::string&, int, std::function<void(int)>,
                std::ostream&), (const, override));
};

class MockSleep {
 public:
    MOCK_METHOD(void, sleep, (int), ());
};

class PrintMessageTest : public ::testing::TestWithParam<
                            std::tuple<
                                int,
                                std::string>> {
 protected:
    MockSleep mockSleep;

    void mockSleepFunc(int seconds) {
        mockSleep.sleep(seconds);
    }
};

TEST_P(PrintMessageTest, CallsSleepAndOutputsCorrectly) {
    CriticalMessagePrinter printer;
    std::ostringstream stringBuffer;
    int seconds = std::get<0>(GetParam());
    std::string expectedOutput = std::get<1>(GetParam());

    EXPECT_CALL(mockSleep, sleep(1)).Times(seconds);
    printer.printMessage("Test Message", seconds, [this](int s) {
        mockSleepFunc(s);
    }, stringBuffer);
    EXPECT_EQ(stringBuffer.str(), expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(
    PrintMessageTests,
    PrintMessageTest,
    ::testing::Values(
        std::make_tuple(11,
            "Test Message\n\r* \r *\r* \r *\r* \r *\r* \r *\r* \r *\r* "),
        std::make_tuple(12,
            "Test Message\n\r* \r *\r* \r *\r* \r *\r* \r *\r* \r *\r* \r *")
    )
);

class VitalCheckerCriticalTest : public ::testing::TestWithParam<
                                    std::tuple<
                                        std::string,
                                        std::string,
                                        std::string>> {
 protected:
    MockPrinter mockPrinter;
    std::string language;
    std::string vitalName;
    std::string expectedMessage;

    void SetUp() override {
        std::tie(language, vitalName, expectedMessage) = GetParam();
    }
};

TEST_P(VitalCheckerCriticalTest, CriticalMessages) {
    VitalChecker vitalChecker{mockPrinter, language};
    EXPECT_CALL(mockPrinter, printMessage(expectedMessage, 12, _, _));
    EXPECT_FALSE(vitalChecker.vitalInRange(vitalName, 0.0, 95, 102));
}

INSTANTIATE_TEST_SUITE_P(
    CriticalTests,
    VitalCheckerCriticalTest,
    ::testing::Values(
        std::make_tuple("en", "Temperature", "Temperature critical!"),
        std::make_tuple("ger", "Temperature", "Temperatur kritisch!"),
        std::make_tuple("ja", "Temperature", "温度が危険です！"),
        std::make_tuple("en", "Pulse Rate", "Pulse Rate is out of range!"),
        std::make_tuple("ger", "Pulse Rate", "Pulsfrequenz außerhalb des Bereichs!"),
        std::make_tuple("ja", "Pulse Rate", "脈拍が範囲外です！"),
        std::make_tuple("en", "Oxygen Saturation", "Oxygen Saturation out of range!"),
        std::make_tuple("ger", "Oxygen Saturation", "Sauerstoffsättigung außerhalb des Bereichs!"),
        std::make_tuple("ja", "Oxygen Saturation", "酸素飽和度が範囲外です！")
    )
);

class VitalCheckerWarningTest : public ::testing::TestWithParam<
                                    std::tuple<
                                        std::string,
                                        std::string,
                                        float,
                                        float,
                                        float,
                                        std::string>> {
 protected:
    MockPrinter mockPrinter;
    std::string language;
    std::string vitalName;
    float lowerLimit;
    float upperLimit;
    float testValue;
    std::string expectedMessage;

    void SetUp() override {
        std::tie(language,
                 vitalName,
                 lowerLimit,
                 upperLimit,
                 testValue,
                 expectedMessage) = GetParam();
    }
};

TEST_P(VitalCheckerWarningTest, WarningMessages) {
    VitalChecker vitalChecker{mockPrinter, language};
    EXPECT_CALL(mockPrinter, printMessage(expectedMessage, 12, _, _));
    EXPECT_TRUE(vitalChecker.vitalInRange(vitalName, testValue, lowerLimit, upperLimit));
}

// Define test cases for warning messages
INSTANTIATE_TEST_SUITE_P(
    WarningTests,
    VitalCheckerWarningTest,
    ::testing::Values(
        std::make_tuple("en", "Temperature", 95.0, 102.0, 95.0 + 1.53,
                        "Approaching hypothermia"),
        std::make_tuple("ger", "Temperature", 95.0, 102.0, 95.0 + 1.53,
                        "Annäherung an Unterkühlung"),
        std::make_tuple("ja", "Temperature", 95.0, 102.0, 95.0 + 1.53,
                        "低体温症に近づいています"),
        std::make_tuple("en", "Temperature", 95.0, 102.0, 102.0 - 1.53,
                        "Approaching hyperthermia"),
        std::make_tuple("ger", "Temperature", 95.0, 102.0, 102.0 - 1.53,
                        "Annäherung an Überhitzung"),
        std::make_tuple("ja", "Temperature", 95.0, 102.0, 102.0 - 1.53,
                        "高体温症に近づいています"),
        std::make_tuple("en", "Pulse Rate", 60.0, 100.0, 60.0 + 1.5,
                        "Approaching bradycardia"),
        std::make_tuple("ger", "Pulse Rate", 60.0, 100.0, 60.0 + 1.5,
                        "Annäherung an Bradykardie"),
        std::make_tuple("ja", "Pulse Rate", 60.0, 100.0, 60.0 + 1.5,
                        "徐脈に近づいています"),
        std::make_tuple("en", "Pulse Rate", 60.0, 100.0, 100.0 - 1.5,
                        "Approaching tachycardia"),
        std::make_tuple("ger", "Pulse Rate", 60.0, 100.0, 100.0 - 1.5,
                        "Annäherung an Tachykardie"),
        std::make_tuple("ja", "Pulse Rate", 60.0, 100.0, 100.0 - 1.5,
                        "頻脈に近づいています"),
        std::make_tuple("en", "Oxygen Saturation", 90.0, 100.0, 90.0 + 1.5,
                        "Approaching hypoxemia"),
        std::make_tuple("ger", "Oxygen Saturation", 90.0, 100.0, 90.0 + 1.5,
                        "Annäherung an Hypoxämie"),
        std::make_tuple("ja", "Oxygen Saturation", 90.0, 100.0, 90.0 + 1.5,
                        "低酸素血症に近づいています"),
        std::make_tuple("en", "Oxygen Saturation", 90.0, 100.0, 100.0 - 1.5,
                        "Approaching hyperoxia"),
        std::make_tuple("ger", "Oxygen Saturation", 90.0, 100.0, 100.0 - 1.5,
                        "Annäherung an Hyperoxie"),
        std::make_tuple("ja", "Oxygen Saturation", 90.0, 100.0, 100.0 - 1.5,
                        "高酸素症に近づいています")
    )
);

class VitalsOkTest : public ::testing::TestWithParam<
                        std::tuple<
                            std::string,
                            float,
                            float,
                            float,
                            bool>> {
 protected:
    MockPrinter mockPrinter;
    std::string language;
    float temperature;
    float pulseRate;
    float spo2;
    bool expectedResult;

    void SetUp() override {
        std::tie(language,
                 temperature,
                 pulseRate,
                 spo2,
                 expectedResult) = GetParam();
    }
};

TEST_P(VitalsOkTest, VitalsOkFunction) {
    VitalChecker vitalChecker{mockPrinter, language};
    if (expectedResult) {
        EXPECT_CALL(mockPrinter, printMessage(_, _, _, _)).Times(0);
    } else {
        EXPECT_CALL(mockPrinter, printMessage(_, _, _, _)).Times(::testing::AtLeast(1));
    }
    EXPECT_EQ(vitalChecker.vitalsOk(temperature, pulseRate, spo2), expectedResult);
}

INSTANTIATE_TEST_SUITE_P(
    VitalsOkTests,
    VitalsOkTest,
    ::testing::Values(
        // All vitals OK
        std::make_tuple("en", 98.0, 80.0, 95.0, true),
        std::make_tuple("ger", 98.0, 80.0, 95.0, true),
        std::make_tuple("ja", 98.0, 80.0, 95.0, true),
        // Temperature out of range
        std::make_tuple("en", 94.0, 80.0, 95.0, false),
        std::make_tuple("ger", 94.0, 80.0, 95.0, false),
        std::make_tuple("ja", 94.0, 80.0, 95.0, false),
        // Pulse Rate out of range
        std::make_tuple("en", 98.0, 50.0, 95.0, false),
        std::make_tuple("ger", 98.0, 50.0, 95.0, false),
        std::make_tuple("ja", 98.0, 50.0, 95.0, false),
        // Oxygen Saturation out of range
        std::make_tuple("en", 98.0, 80.0, 85.0, false),
        std::make_tuple("ger", 98.0, 80.0, 85.0, false),
        std::make_tuple("ja", 98.0, 80.0, 85.0, false),
        // All vitals out of range
        std::make_tuple("en", 94.0, 50.0, 85.0, false),
        std::make_tuple("ger", 94.0, 50.0, 85.0, false),
        std::make_tuple("ja", 94.0, 50.0, 85.0, false)
    )
);

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
