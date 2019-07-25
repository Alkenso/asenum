/*
 * MIT License
 *
 * Copyright (c) 2019 Alkenso (Vladimir Vashurkin)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <asenum/asenum.h>

#include <gtest/gtest.h>

#include <string>

using namespace ::testing;

namespace
{
    enum class TestEnum
    {
        Unknown,
        StringOpt,
        BoolOpt
    };
    
    ASENUM_DECLARE(TestAsEnum, TestEnum)
    {
        ASENUM_DEFINE_STRUCTORS();

        ASENUM_CASE(Unknown, int);
        ASENUM_CASE(StringOpt, std::string);
        ASENUM_CASE(BoolOpt, bool);
    };
}

TEST(AsEnum, NamedGetter)
{
	const TestAsEnum value1 = TestAsEnum::createStringOpt("test");
	const TestAsEnum value2 = TestAsEnum::createBoolOpt(true);
	const TestAsEnum value3 = TestAsEnum::createUnknown(-100500);
	
	EXPECT_EQ(value1.type(), TestEnum::StringOpt);
	EXPECT_EQ(value2.type(), TestEnum::BoolOpt);
	EXPECT_EQ(value3.type(), TestEnum::Unknown);
	
	EXPECT_EQ(value1.asStringOpt(), "test");
	EXPECT_THROW(value1.asUnknown(), std::exception);
	EXPECT_THROW(value1.asBoolOpt(), std::exception);
	
	EXPECT_EQ(value2.asBoolOpt(), true);
	EXPECT_THROW(value2.asUnknown(), std::exception);
	EXPECT_THROW(value2.asStringOpt(), std::exception);
	
	EXPECT_EQ(value3.asUnknown(), -100500);
	EXPECT_THROW(value3.asStringOpt(), std::exception);
	EXPECT_THROW(value3.asBoolOpt(), std::exception);
	
}

TEST(AsEnum, EnumGetter)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::BoolOpt>(true);
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown>(-100500);
    
    EXPECT_EQ(value1.type(), TestEnum::StringOpt);
    EXPECT_EQ(value2.type(), TestEnum::BoolOpt);
    EXPECT_EQ(value3.type(), TestEnum::Unknown);
    
    EXPECT_EQ(value1.as<TestEnum::StringOpt>(), "test");
    EXPECT_THROW(value1.as<TestEnum::Unknown>(), std::exception);
    EXPECT_THROW(value1.as<TestEnum::BoolOpt>(), std::exception);
    
    EXPECT_EQ(value2.as<TestEnum::BoolOpt>(), true);
    EXPECT_THROW(value2.as<TestEnum::Unknown>(), std::exception);
    EXPECT_THROW(value2.as<TestEnum::StringOpt>(), std::exception);
    
    EXPECT_EQ(value3.as<TestEnum::Unknown>(), -100500);
    EXPECT_THROW(value3.as<TestEnum::StringOpt>(), std::exception);
    EXPECT_THROW(value3.as<TestEnum::BoolOpt>(), std::exception);
    
}

TEST(AsEnum, Is)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt>("test");
    
    EXPECT_TRUE(value.is<TestEnum::StringOpt>());
    EXPECT_TRUE(value.isStringOpt());
    
    EXPECT_FALSE(value.is<TestEnum::Unknown>());
    EXPECT_FALSE(value.isUnknown());
    
    EXPECT_FALSE(value.is<TestEnum::BoolOpt>());
    EXPECT_FALSE(value.isBoolOpt());
}

TEST(AsEnum, Switch_Full)
{
    const TestAsEnum value1 = TestAsEnum::createStringOpt("test");
    
    value1.doSwitch()
    .asCase<TestEnum::StringOpt>([] (const std::string& value) {
        EXPECT_EQ(value, "test");
    })
    .asCase<TestEnum::BoolOpt>([] (const bool& value) {
        EXPECT_TRUE(false);
    })
    .asCase<TestEnum::Unknown>([] (const int& value) {
        EXPECT_TRUE(false);
    })
    .asDefault([] () {
        EXPECT_TRUE(false);
    });
}

TEST(AsEnum, Switch_Partial)
{
    const TestAsEnum value1 = TestAsEnum::createStringOpt("test");
    
    value1.doSwitch()
    .asCase<TestEnum::StringOpt>([] (const std::string& value) {
        EXPECT_EQ(value, "test");
    })
    .asCase<TestEnum::BoolOpt>([] (const bool& value) {
        EXPECT_TRUE(false);
    });
}

TEST(AsEnum, Switch_Default)
{
    const TestAsEnum value1 = TestAsEnum::createStringOpt("test");
    
    value1.doSwitch()
    .asCase<TestEnum::Unknown>([] (const int& value) {
        EXPECT_TRUE(false);
    })
    .asCase<TestEnum::BoolOpt>([] (const bool& value) {
        EXPECT_TRUE(false);
    })
    .asDefault([] () {
        EXPECT_TRUE(true);
    });;
}
