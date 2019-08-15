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
        StringOpt1,
        VoidOpt2,
        Unknown3
    };
    
    using TestAsEnum = asenum::AsEnum<
    asenum::Case11<TestEnum, TestEnum::Unknown3, int>,
    asenum::Case11<TestEnum, TestEnum::StringOpt1, std::string>,
    asenum::Case11<TestEnum, TestEnum::VoidOpt2, void>
    >;
    
    static_assert(std::is_same<TestAsEnum::UnderlyingType<TestEnum::Unknown3>, int>::value, "Invalid underlying type");
    static_assert(std::is_same<TestAsEnum::UnderlyingType<TestEnum::StringOpt1>, std::string>::value, "Invalid underlying type");
    static_assert(std::is_same<TestAsEnum::UnderlyingType<TestEnum::VoidOpt2>, void>::value, "Invalid underlying type");
    
    static_assert(GTEST_ARRAY_SIZE_(TestAsEnum::AllCases) == 3, "Invalid number of cases");
    static_assert(TestAsEnum::AllCases[0] == TestEnum::Unknown3, "Invalid enum case");
    static_assert(TestAsEnum::AllCases[1] == TestEnum::StringOpt1, "Invalid enum case");
    static_assert(TestAsEnum::AllCases[2] == TestEnum::VoidOpt2, "Invalid enum case");
    
    
    enum class SomeVoidEnum
    {
        Opt1,
        Opt2
    };
    
    using SomeVoidAsEnum = asenum::AsEnum<
    asenum::Case11<SomeVoidEnum, SomeVoidEnum::Opt1, void>,
    asenum::Case11<SomeVoidEnum, SomeVoidEnum::Opt2, void>
    >;
}

TEST(AsEnum, IfCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::VoidOpt2>();
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown3>(-100500);
    
    MockFunction<void(std::string)> handler1;
    MockFunction<void(void)> handler2;
    MockFunction<void(int)> handler3;
    
    EXPECT_CALL(handler1, Call("test"))
    .WillOnce(Return());
    EXPECT_CALL(handler2, Call())
    .WillOnce(Return());
    EXPECT_CALL(handler3, Call(-100500))
    .WillOnce(Return());
    
    EXPECT_TRUE(value1.ifCase<TestEnum::StringOpt1>(handler1.AsStdFunction()));
    EXPECT_FALSE(value1.ifCase<TestEnum::VoidOpt2>(handler2.AsStdFunction()));
    EXPECT_FALSE(value1.ifCase<TestEnum::Unknown3>(handler3.AsStdFunction()));
    
    EXPECT_FALSE(value2.ifCase<TestEnum::StringOpt1>(handler1.AsStdFunction()));
    EXPECT_TRUE(value2.ifCase<TestEnum::VoidOpt2>(handler2.AsStdFunction()));
    EXPECT_FALSE(value2.ifCase<TestEnum::Unknown3>(handler3.AsStdFunction()));
    
    EXPECT_FALSE(value3.ifCase<TestEnum::StringOpt1>(handler1.AsStdFunction()));
    EXPECT_FALSE(value3.ifCase<TestEnum::VoidOpt2>(handler2.AsStdFunction()));
    EXPECT_TRUE(value3.ifCase<TestEnum::Unknown3>(handler3.AsStdFunction()));
}

TEST(AsEnum, IsCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::VoidOpt2>();
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown3>(-100500);
    
    EXPECT_EQ(value1.enumCase(), TestEnum::StringOpt1);
    EXPECT_EQ(value2.enumCase(), TestEnum::VoidOpt2);
    EXPECT_EQ(value3.enumCase(), TestEnum::Unknown3);
    
    EXPECT_TRUE(value1.isCase<TestEnum::StringOpt1>());
    EXPECT_FALSE(value1.isCase<TestEnum::VoidOpt2>());
    EXPECT_FALSE(value1.isCase<TestEnum::Unknown3>());
    
    EXPECT_FALSE(value2.isCase<TestEnum::StringOpt1>());
    EXPECT_TRUE(value2.isCase<TestEnum::VoidOpt2>());
    EXPECT_FALSE(value2.isCase<TestEnum::Unknown3>());
    
    EXPECT_FALSE(value3.isCase<TestEnum::StringOpt1>());
    EXPECT_FALSE(value3.isCase<TestEnum::VoidOpt2>());
    EXPECT_TRUE(value3.isCase<TestEnum::Unknown3>());
}

TEST(AsEnum, Switch_Full)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt1>("test");
    
    MockFunction<void(std::string)> handler;
    
    EXPECT_CALL(handler, Call("test"))
    .WillOnce(Return());
    
    value.doSwitch()
    .ifCase<TestEnum::StringOpt1>(handler.AsStdFunction())
    .ifCase<TestEnum::VoidOpt2>([] {
        EXPECT_TRUE(false);
    })
    .ifCase<TestEnum::Unknown3>([] (const int& value) {
        EXPECT_TRUE(false);
    })
    .ifDefault([] () {
        EXPECT_TRUE(false);
    });
}

TEST(AsEnum, Switch_Partial)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt1>("test");
    
    MockFunction<void(std::string)> handler;
    
    EXPECT_CALL(handler, Call("test"))
    .WillOnce(Return());
    
    value.doSwitch()
    .ifCase<TestEnum::StringOpt1>(handler.AsStdFunction())
    .ifCase<TestEnum::VoidOpt2>([] {
        EXPECT_TRUE(false);
    });
}

TEST(AsEnum, Switch_Default)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt1>("test");
    
    MockFunction<void(void)> handler;
    
    EXPECT_CALL(handler, Call())
    .WillOnce(Return());
    
    value.doSwitch()
    .ifCase<TestEnum::Unknown3>([] (const int& value) {
        EXPECT_TRUE(false);
    })
    .ifCase<TestEnum::VoidOpt2>([] {
        EXPECT_TRUE(false);
    })
    .ifDefault(handler.AsStdFunction());
}

TEST(AsEnum, Map_With_Default)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt1>("test");
    
    const bool vv = value.doMap<bool>()
    .ifCase<TestEnum::StringOpt1>([] (const std::string& value) {
        return true;
    })
    .ifCase<TestEnum::VoidOpt2>([] {
        return false;
    })
    .ifDefault([] {
        return false;
    });
    
    EXPECT_TRUE(vv);
}

TEST(AsEnum, Map_All_Cases)
{
    const TestAsEnum value = TestAsEnum::create<TestEnum::StringOpt1>("test");
    
    const bool vv = value.doMap<bool>()
    .ifCase<TestEnum::Unknown3>([] (const int& value) {
        return false;
    })
    .ifCase<TestEnum::VoidOpt2>([] {
        return false;
    })
    .ifCase<TestEnum::StringOpt1>([] (const std::string& value) {
        return true;
    });

    EXPECT_TRUE(vv);
}

TEST(AsEnum, ForceAsCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    // TestEnum::VoidOpt doesn't have 'forceAsEnum' method because associated type is 'void'.
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown3>(-100500);
    
    EXPECT_EQ(value1.forceAsCase<TestEnum::StringOpt1>(), "test");
    EXPECT_THROW(value1.forceAsCase<TestEnum::Unknown3>(), std::invalid_argument);
    
    EXPECT_THROW(value3.forceAsCase<TestEnum::StringOpt1>(), std::invalid_argument);
    EXPECT_EQ(value3.forceAsCase<TestEnum::Unknown3>(), -100500);
}

TEST(AsEnum, Equality)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::StringOpt1>("test2");
    const TestAsEnum value4 = TestAsEnum::create<TestEnum::VoidOpt2>();
    const TestAsEnum value5 = TestAsEnum::create<TestEnum::Unknown3>(-100500);
    
    EXPECT_EQ(value1, value1);
    EXPECT_EQ(value1, value2);
    EXPECT_NE(value1, value3);
    EXPECT_NE(value1, value4);
    EXPECT_NE(value1, value5);
    
    EXPECT_EQ(value4, TestAsEnum::create<TestEnum::VoidOpt2>());
}

TEST(AsEnum, Equality_Void)
{
    const SomeVoidAsEnum value1 = SomeVoidAsEnum::create<SomeVoidEnum::Opt1>();
    const SomeVoidAsEnum value2 = SomeVoidAsEnum::create<SomeVoidEnum::Opt1>();
    const SomeVoidAsEnum value3 = SomeVoidAsEnum::create<SomeVoidEnum::Opt2>();
    
    EXPECT_EQ(value1, value1);
    EXPECT_EQ(value1, value2);
    EXPECT_NE(value1, value3);
}

TEST(AsEnum, Compare_SameCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::StringOpt1>("test2");
    
    EXPECT_LT(value1, value3);
    EXPECT_LE(value1, value2);
    EXPECT_GT(value3, value1);
    EXPECT_GE(value1, value2);
}

TEST(AsEnum, Compare_SameCase_Void)
{
    const SomeVoidAsEnum value1 = SomeVoidAsEnum::create<SomeVoidEnum::Opt1>();
    const SomeVoidAsEnum value2 = SomeVoidAsEnum::create<SomeVoidEnum::Opt1>();
    const SomeVoidAsEnum value3 = SomeVoidAsEnum::create<SomeVoidEnum::Opt2>();
    
    EXPECT_LT(value1, value3);
    EXPECT_LE(value1, value2);
    EXPECT_GT(value3, value1);
    EXPECT_GE(value1, value2);
}

TEST(AsEnum, Compare_RandomCase)
{
    const TestAsEnum value1 = TestAsEnum::create<TestEnum::StringOpt1>("test");
    const TestAsEnum value2 = TestAsEnum::create<TestEnum::VoidOpt2>();
    const TestAsEnum value3 = TestAsEnum::create<TestEnum::Unknown3>(-100500);
    
    EXPECT_LT(value1, value2);
    EXPECT_LT(value2, value3);
    
    EXPECT_LE(value1, value1);
    EXPECT_LE(value1, value2);
    EXPECT_LE(value1, value3);
    EXPECT_LE(value2, value2);
    EXPECT_LE(value2, value3);
    EXPECT_LE(value3, value3);
    
    EXPECT_GT(value3, value2);
    EXPECT_GT(value3, value1);
    
    EXPECT_GE(value3, value3);
    EXPECT_GE(value3, value2);
    EXPECT_GE(value3, value1);
    EXPECT_GE(value2, value2);
    EXPECT_GE(value2, value1);
    EXPECT_GE(value1, value1);
}
