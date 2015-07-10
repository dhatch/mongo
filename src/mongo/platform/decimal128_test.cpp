/*    Copyright 2014 MongoDB Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects
 *    for all of the code used other than as permitted herein. If you modify
 *    file(s) with this exception, you may extend this exception to your
 *    version of the file(s), but you are not obligated to do so. If you do not
 *    wish to do so, delete this exception statement from your version. If you
 *    delete this exception statement from all source files in the program,
 *    then also delete it in the license file.
 */

#include "mongo/platform/decimal128.h"

#include <array>
#include <cmath>
#include <string>
#include <utility>

#include "mongo/config.h"
#include "mongo/stdx/memory.h"
#include "mongo/unittest/unittest.h"

namespace mongo {

// Tests for Decimal128 constructors
TEST(Decimal128Test, TestInt32ConstructorZero) {
    int32_t intZero = 0;
    Decimal128 d(intZero);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x3040000000000000 0000000000000000 = +0E+0
    uint64_t highBytes = 0x3040000000000000ull;
    uint64_t lowBytes = 0x0000000000000000ull;
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestInt32ConstructorMax) {
    int32_t intMax = 2147483647;
    Decimal128 d(intMax);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x3040000000000000 000000007fffffff = +2147483647E+0
    uint64_t highBytes = 0x3040000000000000ull;
    uint64_t lowBytes = 0x000000007fffffffull;
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestInt32ConstructorMin) {
    int32_t intMin = -2147483648;
    Decimal128 d(intMin);
    Decimal128::Decimal128Value val = d.getValue();
    // 0xb040000000000000 000000007fffffff = -2147483648E+0
    uint64_t highBytes = 0xb040000000000000ull;
    uint64_t lowBytes = 0x0000000080000000ull;
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestInt64ConstructorZero) {
    int64_t longZero = 0;
    Decimal128 d(longZero);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x3040000000000000 0000000000000000 = +0E+0
    uint64_t highBytes = 0x3040000000000000ull;
    uint64_t lowBytes = 0x0000000000000000ull;
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestInt64ConstructorMax) {
    int64_t longMax = LONG_MAX;
    Decimal128 d(longMax);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x3040000000000000 7fffffffffffffff = +9223372036854775807E+0
    uint64_t highBytes = 0x3040000000000000ull;
    uint64_t lowBytes = 0x7fffffffffffffffull;
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestInt64ConstructorMin) {
    int64_t longMin = LONG_MIN;
    Decimal128 d(longMin);
    Decimal128::Decimal128Value val = d.getValue();
    // 0xb040000000000000 8000000000000000 = -9223372036854775808E+0
    uint64_t highBytes = std::stoull("b040000000000000", nullptr, 16);
    uint64_t lowBytes = std::stoull("8000000000000000", nullptr, 16);
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestDoubleConstructorQuant1) {
    double dbl = 0.1 / 10;
    Decimal128 d(dbl);
    Decimal128 e("0.01");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuant2) {
    double dbl = 0.1 / 10000;
    Decimal128 d(dbl);
    Decimal128 e("0.00001");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuant3) {
    double dbl = 0.1 / 1000 / 1000 / 1000 / 1000 / 1000 / 1000;
    Decimal128 d(dbl);
    Decimal128 e("1E-19");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuant4) {
    double dbl = 0.01 * 1000 * 1000 * 1000 * 1000 * 1000 * 1000;
    Decimal128 d(dbl);
    Decimal128 e("100000000000000E+2");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuant5) {
    double dbl = 0.0127;
    Decimal128 d(dbl);
    Decimal128 e("0.0127");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuant6) {
    double dbl = 1234567890.12709;
    Decimal128 d(dbl);
    Decimal128 e("1234567890.12709");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuant7) {
    double dbl = 0.1129857 / 1000 / 1000 / 1000 / 1000 / 1000 / 1000;
    Decimal128 d(dbl);
    Decimal128 e("1.12985700000000E-19");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorQuantFailPoorLog10Of2Estimate) {
    double dbl = exp2(1000);
    Decimal128 d(dbl);
    Decimal128 e("1.07150860718627E301");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorZero) {
    double doubleZero = 0;
    Decimal128 d(doubleZero);
    Decimal128 e("0");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorNeg) {
    double doubleNeg = -1.0;
    Decimal128 d(doubleNeg);
    Decimal128 e("-1.0");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorMaxRoundDown) {
    double doubleMax = DBL_MAX;
    Decimal128 d(doubleMax, Decimal128::RoundingMode::kRoundTowardNegative);
    Decimal128 e("179769313486231E294");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorMaxRoundUp) {
    double doubleMax = DBL_MAX;
    Decimal128 d(doubleMax, Decimal128::RoundingMode::kRoundTowardPositive);
    Decimal128 e("179769313486232E294");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorMaxNeg) {
    double doubleMax = -1 * DBL_MAX;
    Decimal128 d(doubleMax);
    Decimal128 e("-179769313486232E294");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorMin) {
    double min = DBL_MIN;
    Decimal128 d(min);
    Decimal128 e("2.22507385850720E-308");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorMinNeg) {
    double min = -DBL_MIN;
    Decimal128 d(min);
    Decimal128 e("-2.22507385850720E-308");
    ASSERT_TRUE(d.isEqual(e));
}

TEST(Decimal128Test, TestDoubleConstructorInfinity) {
    double dbl = std::numeric_limits<double>::infinity();
    Decimal128 d(dbl);
    ASSERT_TRUE(d.isInfinite());
}

TEST(Decimal128Test, TestDoubleConstructorNaN) {
    double dbl = std::numeric_limits<double>::quiet_NaN();
    Decimal128 d(dbl);
    ASSERT_TRUE(d.isNaN());
}

TEST(Decimal128Test, TestStringConstructorInRange) {
    std::string s = "+2.010";
    Decimal128 d(s);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x303a000000000000 00000000000007da = +2.010
    uint64_t highBytes = std::stoull("303a000000000000", nullptr, 16);
    uint64_t lowBytes = std::stoull("00000000000007da", nullptr, 16);
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestStringConstructorPosInfinity) {
    std::string s = "+INFINITY";
    Decimal128 d(s);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x7800000000000000 0000000000000000 = +Inf
    uint64_t highBytes = std::stoull("7800000000000000", nullptr, 16);
    uint64_t lowBytes = std::stoull("0000000000000000", nullptr, 16);
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestStringConstructorNegInfinity) {
    std::string s = "-INFINITY";
    Decimal128 d(s);
    Decimal128::Decimal128Value val = d.getValue();
    // 0xf800000000000000 0000000000000000 = -Inf
    uint64_t highBytes = std::stoull("f800000000000000", nullptr, 16);
    uint64_t lowBytes = std::stoull("0000000000000000", nullptr, 16);
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

TEST(Decimal128Test, TestStringConstructorNaN) {
    std::string s = "I am not a number!";
    Decimal128 d(s);
    Decimal128::Decimal128Value val = d.getValue();
    // 0x7c00000000000000 0000000000000000 = NaN
    uint64_t highBytes = std::stoull("7c00000000000000", nullptr, 16);
    uint64_t lowBytes = std::stoull("0000000000000000", nullptr, 16);
    ASSERT_EQUALS(val.high64, highBytes);
    ASSERT_EQUALS(val.low64, lowBytes);
}

// Tests for absolute value function
TEST(Decimal128Test, TestAbsValuePos) {
    Decimal128 d(25);
    Decimal128 dAbs = d.toAbs();
    ASSERT_TRUE(dAbs.isEqual(d));
}

TEST(Decimal128Test, TestAbsValueNeg) {
    Decimal128 d(-25);
    Decimal128 dAbs = d.toAbs();
    ASSERT_TRUE(dAbs.isEqual(Decimal128(25)));
}


// Tests for Decimal128 conversions
TEST(Decimal128Test, TestDecimal128ToInt32Even) {
    std::string in[6] = {"-2.7", "-2.5", "-2.2", "2.2", "2.5", "2.7"};
    int32_t out[6] = {-3, -2, -2, 2, 2, 3};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toInt(), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt32Neg) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTowardNegative;
    std::string in[6] = {"-2.7", "-2.5", "-2.2", "2.2", "2.5", "2.7"};
    int32_t out[6] = {-3, -3, -3, 2, 2, 2};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toInt(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt32Pos) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTowardPositive;
    std::string in[6] = {"-2.7", "-2.5", "-2.2", "2.2", "2.5", "2.7"};
    int32_t out[6] = {-2, -2, -2, 3, 3, 3};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toInt(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt32Zero) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTowardZero;
    std::string in[6] = {"-2.7", "-2.5", "-2.2", "2.2", "2.5", "2.7"};
    int32_t out[6] = {-2, -2, -2, 2, 2, 2};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toInt(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt32Away) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToAway;
    std::string in[6] = {"-2.7", "-2.5", "-2.2", "2.2", "2.5", "2.7"};
    int32_t out[6] = {-3, -3, -2, 2, 3, 3};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toInt(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt64Even) {
    std::string in[6] = {"-4294967296.7",
                         "-4294967296.5",
                         "-4294967296.2",
                         "4294967296.2",
                         "4294967296.5",
                         "4294967296.7"};
    int64_t out[6] = {-4294967297, -4294967296, -4294967296, 4294967296, 4294967296, 4294967297};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toLong(), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt64Neg) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTowardNegative;
    std::string in[6] = {"-4294967296.7",
                         "-4294967296.5",
                         "-4294967296.2",
                         "4294967296.2",
                         "4294967296.5",
                         "4294967296.7"};
    int64_t out[6] = {-4294967297, -4294967297, -4294967297, 4294967296, 4294967296, 4294967296};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toLong(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt64Pos) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTowardPositive;
    std::string in[6] = {"-4294967296.7",
                         "-4294967296.5",
                         "-4294967296.2",
                         "4294967296.2",
                         "4294967296.5",
                         "4294967296.7"};
    int64_t out[6] = {-4294967296, -4294967296, -4294967296, 4294967297, 4294967297, 4294967297};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toLong(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt64Zero) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTowardZero;
    std::string in[6] = {"-4294967296.7",
                         "-4294967296.5",
                         "-4294967296.2",
                         "4294967296.2",
                         "4294967296.5",
                         "4294967296.7"};
    int64_t out[6] = {-4294967296, -4294967296, -4294967296, 4294967296, 4294967296, 4294967296};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toLong(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToInt64Away) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToAway;
    std::string in[6] = {"-4294967296.7",
                         "-4294967296.5",
                         "-4294967296.2",
                         "4294967296.2",
                         "4294967296.5",
                         "4294967296.7"};
    int64_t out[6] = {-4294967297, -4294967297, -4294967296, 4294967296, 4294967297, 4294967297};
    std::unique_ptr<Decimal128> decPtr;
    for (int testNo = 0; testNo < 6; ++testNo) {
        decPtr = stdx::make_unique<Decimal128>(in[testNo]);
        ASSERT_EQUALS(decPtr->toLong(roundMode), out[testNo]);
    }
}

TEST(Decimal128Test, TestDecimal128ToDoubleNormal) {
    std::string s = "+2.015";
    Decimal128 d(s);
    double result = d.toDouble();
    ASSERT_EQUALS(result, 2.015);
}

TEST(Decimal128Test, TestDecimal128ToDoubleZero) {
    std::string s = "+0.000";
    Decimal128 d(s);
    double result = d.toDouble();
    ASSERT_EQUALS(result, 0.0);
}

TEST(Decimal128Test, TestDecimal128ToString) {
    std::string s = "-2087.015E+281";
    Decimal128 d(s);
    std::string result = d.toString();
    ASSERT_EQUALS(result, "-2087015E+278");
}

TEST(Decimal128Test, TestDecimal128IsAndToIntWithInt) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToEven;
    std::string s = "2";
    Decimal128 d(s);
    std::pair<int32_t, bool> result;
    result = d.isAndToInt(roundMode);
    ASSERT_EQUALS(result.first, 2);
    ASSERT_EQUALS(result.second, true);
}

TEST(Decimal128Test, TestDecimal128IsAndToIntWithNonInt) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToEven;
    std::string s = "2.6";
    Decimal128 d(s);
    std::pair<int32_t, bool> result;
    result = d.isAndToInt(roundMode);
    ASSERT_EQUALS(result.first, 3);
    ASSERT_EQUALS(result.second, false);
}

TEST(Decimal128Test, TestDecimal128IsAndToLongWithLong) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToEven;
    std::string s = "1125899906842624";
    Decimal128 d(s);
    std::pair<int64_t, bool> result;
    result = d.isAndToLong(roundMode);
    ASSERT_EQUALS(result.first, 1125899906842624);
    ASSERT_EQUALS(result.second, true);
}

TEST(Decimal128Test, TestDecimal128IsAndToLongWithNonLong) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToEven;
    std::string s = "1125899906842624.8";
    Decimal128 d(s);
    std::pair<int64_t, bool> result;
    result = d.isAndToLong(roundMode);
    ASSERT_EQUALS(result.first, 1125899906842625);
    ASSERT_EQUALS(result.second, false);
}

TEST(Decimal128Test, TestDecimal128IsAndToDoubleWithDouble) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToEven;
    std::string s = "0.125";
    Decimal128 d(s);
    std::pair<double, bool> result;
    result = d.isAndToDouble(roundMode);
    ASSERT_EQUALS(result.first, 0.125);
    ASSERT_EQUALS(result.second, true);
}

TEST(Decimal128Test, TestDecimal128IsAndToDoubleWithNonDouble) {
    Decimal128::RoundingMode roundMode = Decimal128::RoundingMode::kRoundTiesToEven;
    std::string s = "0.1";
    Decimal128 d(s);
    std::pair<double, bool> result;
    result = d.isAndToDouble(roundMode);
    ASSERT_EQUALS(result.first, 0.1);
    ASSERT_EQUALS(result.second, false);
}

TEST(Decimal128Test, TestDecimal128IsZero) {
    Decimal128 d1(0);
    Decimal128 d2(500);
    ASSERT_TRUE(d1.isZero());
    ASSERT_FALSE(d2.isZero());
}

TEST(Decimal128Test, TestDecimal128IsNaN) {
    Decimal128 d1("NaN");
    Decimal128 d2("10.5");
    Decimal128 d3("Inf");
    ASSERT_TRUE(d1.isNaN());
    ASSERT_FALSE(d2.isNaN());
    ASSERT_FALSE(d3.isNaN());
}

TEST(Decimal128Test, TestDecimal128IsInfinite) {
    Decimal128 d1("NaN");
    Decimal128 d2("10.5");
    Decimal128 d3("Inf");
    Decimal128 d4("-Inf");
    ASSERT_FALSE(d1.isInfinite());
    ASSERT_FALSE(d2.isInfinite());
    ASSERT_TRUE(d3.isInfinite());
    ASSERT_TRUE(d4.isInfinite());
}

TEST(Decimal128Test, TestDecimal128IsNegative) {
    Decimal128 d1("NaN");
    Decimal128 d2("-NaN");
    Decimal128 d3("10.5");
    Decimal128 d4("-10.5");
    Decimal128 d5("Inf");
    Decimal128 d6("-Inf");
    ASSERT_FALSE(d1.isNegative());
    ASSERT_FALSE(d3.isNegative());
    ASSERT_FALSE(d5.isNegative());
    ASSERT_TRUE(d2.isNegative());
    ASSERT_TRUE(d4.isNegative());
    ASSERT_TRUE(d6.isNegative());
}

// Tests for Decimal128 math operations
TEST(Decimal128Test, TestDecimal128AdditionCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-50.5218E19");
    Decimal128 result = d1.add(d2);
    Decimal128 expected("1.999782E21");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128AdditionCase2) {
    Decimal128 d1("1.00");
    Decimal128 d2("2.000");
    Decimal128 result = d1.add(d2);
    Decimal128 expected("3.000");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128SubtractionCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-50.5218E19");
    Decimal128 result = d1.subtract(d2);
    Decimal128 expected("3.010218E21");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128SubtractionCase2) {
    Decimal128 d1("1.00");
    Decimal128 d2("2.000");
    Decimal128 result = d1.subtract(d2);
    Decimal128 expected("-1.000");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128MultiplicationCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-50.5218E19");
    Decimal128 result = d1.multiply(d2);
    Decimal128 expected("-1.265571090E42");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128MultiplicationCase2) {
    Decimal128 d1("1.00");
    Decimal128 d2("2.000");
    Decimal128 result = d1.multiply(d2);
    Decimal128 expected("2.00000");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128DivisionCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-50.5218E19");
    Decimal128 result = d1.divide(d2);
    Decimal128 expected("-4.958255644098191275845278671781290");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128DivisionCase2) {
    Decimal128 d1("1.00");
    Decimal128 d2("2.000");
    Decimal128 result = d1.divide(d2);
    Decimal128 expected("0.5");
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

TEST(Decimal128Test, TestDecimal128Quantizer) {
    Decimal128 expected("1.00001");
    Decimal128 val("1.000008");
    Decimal128 ref("0.00001");
    Decimal128 result = val.quantize(ref);
    ASSERT_EQUALS(result.getValue().low64, expected.getValue().low64);
    ASSERT_EQUALS(result.getValue().high64, expected.getValue().high64);
}

// Tests for Decimal128 comparison operations
TEST(Decimal128Test, TestDecimal128EqualCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("25.05E20");
    bool result = d1.isEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128EqualCase2) {
    Decimal128 d1("1.00");
    Decimal128 d2("1.000000000");
    bool result = d1.isEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128EqualCase3) {
    Decimal128 d1("0.1");
    Decimal128 d2("0.100000000000000005");
    bool result = d1.isEqual(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128NotEqualCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("25.06E20");
    bool result = d1.isNotEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128NotEqualCase2) {
    Decimal128 d1("-25.0001E20");
    Decimal128 d2("-25.00010E20");
    bool result = d1.isNotEqual(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128GreaterCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-25.05E20");
    bool result = d1.isGreater(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128GreaterCase2) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("25.05E20");
    bool result = d1.isGreater(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128GreaterCase3) {
    Decimal128 d1("-INFINITY");
    Decimal128 d2("+INFINITY");
    bool result = d1.isGreater(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128GreaterEqualCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-25.05E20");
    bool result = d1.isGreaterEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128GreaterEqualCase2) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("25.05E20");
    bool result = d1.isGreaterEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128GreaterEqualCase3) {
    Decimal128 d1("-INFINITY");
    Decimal128 d2("+INFINITY");
    bool result = d1.isGreaterEqual(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128LessCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-25.05E20");
    bool result = d1.isLess(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128LessCase2) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("25.05E20");
    bool result = d1.isLess(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128LessCase3) {
    Decimal128 d1("-INFINITY");
    Decimal128 d2("+INFINITY");
    bool result = d1.isLess(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128LessEqualCase1) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("-25.05E20");
    bool result = d1.isLessEqual(d2);
    ASSERT_FALSE(result);
}

TEST(Decimal128Test, TestDecimal128LessEqualCase2) {
    Decimal128 d1("25.05E20");
    Decimal128 d2("25.05E20");
    bool result = d1.isLessEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128LessEqualCase3) {
    Decimal128 d1("-INFINITY");
    Decimal128 d2("+INFINITY");
    bool result = d1.isLessEqual(d2);
    ASSERT_TRUE(result);
}

TEST(Decimal128Test, TestDecimal128GetLargestPositive) {
    Decimal128 d = Decimal128::kLargestPositive;
    uint64_t largestPositiveDecimalHigh64 = 6917508178773903296ull;
    uint64_t largestPositveDecimalLow64 = 4003012203950112767ull;
    ASSERT_EQUALS(d.getValue().high64, largestPositiveDecimalHigh64);
    ASSERT_EQUALS(d.getValue().low64, largestPositveDecimalLow64);
}

TEST(Decimal128Test, TestDecimal128GetSmallestPositive) {
    Decimal128 d = Decimal128::kSmallestPositive;
    uint64_t smallestPositiveDecimalHigh64 = 0;
    uint64_t smallestPositiveDecimalLow64 = 1;
    ASSERT_EQUALS(d.getValue().high64, smallestPositiveDecimalHigh64);
    ASSERT_EQUALS(d.getValue().low64, smallestPositiveDecimalLow64);
}

TEST(Decimal128Test, TestDecimal128GetLargestNegative) {
    Decimal128 d = Decimal128::kLargestNegative;
    uint64_t largestNegativeDecimalHigh64 = 16140880215628679104ull;
    uint64_t largestNegativeDecimalLow64 = 4003012203950112767ull;
    ASSERT_EQUALS(d.getValue().high64, largestNegativeDecimalHigh64);
    ASSERT_EQUALS(d.getValue().low64, largestNegativeDecimalLow64);
}

TEST(Decimal128Test, TestDecimal128GetSmallestNegative) {
    Decimal128 d = Decimal128::kSmallestNegative;
    uint64_t smallestNegativeDecimalHigh64 = 9223372036854775808ull;
    uint64_t smallestNegativeDecimalLow64 = 1ull;
    ASSERT_EQUALS(d.getValue().high64, smallestNegativeDecimalHigh64);
    ASSERT_EQUALS(d.getValue().low64, smallestNegativeDecimalLow64);
}

TEST(Decimal128Test, TestDecimal128GetPosInfinity) {
    Decimal128 d = Decimal128::kPositiveInfinity;
    uint64_t decimalPositiveInfinityHigh64 = 8646911284551352320ull;
    uint64_t decimalPositiveInfinityLow64 = 0;
    ASSERT_EQUALS(d.getValue().high64, decimalPositiveInfinityHigh64);
    ASSERT_EQUALS(d.getValue().low64, decimalPositiveInfinityLow64);
}

TEST(Decimal128Test, TestDecimal128GetNegInfinity) {
    Decimal128 d = Decimal128::kNegativeInfinity;
    uint64_t decimalNegativeInfinityHigh64 = 17870283321406128128ull;
    uint64_t decimalNegativeInfinityLow64 = 0;
    ASSERT_EQUALS(d.getValue().high64, decimalNegativeInfinityHigh64);
    ASSERT_EQUALS(d.getValue().low64, decimalNegativeInfinityLow64);
}

TEST(Decimal128Test, TestDecimal128GetPosNaN) {
    Decimal128 d = Decimal128::kPositiveNaN;
    uint64_t decimalPositiveNaNHigh64 = 8935141660703064064ull;
    uint64_t decimalPositiveNaNLow64 = 0;
    ASSERT_EQUALS(d.getValue().high64, decimalPositiveNaNHigh64);
    ASSERT_EQUALS(d.getValue().low64, decimalPositiveNaNLow64);
}

TEST(Decimal128Test, TestDecimal128GetNegNaN) {
    Decimal128 d = Decimal128::kNegativeNaN;
    uint64_t decimalNegativeNaNHigh64 = 18158513697557839872ull;
    uint64_t decimalNegativeNaNLow64 = 0;
    ASSERT_EQUALS(d.getValue().high64, decimalNegativeNaNHigh64);
    ASSERT_EQUALS(d.getValue().low64, decimalNegativeNaNLow64);
}

}  // namespace mongo