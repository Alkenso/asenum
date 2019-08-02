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

#include <gmock/gmock.h>

#include <string>

using namespace ::testing;

namespace
{
    enum class TestEnum
    {
        Unknown,
        StringOpt,
        VoidOpt
    };
    
    using TestAsEnum = asenum::AsEnum<
    asenum::Case11<TestEnum, TestEnum::Unknown, int>,
    asenum::Case11<TestEnum, TestEnum::StringOpt, std::string>,
    asenum::Case11<TestEnum, TestEnum::VoidOpt, void>
    >;
    
    static_assert(std::is_same<TestAsEnum::UnderlyingType<TestEnum::Unknown>, int>::value, "Invalid underlying type");
    static_assert(std::is_same<TestAsEnum::UnderlyingType<TestEnum::StringOpt>, std::string>::value, "Invalid underlying type");
    static_assert(std::is_same<TestAsEnum::UnderlyingType<TestEnum::VoidOpt>, void>::value, "Invalid underlying type");
    
    static_assert(GTEST_ARRAY_SIZE_(TestAsEnum::AllCases) == 3, "Invalid number of cases");
    static_assert(TestAsEnum::AllCases[0] == TestEnum::Unknown, "Invalid enum case");
    static_assert(TestAsEnum::AllCases[1] == TestEnum::StringOpt, "Invalid enum case");
    static_assert(TestAsEnum::AllCases[2] == TestEnum::VoidOpt, "Invalid enum case");
}

TEST(AsEnum, IfCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::VoidOpt>();
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown>(-100500);
    
    MockFunction<void(std::string)> handler1;
    MockFunction<void(void)> handler2;
    MockFunction<void(int)> handler3;
    
    EXPECT_CALL(handler1, Call("test"))
    .WillOnce(Return());
    EXPECT_CALL(handler2, Call())
    .WillOnce(Return());
    EXPECT_CALL(handler3, Call(-100500))
    .WillOnce(Return());
    
    EXPECT_TRUE(value1.ifCase<TestEnum::StringOpt>(handler1.AsStdFunction()));
    EXPECT_FALSE(value1.ifCase<TestEnum::VoidOpt>(handler2.AsStdFunction()));
    EXPECT_FALSE(value1.ifCase<TestEnum::Unknown>(handler3.AsStdFunction()));
    
    EXPECT_FALSE(value2.ifCase<TestEnum::StringOpt>(handler1.AsStdFunction()));
    EXPECT_TRUE(value2.ifCase<TestEnum::VoidOpt>(handler2.AsStdFunction()));
    EXPECT_FALSE(value2.ifCase<TestEnum::Unknown>(handler3.AsStdFunction()));
    
    EXPECT_FALSE(value3.ifCase<TestEnum::StringOpt>(handler1.AsStdFunction()));
    EXPECT_FALSE(value3.ifCase<TestEnum::VoidOpt>(handler2.AsStdFunction()));
    EXPECT_TRUE(value3.ifCase<TestEnum::Unknown>(handler3.AsStdFunction()));
}

TEST(AsEnum, IsCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::VoidOpt>();
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown>(-100500);
    
    EXPECT_EQ(value1.enumCase(), TestEnum::StringOpt);
    EXPECT_EQ(value2.enumCase(), TestEnum::VoidOpt);
    EXPECT_EQ(value3.enumCase(), TestEnum::Unknown);
    
    EXPECT_TRUE(value1.isCase<TestEnum::StringOpt>());
    EXPECT_FALSE(value1.isCase<TestEnum::VoidOpt>());
    EXPECT_FALSE(value1.isCase<TestEnum::Unknown>());
    
    EXPECT_FALSE(value2.isCase<TestEnum::StringOpt>());
    EXPECT_TRUE(value2.isCase<TestEnum::VoidOpt>());
    EXPECT_FALSE(value2.isCase<TestEnum::Unknown>());
    
    EXPECT_FALSE(value3.isCase<TestEnum::StringOpt>());
    EXPECT_FALSE(value3.isCase<TestEnum::VoidOpt>());
    EXPECT_TRUE(value3.isCase<TestEnum::Unknown>());
}

TEST(AsEnum, Switch_Full)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt>("test");
    
    MockFunction<void(std::string)> handler;
    
    EXPECT_CALL(handler, Call("test"))
    .WillOnce(Return());
    
    value.doSwitch()
    .ifCase<TestEnum::StringOpt>(handler.AsStdFunction())
    .ifCase<TestEnum::VoidOpt>([] {
        EXPECT_TRUE(false);
    })
    .ifCase<TestEnum::Unknown>([] (const int& value) {
        EXPECT_TRUE(false);
    })
    .ifDefault([] () {
        EXPECT_TRUE(false);
    });
}

TEST(AsEnum, Switch_Partial)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt>("test");
    
    MockFunction<void(std::string)> handler;
    
    EXPECT_CALL(handler, Call("test"))
    .WillOnce(Return());
    
    value.doSwitch()
    .ifCase<TestEnum::StringOpt>(handler.AsStdFunction())
    .ifCase<TestEnum::VoidOpt>([] {
        EXPECT_TRUE(false);
    });
}

TEST(AsEnum, Switch_Default)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt>("test");
    
    MockFunction<void(void)> handler;
    
    EXPECT_CALL(handler, Call())
    .WillOnce(Return());
    
    value.doSwitch()
    .ifCase<TestEnum::Unknown>([] (const int& value) {
        EXPECT_TRUE(false);
    })
    .ifCase<TestEnum::VoidOpt>([] {
        EXPECT_TRUE(false);
    })
    .ifDefault(handler.AsStdFunction());
}
